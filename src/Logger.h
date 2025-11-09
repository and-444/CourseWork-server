#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <mutex>

class Logger {
public:
    static Logger& getInstance();
    
    void initialize(const std::string& filename);
    void log(const std::string& message, bool isError = false);
    
private:
    Logger() = default;
    std::ofstream logFile_;
    std::mutex logMutex_;
    bool initialized_ = false;
};

#endif // LOGGER_H