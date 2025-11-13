#include "SHA256.h"
#include <openssl/evp.h>
#include <sstream>
#include <iomanip>
#include <stdexcept>

std::string SHA256::hash(const std::string& input) {
    EVP_MD_CTX* context = EVP_MD_CTX_new();
    if (context == nullptr) {
        throw std::runtime_error("Не удалось создать контекст хеширования");
    }
    
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLength = 0;
    
    // Инициализация контекста для SHA256
    if (EVP_DigestInit_ex(context, EVP_sha256(), nullptr) != 1) {
        EVP_MD_CTX_free(context);
        throw std::runtime_error("Ошибка инициализации алгоритма SHA256");
    }
    
    // Добавление данных
    if (EVP_DigestUpdate(context, input.c_str(), input.size()) != 1) {
        EVP_MD_CTX_free(context);
        throw std::runtime_error("Ошибка добавления данных для хеширования");
    }
    
    // Получение хеша
    if (EVP_DigestFinal_ex(context, hash, &hashLength) != 1) {
        EVP_MD_CTX_free(context);
        throw std::runtime_error("Ошибка получения финального хеша");
    }
    
    EVP_MD_CTX_free(context);
    
    // Преобразование в hex-строку
    std::stringstream ss;
    for (unsigned int i = 0; i < hashLength; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    
    return ss.str();
}