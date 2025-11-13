#include "Logger.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

bool Logger::initialize(const std::string& filename) {
    m_logFile.open(filename, std::ios::app);
    if (!m_logFile.is_open()) {
        std::cerr << "Ошибка: Не удалось открыть файл журнала: " << filename << std::endl;
        return false;
    }
    
    std::cout << "Файл журнала: " << filename << std::endl;
    return true;
}

void Logger::log(LogLevel level, const std::string& message, const std::string& params) {
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
        m_logFile.flush();
    }
}

std::string Logger::getCurrentTime() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}