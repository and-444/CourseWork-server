#include "AuthManager.h"
#include "SHA256.h"
#include "Logger.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cctype>

// Конструктор
AuthManager::AuthManager() : m_gen(m_rd()), m_dis(0, UINT64_MAX) {}

// Загрузка базы пользователей из файла
bool AuthManager::loadUserDatabase(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Ошибка: Не удалось открыть файл базы: " << filename << std::endl;
        return false;
    }
    
    std::string line;
    int userCount = 0;
    m_users.clear();
    
    std::cout << "Загрузка базы пользователей: " << filename << std::endl;
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        if (line[0] == '#') continue;
        
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string login = line.substr(0, pos);
            std::string password = line.substr(pos + 1);
            
            // Убираем пробелы
            login.erase(0, login.find_first_not_of(" \t"));
            login.erase(login.find_last_not_of(" \t") + 1);
            password.erase(0, password.find_first_not_of(" \t"));
            password.erase(password.find_last_not_of(" \t") + 1);
            
            m_users[login] = password;
            userCount++;
            
            std::cout << "Загружен пользователь: " << login << std::endl;
        }
    }
    
    file.close();
    
    std::cout << "Загружено пользователей: " << userCount << std::endl;
    
    if (userCount == 0) {
        std::cout << "Внимание: база пользователей пуста!" << std::endl;
        return false;
    }
    
    return true;
}

// Генерация соли
std::string AuthManager::generateSalt() {
    uint64_t salt = m_dis(m_gen);
    std::stringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << salt;
    std::string saltStr = ss.str();
    
    // Приводим к верхнему регистру
    for (char& c : saltStr) {
        c = std::toupper(c);
    }
    
    return saltStr;
}

// Аутентификация пользователя
bool AuthManager::authenticate(const std::string& login, const std::string& salt, 
                              const std::string& clientHash) {
    // Ищем пользователя в базе
    auto it = m_users.find(login);
    if (it == m_users.end()) {
        Logger::getInstance().log(LogLevel::ERROR, "Пользователь не найден", "логин: " + login);
        return false;
    }
    
    // Вычисляем хеш на сервере
    std::string serverHash = computeHash(salt, it->second);
    
    // Приводим к верхнему регистру для сравнения
    std::string clientHashUpper = clientHash;
    for (char& c : clientHashUpper) {
        c = std::toupper(c);
    }
    
    // Сравниваем хеши
    bool hashesMatch = (serverHash == clientHashUpper);
    
    if (!hashesMatch) {
        Logger::getInstance().log(LogLevel::ERROR, "Ошибка аутентификации", 
                                 "логин: " + login + ", хеши не совпадают");
    } else {
        Logger::getInstance().log(LogLevel::INFO, "Аутентификация успешна", "логин: " + login);
    }
    
    return hashesMatch;
}

// Вычисление хеша SHA256
std::string AuthManager::computeHash(const std::string& salt, const std::string& password) {
    std::string data = salt + password;
    std::string hash = SHA256::hash(data);
    
    // Приводим к верхнему регистру
    for (char& c : hash) {
        c = std::toupper(c);
    }
    
    return hash;
}