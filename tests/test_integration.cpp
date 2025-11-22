#include <UnitTest++/UnitTest++.h>
#include <fstream>
#include "AuthManager.h"
#include "SHA256.h"
#include "VectorProcessor.h"

SUITE(IntegrationTests) {
    
    // Вспомогательные функции внутри suite
    static void createTestFile(const std::string& filename, const std::string& content) {
        std::ofstream file(filename);
        file << content;
        file.close();
    }
    
    static void cleanupTestFile(const std::string& filename) {
        remove(filename.c_str());
    }
    
    TEST(HashConsistencyBetweenAuthAndSHA256) {
        std::string salt = "1234567890ABCDEF";
        std::string password = "testpassword123";
        
        // Хеш вычисленный напрямую через SHA256
        std::string directHash = SHA256::hash(salt + password);
        
        // Хеш вычисленный через AuthManager
        AuthManager auth;
        std::string authHash = auth.computeHash(salt, password);
        
        // Приводим к верхнему регистру для сравнения
        for (char& c : directHash) {
            c = std::toupper(c);
        }
        
        CHECK_EQUAL(directHash, authHash);
    }
    
    TEST(FullAuthenticationFlow) {
        createTestFile("integration_users.conf", "alice:secret123\nbob:password456");
        
        AuthManager auth;
        bool loadResult = auth.loadUserDatabase("integration_users.conf");
        CHECK_EQUAL(true, loadResult);
        
        std::string salt = auth.generateSalt();
        std::string clientHash = auth.computeHash(salt, "secret123");
        
        bool authResult = auth.authenticate("alice", salt, clientHash);
        CHECK_EQUAL(true, authResult);
        
        bool wrongAuth = auth.authenticate("alice", salt, "wronghash");
        CHECK_EQUAL(false, wrongAuth);
        
        cleanupTestFile("integration_users.conf");
    }
    
    TEST(VectorProcessingAfterAuthSimulation) {
        // Симуляция обработки векторов после успешной аутентификации
        std::vector<uint32_t> testVectors[] = {
            {1, 2, 3, 4, 5},           // 120
            {10, 20, 30},              // 6000
            {100, 200},                // 20000
            {1000, 1000, 1000},        // 1000000000
            {65536, 65536}             // MAX_UINT32
        };
        
        uint32_t expected[] = {120, 6000, 20000, 1000000000, std::numeric_limits<uint32_t>::max()};
        
        for (size_t i = 0; i < 5; i++) {
            uint32_t result = VectorProcessor::computeProduct(testVectors[i]);
            CHECK_EQUAL(expected[i], result);
        }
    }
};