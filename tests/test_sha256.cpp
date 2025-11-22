#include <UnitTest++/UnitTest++.h>
#include <string>
#include "SHA256.h"

SUITE(SHA256Tests) {
    TEST(HashNotEmptyString) {
        std::string input = "hello world";
        std::string hash = SHA256::hash(input);
        
        CHECK(!hash.empty());
        CHECK_EQUAL(64, hash.length()); // SHA256 выдает 64 шестнадцатеричных символа
    }
    
    TEST(HashEmptyString) {
        std::string input = "";
        std::string hash = SHA256::hash(input);
        
        CHECK(!hash.empty());
        CHECK_EQUAL(64, hash.length());
    }
    
    TEST(HashConsistency) {
        std::string input = "consistent input";
        std::string hash1 = SHA256::hash(input);
        std::string hash2 = SHA256::hash(input);
        std::string hash3 = SHA256::hash(input);
        
        CHECK_EQUAL(hash1, hash2);
        CHECK_EQUAL(hash2, hash3);
        CHECK_EQUAL(hash1, hash3);
    }
    
    TEST(DifferentInputsDifferentHashes) {
        std::string input1 = "first input";
        std::string input2 = "second input";
        
        std::string hash1 = SHA256::hash(input1);
        std::string hash2 = SHA256::hash(input2);
        
        CHECK(hash1 != hash2);
    }
    
    TEST(KnownHashValue) {
        std::string input = "hello";
        std::string expected = "2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824";
        std::string actual = SHA256::hash(input);
        
        CHECK_EQUAL(expected, actual);
    }
    
    TEST(LongInput) {
        std::string input = "This is a much longer input string that should still work correctly with the SHA256 algorithm without any issues whatsoever.";
        std::string hash = SHA256::hash(input);
        
        CHECK(!hash.empty());
        CHECK_EQUAL(64, hash.length());
    }
}