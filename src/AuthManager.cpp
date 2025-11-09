#include "AuthManager.h"
#include "Logger.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <openssl/evp.h>
#include <cstring>

AuthManager& AuthManager::getInstance() {
    static AuthManager instance;
    return instance;
}

bool AuthManager::loadUsers(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        Logger::getInstance().log("ERROR: Cannot open user database file: " + filename, true);
        return false;
    }
    
    users_.clear();
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string login = line.substr(0, pos);
            std::string password = line.substr(pos + 1);
            users_[login] = password;
        }
    }
    
    file.close();
    Logger::getInstance().log("Loaded " + std::to_string(users_.size()) + " users from " + filename);
    return true;
}

bool AuthManager::userExists(const std::string& login) const {
    return users_.find(login) != users_.end();
}

std::string AuthManager::generateSalt() {
    std::uniform_int_distribution<uint64_t> dis;
    uint64_t saltValue = dis(gen_);
    
    std::stringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << saltValue;
    return ss.str();
}

std::string AuthManager::computeHash(const std::string& salt, const std::string& password) {
    std::string data = salt + password;
    
    EVP_MD_CTX* context = EVP_MD_CTX_new();
    if (!context) {
        throw std::runtime_error("Failed to create EVP context");
    }
    
    // Используем SHA-256 через EVP API
    if (EVP_DigestInit_ex(context, EVP_sha256(), nullptr) != 1) {
        EVP_MD_CTX_free(context);
        throw std::runtime_error("Failed to initialize SHA-256 digest");
    }
    
    if (EVP_DigestUpdate(context, data.c_str(), data.length()) != 1) {
        EVP_MD_CTX_free(context);
        throw std::runtime_error("Failed to update SHA-256 digest");
    }
    
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLength = 0;
    if (EVP_DigestFinal_ex(context, hash, &hashLength) != 1) {
        EVP_MD_CTX_free(context);
        throw std::runtime_error("Failed to finalize SHA-256 digest");
    }
    
    EVP_MD_CTX_free(context);
    
    // Проверяем, что получили именно SHA-256
    if (hashLength != 32) {
        throw std::runtime_error("Invalid SHA-256 hash length");
    }
    
    std::stringstream ss;
    for (unsigned int i = 0; i < hashLength; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    
    return ss.str();
}

bool AuthManager::verifyHash(const std::string& login, const std::string& salt, const std::string& receivedHash) {
    auto it = users_.find(login);
    if (it == users_.end()) {
        return false;
    }
    
    std::string computedHash = computeHash(salt, it->second);
    
    // Постоянное время сравнения для защиты от атак по времени
    if (computedHash.length() != receivedHash.length()) {
        return false;
    }
    
    int result = 0;
    for (size_t i = 0; i < computedHash.length(); i++) {
        result |= computedHash[i] ^ receivedHash[i];
    }
    
    return result == 0;
}