#include <iostream>
#include <string>
#include "Server.h"
#include "Logger.h"

void showHelp() {
    std::cout << "Использование: server [опции]\n"
              << "Опции:\n"
              << "  -h, --help          Показать эту справку\n"
              << "  -c, --config FILE   Файл базы пользователей (по умолчанию: /etc/vcalc.conf)\n"
              << "  -l, --log FILE      Файл журнала (по умолчанию: /var/log/vcalc.log)\n"
              << "  -p, --port PORT     Номер порта (по умолчанию: 33333)\n";
}

int main(int argc, char* argv[]) {
    std::string userDbFile = "/etc/vcalc.conf";
    std::string logFile = "/var/log/vcalc.log";
    uint16_t port = 33333;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            showHelp();
            return 0;
        } else if ((arg == "-c" || arg == "--config") && i + 1 < argc) {
            userDbFile = argv[++i];
        } else if ((arg == "-l" || arg == "--log") && i + 1 < argc) {
            logFile = argv[++i];
        } else if ((arg == "-p" || arg == "--port") && i + 1 < argc) {
            port = static_cast<uint16_t>(std::stoi(argv[++i]));
        } else {
            std::cerr << "Неизвестный параметр: " << arg << std::endl;
            showHelp();
            return 1;
        }
    }
    
    std::cout << "Запуск сервера..." << std::endl;
    std::cout << "База пользователей: " << userDbFile << std::endl;
    std::cout << "Файл журнала: " << logFile << std::endl;
    std::cout << "Порт: " << port << std::endl;
    
    Server server;
    if (!server.initialize(userDbFile, logFile, port)) {
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