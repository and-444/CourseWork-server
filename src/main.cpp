#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <limits.h>
#include "Server.h"
#include "Logger.h"

void showHelp(const char* programName) {
    std::cout << "Использование: " << programName << " [опции]\n"
              << "Опции:\n"
              << "  -h, --help          Показать эту справку\n"
              << "  -c, --config FILE   Файл базы пользователей (по умолчанию: /etc/vcalc.conf)\n"
              << "  -l, --log FILE      Файл журнала (по умолчанию: /var/log/vcalc.log)\n"
              << "  -p, --port PORT     Номер порта (по умолчанию: 33333, диапазон: 1-65535)\n"
              << "\nПример:\n"
              << "  " << programName << " -c ./vcalc.conf -l ./vcalc.log -p 33333\n";
}

bool fileExists(const std::string& filename) {
    return access(filename.c_str(), F_OK) != -1;
}

bool validatePort(int port) {
    return port > 0 && port <= 65535;
}

int main(int argc, char* argv[]) {
    std::string userDbFile = "/etc/vcalc.conf";
    std::string logFile = "/var/log/vcalc.log";
    int port = 33333;
    
    // Парсинг аргументов командной строки с использованием getopt
    int opt;
    while ((opt = getopt(argc, argv, "hc:l:p:")) != -1) {
        switch (opt) {
            case 'h':
                showHelp(argv[0]);
                return 0;
            case 'c':
                userDbFile = optarg;
                break;
            case 'l':
                logFile = optarg;
                break;
            case 'p':
                try {
                    port = std::stoi(optarg);
                    if (!validatePort(port)) {
                        std::cerr << "Ошибка: Порт должен быть в диапазоне 1-65535" << std::endl;
                        return 1;
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Ошибка: Неверный формат порта: " << optarg << std::endl;
                    return 1;
                }
                break;
            case '?':
                std::cerr << "Неизвестный параметр или отсутствует значение" << std::endl;
                showHelp(argv[0]);
                return 1;
            default:
                showHelp(argv[0]);
                return 1;
        }
    }
    
    // Проверка существования файла конфигурации
    if (!fileExists(userDbFile)) {
        std::cerr << "Ошибка: Файл конфигурации не существует: " << userDbFile << std::endl;
        std::cerr << "Используется стандартный файл: /etc/vcalc.conf" << std::endl;
        userDbFile = "/etc/vcalc.conf";
        
        // Проверяем существование стандартного файла
        if (!fileExists(userDbFile)) {
            std::cerr << "Ошибка: Стандартный файл конфигурации также не существует" << std::endl;
            std::cerr << "Создайте файл конфигурации или укажите правильный путь с помощью -c" << std::endl;
            return 1;
        }
    }
    
    // Проверка возможности создания/записи лог-файла
    std::ofstream testLog(logFile, std::ios::app);
    if (!testLog.is_open()) {
        std::cerr << "Ошибка: Не удается открыть файл журнала для записи: " << logFile << std::endl;
        std::cerr << "Проверьте права доступа к директории" << std::endl;
        return 1;
    }
    testLog.close();
    
    std::cout << "Запуск сервера с параметрами:" << std::endl;
    std::cout << "  База пользователей: " << userDbFile << std::endl;
    std::cout << "  Файл журнала: " << logFile << std::endl;
    std::cout << "  Порт: " << port << std::endl;
    
    Server server;
    if (!server.initialize(userDbFile, logFile, static_cast<uint16_t>(port))) {
        std::cerr << "Ошибка: Не удалось инициализировать сервер" << std::endl;
        return 1;
    }
    
    std::cout << "Сервер запущен. Для остановки нажмите Ctrl+C" << std::endl;
    
    try {
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Ошибка сервера: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}