#include "Logger.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::~Logger() {
    if (m_logFile.is_open()) {
        m_logFile.close();
    }
}

bool Logger::initialize(const std::string& filename) {
    // Если уже инициализирован - закрываем предыдущий файл
    if (m_logFile.is_open()) {
        m_logFile.close();
    }
    
    m_logFile.open(filename, std::ios::app);
    if (!m_logFile.is_open()) {
        std::cerr << "Ошибка: Не удалось открыть файл журнала: " << filename << std::endl;
        m_initialized = false;
        return false;
    }
    
    m_filename = filename;
    m_initialized = true;
    
    std::cout << "Файл журнала: " << filename << std::endl;
    return true;
}

void Logger::log(LogLevel level, const std::string& message, const std::string& params) {
    if (!m_initialized) {
        return;
    }
    
    std::string levelStr = (level == LogLevel::INFO) ? "ИНФО" : "ОШИБКА";
    std::string logEntry = getCurrentTime() + " [" + levelStr + "] " + message;
    
    if (!params.empty()) {
        logEntry += " (" + params + ")";
    }
    
    // Вывод в консоль
    std::cout << logEntry << std::endl;
    
    // Запись в файл
    if (m_logFile.is_open()) {
        m_logFile << logEntry << std::endl;
        m_logFile.flush(); // Принудительно сбрасываем буфер
    }
}

void Logger::reset() {
    if (m_logFile.is_open()) {
        m_logFile.close();
    }
    m_initialized = false;
    m_filename.clear();
}

std::string Logger::getCurrentTime() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}