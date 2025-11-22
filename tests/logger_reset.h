#ifndef LOGGER_RESET_H
#define LOGGER_RESET_H

#include "Logger.h"

struct LoggerReset {
    LoggerReset() {
        Logger::getInstance().reset();
    }
};

#endif