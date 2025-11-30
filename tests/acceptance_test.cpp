/**
 * @file acceptance_test.cpp
 * @brief Приемочные тесты для системы
 * @author Савин А.В.
 * @version 1.0
 */

#include <UnitTest++/UnitTest++.h>
#include <fstream>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include "AuthManager.h"
#include "VectorProcessor.h"
#include "SHA256.h"
#include "Logger.h"

SUITE(AcceptanceTests) {
    
    // Вспомогательные функции
    static void createTestFile(const std::string& filename, const std::string& content) {
        std::ofstream file(filename);
        file << content;
        file.close();
    }
    
    static void cleanupTestFile(const std::string& filename) {
        remove(filename.c_str());
    }
    
    TEST(FullSystemWorkflow_Acceptance) {
        std::cout << "=== ПРИЕМОЧНЫЙ ТЕСТ: Полный рабочий процесс ===" << std::endl;
        
        // 1. Подготовка тестовых данных
        createTestFile("acceptance_users.conf", 
            "alice:AlicePassword123\n"
            "bob:BobSecret456\n"
            "charlie:Charlie789");
        
        // 2. Инициализация компонентов
        AuthManager auth;
        CHECK(auth.loadUserDatabase("acceptance_users.conf"));
        
        // 3. Тестирование аутентификации
        std::string salt = auth.generateSalt();
        std::string clientHash = auth.computeHash(salt, "AlicePassword123");
        bool authResult = auth.authenticate("alice", salt, clientHash);
        CHECK_EQUAL(true, authResult);
        std::cout << "Аутентификация пользователя alice: УСПЕХ" << std::endl;
        
        // 4. Тестирование вычислений
        std::vector<uint32_t> testVectors[] = {
            {1, 2, 3, 4, 5},     // 120
            {10, 20, 30},        // 6000
            {2, 2, 2, 2, 2},     // 32
            {100, 200, 300}      // 6000000
        };
        
        uint32_t expected[] = {120, 6000, 32, 6000000};
        
        for (size_t i = 0; i < 4; i++) {
            uint32_t result = VectorProcessor::computeProduct(testVectors[i]);
            CHECK_EQUAL(expected[i], result);
            std::cout << "Вычисление произведения вектора " << (i+1) << ": " 
                      << result << " (ожидалось: " << expected[i] << ") - УСПЕХ" << std::endl;
        }
        
        // 5. Тестирование обработки ошибок
        bool wrongAuth = auth.authenticate("alice", salt, "WRONGHASH");
        CHECK_EQUAL(false, wrongAuth);
        std::cout << "Обработка неверной аутентификации: УСПЕХ" << std::endl;
        
        bool unknownUser = auth.authenticate("unknown", salt, clientHash);
        CHECK_EQUAL(false, unknownUser);
        std::cout << "Обработка неизвестного пользователя: УСПЕХ" << std::endl;
        
        // Очистка
        cleanupTestFile("acceptance_users.conf");
        std::cout << "=== ПРИЕМОЧНЫЙ ТЕСТ ЗАВЕРШЕН УСПЕШНО ===" << std::endl;
    }
    
    TEST(Performance_Acceptance) {
        std::cout << "=== ПРИЕМОЧНЫЙ ТЕСТ: Производительность ===" << std::endl;
        
        // Тестирование производительности с большими данными
        std::vector<uint32_t> largeVector(500, 2);
        
        auto start = std::chrono::high_resolution_clock::now();
        uint32_t result = VectorProcessor::computeProduct(largeVector);
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        CHECK(result > 0);
        CHECK(duration.count() < 100); // Должно выполняться менее 100ms
        
        std::cout << "Производительность: обработка 500 элементов за " 
                  << duration.count() << "ms - УСПЕХ" << std::endl;
        std::cout << "=== ТЕСТ ПРОИЗВОДИТЕЛЬНОСТИ ЗАВЕРШЕН ===" << std::endl;
    }
    
    TEST(Security_Acceptance) {
        std::cout << "=== ПРИЕМОЧНЫЙ ТЕСТ: Безопасность ===" << std::endl;
        
        // Тестирование криптографических функций
        std::string password = "SecurePassword123";
        std::string salt1 = "1234567890ABCDEF";
        std::string salt2 = "FEDCBA0987654321";
        
        AuthManager auth;
        std::string hash1 = auth.computeHash(salt1, password);
        std::string hash2 = auth.computeHash(salt2, password);
        
        // Хеши с разной солью должны быть разными
        CHECK(hash1 != hash2);
        std::cout << "Разные соли дают разные хеши: УСПЕХ" << std::endl;
        
        // Хеши должны быть в верхнем регистре
        for (char c : hash1) {
            CHECK(std::isupper(c) || std::isdigit(c));
        }
        std::cout << "Хеши в верхнем регистре: УСПЕХ" << std::endl;
        
        // Длина хеша должна быть 64 символа
        CHECK_EQUAL(64, hash1.length());
        std::cout << "Длина хеша 64 символа: УСПЕХ" << std::endl;
        
        std::cout << "=== ТЕСТ БЕЗОПАСНОСТИ ЗАВЕРШЕН ===" << std::endl;
    }
};