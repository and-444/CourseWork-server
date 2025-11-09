#include "Server.h"
#include "Logger.h"
#include <iostream>
#include <string>
#include <cstring>
#include <getopt.h>
#include <unistd.h>

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]\n"
              << "Options:\n"
              << "  -c, --config FILE    User database file (default: /etc/vcalc.conf)\n"
              << "  -l, --log FILE       Log file (default: /var/log/vcalc.log)\n"
              << "  -p, --port PORT      Server port (default: 33333)\n"
              << "  -h, --help           Show this help message\n";
}

int main(int argc, char* argv[]) {
    std::string configFile = "/etc/vcalc.conf";
    std::string logFile = "/var/log/vcalc.log";
    uint16_t port = 33333;
    
    // Обработка аргументов командной строки
    static struct option long_options[] = {
        {"config", required_argument, 0, 'c'},
        {"log", required_argument, 0, 'l'},
        {"port", required_argument, 0, 'p'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };
    
    int option;
    while ((option = getopt_long(argc, argv, "c:l:p:h", long_options, nullptr)) != -1) {
        switch (option) {
            case 'c':
                configFile = optarg;
                break;
            case 'l':
                logFile = optarg;
                break;
            case 'p':
                port = static_cast<uint16_t>(std::stoi(optarg));
                break;
            case 'h':
                printUsage(argv[0]);
                return 0;
            default:
                printUsage(argv[0]);
                return 1;
        }
    }
    
    // Вывод информации о путях
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        std::cout << "Current working directory: " << cwd << std::endl;
    }
    std::cout << "Config file path: " << configFile << std::endl;
    std::cout << "Log file path: " << logFile << std::endl;
    
    // Проверка существования файла конфигурации
    std::ifstream testFile(configFile);
    if (!testFile.is_open()) {
        std::cerr << "ERROR: Cannot open config file: " << configFile << std::endl;
        std::cerr << "Please check the file path and permissions." << std::endl;
        return 1;
    }
    testFile.close();
    
    // Запуск сервера
    Server server(configFile, logFile, port);
    
    if (!server.initialize()) {
        std::cerr << "Failed to initialize server" << std::endl;
        return 1;
    }
    
    std::cout << "Server starting with configuration:\n"
              << "  Config file: " << configFile << "\n"
              << "  Log file: " << logFile << "\n"
              << "  Port: " << port << std::endl;
    
    server.run();
    
    return 0;
}