#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>

enum class LogLevel {
    INFO,
    ERROR
};

class Logger {
public:
    static Logger& getInstance();
    bool initialize(const std::string& filename);
    void log(LogLevel level, const std::string& message, const std::string& params = "");
    
    // Метод для тестирования - принудительная очистка
    void reset();
    
private:
    Logger() = default;
    ~Logger();
    std::string getCurrentTime();
    
    std::ofstream m_logFile;
    std::string m_filename;
    bool m_initialized = false;
};

#endif