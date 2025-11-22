#include <UnitTest++/UnitTest++.h>
#include <fstream>
#include <string>
#include <vector>
#include <cstdio>
#include <thread>
#include <chrono>
#include "Logger.h"
#include "logger_reset.h"

// Сброс логгера перед каждым тестом
struct LoggerFixture {
    LoggerFixture() {
        Logger::getInstance().reset();
    }
};

SUITE(LoggerTests) {
    
    TEST_FIXTURE(LoggerFixture, LoggerSingletonPattern) {
        Logger& logger1 = Logger::getInstance();
        Logger& logger2 = Logger::getInstance();
        
        CHECK(&logger1 == &logger2);
    }
    
    TEST_FIXTURE(LoggerFixture, LoggerInitializationCreatesFile) {
        std::string filename = "test_init_create.log";
        
        // Удаляем файл если существует
        std::remove(filename.c_str());
        
        Logger& logger = Logger::getInstance();
        bool result = logger.initialize(filename);
        
        CHECK_EQUAL(true, result);
        
        // Проверяем что файл создался
        std::ifstream testFile(filename);
        bool fileExists = testFile.is_open();
        testFile.close();
        
        CHECK_EQUAL(true, fileExists);
        
        // Очистка
        std::remove(filename.c_str());
    }
    
    TEST_FIXTURE(LoggerFixture, LoggerCanWriteToFile) {
        std::string filename = "test_write.log";
        std::remove(filename.c_str());
        
        Logger& logger = Logger::getInstance();
        logger.initialize(filename);
        
        // Записываем тестовое сообщение
        logger.log(LogLevel::INFO, "TEST_WRITE_OPERATION");
        
        // Даем время для записи на диск
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Проверяем что файл не пустой
        std::ifstream file(filename);
        if (file.is_open()) {
            std::string content;
            std::getline(file, content);
            file.close();
            
            CHECK(!content.empty());
            CHECK(content.find("TEST_WRITE_OPERATION") != std::string::npos);
        } else {
            CHECK(false); // Файл должен открываться
        }
        
        std::remove(filename.c_str());
    }
    
    TEST_FIXTURE(LoggerFixture, LoggerHandlesDifferentLogLevels) {
        std::string filename = "test_levels.log";
        std::remove(filename.c_str());
        
        Logger& logger = Logger::getInstance();
        logger.initialize(filename);
        
        // Записываем сообщения разных уровней
        logger.log(LogLevel::INFO, "INFO_LEVEL_TEST");
        logger.log(LogLevel::ERROR, "ERROR_LEVEL_TEST");
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Проверяем запись
        std::ifstream file(filename);
        if (file.is_open()) {
            std::string line;
            bool foundInfo = false;
            bool foundError = false;
            
            while (std::getline(file, line)) {
                if (line.find("INFO_LEVEL_TEST") != std::string::npos) foundInfo = true;
                if (line.find("ERROR_LEVEL_TEST") != std::string::npos) foundError = true;
            }
            file.close();
            
            CHECK(foundInfo);
            CHECK(foundError);
        }
        
        std::remove(filename.c_str());
    }
    
    TEST_FIXTURE(LoggerFixture, LoggerFormatIncludesTimestampAndLevel) {
        std::string filename = "test_format.log";
        std::remove(filename.c_str());
        
        Logger& logger = Logger::getInstance();
        logger.initialize(filename);
        
        logger.log(LogLevel::INFO, "FORMAT_TEST_MESSAGE");
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        std::ifstream file(filename);
        if (file.is_open()) {
            std::string line;
            std::getline(file, line);
            file.close();
            
            // Проверяем базовые элементы формата
            CHECK(line.find("FORMAT_TEST_MESSAGE") != std::string::npos);
            CHECK(line.find("[ИНФО]") != std::string::npos);
            CHECK(line.length() > 20); // Должна быть дата + сообщение
            
        } else {
            CHECK(false);
        }
        
        std::remove(filename.c_str());
    }
    
    TEST_FIXTURE(LoggerFixture, LoggerHandlesParameters) {
        std::string filename = "test_params.log";
        std::remove(filename.c_str());
        
        Logger& logger = Logger::getInstance();
        logger.initialize(filename);
        
        logger.log(LogLevel::INFO, "MESSAGE_WITH_PARAMS", "param1=value1,param2=value2");
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        std::ifstream file(filename);
        if (file.is_open()) {
            std::string line;
            std::getline(file, line);
            file.close();
            
            CHECK(line.find("MESSAGE_WITH_PARAMS") != std::string::npos);
            CHECK(line.find("param1=value1") != std::string::npos);
            
        } else {
            CHECK(false);
        }
        
        std::remove(filename.c_str());
    }
};