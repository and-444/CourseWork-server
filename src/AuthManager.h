/**
 * @file AuthManager.h
 * @brief Менеджер аутентификации пользователей
 * @author Савин А.В.
 * @version 1.0
 */

#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <string>
#include <unordered_map>
#include <random>

/**
 * @class AuthManager
 * @brief Управление аутентификацией пользователей
 * 
 * Класс отвечает за загрузку базы пользователей, генерацию соли,
 * вычисление хешей и проверку подлинности пользователей.
 */
class AuthManager {
public:
    AuthManager();
    
     /**
     * @brief Загрузка базы пользователей из файла
     * @param filename Путь к файлу базы пользователей
     * @return true если загрузка успешна, false в случае ошибки
     */
    bool loadUserDatabase(const std::string& filename);

    /**
     * @brief Генерация случайной соли
     * @return 16-символьная hex строка соли
     */
    std::string generateSalt();

     /**
     * @brief Аутентификация пользователя
     * @param login Логин пользователя
     * @param salt Соль для хеширования
     * @param clientHash Хеш от клиента
     * @return true если аутентификация успешна
     */
    bool authenticate(const std::string& login, const std::string& salt, 
                     const std::string& clientHash);
    
    /**
     * @brief Вычисление хеша SHA256
     * @param salt Соль для хеширования
     * @param password Пароль пользователя
     * @return 64-символьный hex хеш в верхнем регистре
     */
    std::string computeHash(const std::string& salt, const std::string& password);
    
private:
    std::unordered_map<std::string, std::string> m_users; ///< Хранилище пользователей
    std::random_device m_rd;                              ///< Генератор случайных чисел
    std::mt19937 m_gen;                                   ///< Mersenne Twister генератор
    std::uniform_int_distribution<uint64_t> m_dis;        ///< Распределение для соли
};

#endif