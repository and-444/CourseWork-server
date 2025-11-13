#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <string>
#include <unordered_map>
#include <random>

class AuthManager {
public:
    AuthManager();
    
    // Основные методы
    bool loadUserDatabase(const std::string& filename);
    std::string generateSalt();
    bool authenticate(const std::string& login, const std::string& salt, 
                     const std::string& clientHash);
    
    // Тестовые методы
    void testHashComputation();
    
private:
    std::unordered_map<std::string, std::string> m_users;
    std::random_device m_rd;
    std::mt19937 m_gen;
    std::uniform_int_distribution<uint64_t> m_dis;
    
    std::string computeHash(const std::string& salt, const std::string& password);
};

#endif