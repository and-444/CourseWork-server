#include <UnitTest++/UnitTest++.h>
#include <fstream>
#include "AuthManager.h"
#include "Logger.h"

SUITE(AuthManagerTests) {
    // Вспомогательные функции
    void createTestFile(const std::string& filename, const std::string& content) {
        std::ofstream file(filename);
        file << content;
        file.close();
    }
    
    void cleanupTestFile(const std::string& filename) {
        remove(filename.c_str());
    }
    
    TEST(LoadValidUserDatabase) {
        createTestFile("test_users.conf", 
            "user1:pass1\n"
            "user2:pass2\n"
            "user3:pass3\n");
        
        AuthManager auth;
        bool result = auth.loadUserDatabase("test_users.conf");
        
        CHECK_EQUAL(true, result);
        cleanupTestFile("test_users.conf");
    }
    
    TEST(LoadNonExistentFile) {
        AuthManager auth;
        bool result = auth.loadUserDatabase("nonexistent_file.conf");
        
        CHECK_EQUAL(false, result);
    }
    
    TEST(LoadEmptyFile) {
        createTestFile("empty_users.conf", "");
        
        AuthManager auth;
        bool result = auth.loadUserDatabase("empty_users.conf");
        
        CHECK_EQUAL(false, result);
        cleanupTestFile("empty_users.conf");
    }
    
    TEST(LoadFileWithInvalidFormat) {
    createTestFile("invalid_format.conf",
        "user1:pass1\n"
        "invalid_line\n"
        "user2:pass2\n"
        ":empty_password\n"
        "user3:pass3\n");
    
    AuthManager auth;
    bool result = auth.loadUserDatabase("invalid_format.conf");
    
    CHECK_EQUAL(true, result); // Должны загрузиться только валидные строки
    
    cleanupTestFile("invalid_format.conf");
}
    
    TEST(SaltGenerationFormat) {
        AuthManager auth;
        std::string salt = auth.generateSalt();
        
        CHECK_EQUAL(16, salt.length());
        for (char c : salt) {
            CHECK(std::isxdigit(c));
            CHECK(std::isupper(c) || std::isdigit(c));
        }
    }
    
    TEST(SaltUniqueness) {
        AuthManager auth;
        std::string salt1 = auth.generateSalt();
        std::string salt2 = auth.generateSalt();
        std::string salt3 = auth.generateSalt();
        
        CHECK(salt1 != salt2);
        CHECK(salt2 != salt3);
        CHECK(salt1 != salt3);
    }
    
    TEST(SuccessfulAuthentication) {
        createTestFile("auth_test.conf", "testuser:testpassword");
        
        AuthManager auth;
        auth.loadUserDatabase("auth_test.conf");
        
        std::string salt = "1234567890ABCDEF";
        std::string clientHash = auth.computeHash(salt, "testpassword");
        
        bool result = auth.authenticate("testuser", salt, clientHash);
        CHECK_EQUAL(true, result);
        
        cleanupTestFile("auth_test.conf");
    }
    
    TEST(FailedAuthenticationWrongPassword) {
        createTestFile("auth_test.conf", "testuser:testpassword");
        
        AuthManager auth;
        auth.loadUserDatabase("auth_test.conf");
        
        std::string salt = "1234567890ABCDEF";
        bool result = auth.authenticate("testuser", salt, "WRONGHASH");
        
        CHECK_EQUAL(false, result);
        cleanupTestFile("auth_test.conf");
    }
    
    TEST(FailedAuthenticationNonExistentUser) {
        createTestFile("auth_test.conf", "testuser:testpassword");
        
        AuthManager auth;
        auth.loadUserDatabase("auth_test.conf");
        
        std::string salt = "1234567890ABCDEF";
        std::string clientHash = auth.computeHash(salt, "testpassword");
        bool result = auth.authenticate("nonexistent", salt, clientHash);
        
        CHECK_EQUAL(false, result);
        cleanupTestFile("auth_test.conf");
    }
    
    TEST(HashComputationUpperCase) {
        AuthManager auth;
        std::string hash = auth.computeHash("SALT", "password");
        
        for (char c : hash) {
            CHECK(std::isupper(c) || std::isdigit(c));
        }
    }
}