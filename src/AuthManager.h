#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <string>
#include <unordered_map>
#include <random>

class AuthManager {
public:
    static AuthManager& getInstance();
    
    bool loadUsers(const std::string& filename);
    bool userExists(const std::string& login) const;
    std::string generateSalt();
    bool verifyHash(const std::string& login, const std::string& salt, const std::string& receivedHash);
    
private:
    AuthManager() = default;
    std::unordered_map<std::string, std::string> users_;
    std::random_device rd_;
    std::mt19937_64 gen_;
    
    std::string computeHash(const std::string& salt, const std::string& password);
};

#endif // AUTHMANAGER_H