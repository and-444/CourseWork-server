#include "Logger.h"
#include <iostream>
#include <chrono>
#include <iomanip>

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::initialize(const std::string& filename) {
    std::lock_guard<std::mutex> lock(logMutex_);
    logFile_.open(filename, std::ios::app);
    initialized_ = logFile_.is_open();
}

void Logger::log(const std::string& message, bool isError) {
    std::lock_guard<std::mutex> lock(logMutex_);
    
    // Получение текущего времени
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    
    std::string logEntry = "[" + ss.str() + "] " + 
                          (isError ? "ERROR: " : "INFO: ") + message;
    
    // Вывод в консоль
    std::cout << logEntry << std::endl;
    
    // Запись в файл
    if (initialized_) {
        logFile_ << logEntry << std::endl;
        logFile_.flush();
    }
}