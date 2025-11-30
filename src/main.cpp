#include <iostream>
#include <string>
#include "Server.h"
#include "Logger.h"
#include "Interface.h"

int main(int argc, char* argv[]) {
    Interface interface;
    
    // Парсинг аргументов командной строки
    if (!interface.parse(argc, argv)) {
        interface.printHelp();
        return 0;
    }
    
    // Получаем параметры
    Params params = interface.getParams();
    
    std::cout << "Запуск сервера с параметрами:" << std::endl;
    std::cout << "  База пользователей: " << params.dbFile << std::endl;
    std::cout << "  Файл журнала: " << params.logFile << std::endl;
    std::cout << "  Порт: " << params.port << std::endl;
    
    Server server;
    if (!server.initialize(params.dbFile, params.logFile, params.port)) {
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