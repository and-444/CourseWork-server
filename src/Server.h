/**
 * @file Server.h
 * @brief Основной класс сервера
 * @author Савин А.В.
 * @version 1.0
 */

#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <cstdint>

/**
 * @class Server
 * @brief Основной класс TCP-сервера
 * 
 * Класс реализует функциональность сетевого сервера для обработки клиентских запросов,
 * включая аутентификацию и вычисление произведения векторов.
 */
class Server {
public:
    Server();

    /**
     * @brief Инициализация сервера
     * @param userDbFile Путь к файлу базы пользователей
     * @param logFile Путь к файлу журнала
     * @param port Порт для прослушивания
     * @return true если инициализация успешна, false в случае ошибки
     */
    bool initialize(const std::string& userDbFile, const std::string& logFile, uint16_t port);

      /**
     * @brief Запуск основного цикла сервера
     * @throws std::runtime_error в случае ошибок в работе сервера
     */
    void run();

    /**
     * @brief Остановка сервера
     */
    void stop();
    
private:
    int m_serverSocket;       ///< Дескриптор серверного сокета
    uint16_t m_port;          ///< Порт сервера
    bool m_running;           ///< Флаг работы сервера
    std::string m_userDbFile; ///< Путь к файлу базы пользователей
    
    // Приватные методы
    bool createSocket();
    bool bindSocket();
    bool startListening();
    void handleClient(int clientSocket);
    bool authenticateClient(int clientSocket);
    void processVectors(int clientSocket);
};

#endif