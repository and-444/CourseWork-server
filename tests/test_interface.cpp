#include <UnitTest++/UnitTest++.h>
#include "Interface.h"
#include <vector>
#include <string>

SUITE(InterfaceTests) {
    
    TEST(InterfaceCreation) {
        Interface interface;
        CHECK(true);
    }
    
    TEST(InterfaceDefaultParameters) {
        Interface interface;
        std::vector<const char*> argv = {"program"};
        bool result = interface.parse(argv.size(), const_cast<char**>(argv.data()));
        CHECK_EQUAL(true, result);
        
        Params params = interface.getParams();
        CHECK_EQUAL("/etc/vcalc.conf", params.dbFile);
        CHECK_EQUAL("/var/log/vcalc.log", params.logFile);
        CHECK_EQUAL(33333, params.port);
    }
    
    TEST(InterfaceHelpParameter) {
        Interface interface;
        std::vector<const char*> argv = {"program", "--help"};
        bool result = interface.parse(argv.size(), const_cast<char**>(argv.data()));
        CHECK_EQUAL(false, result);
    }
    
    TEST(InterfaceValidCustomParameters) {
        Interface interface;
        std::vector<const char*> argv = {
            "program",
            "--config", "test.conf",
            "--log", "test.log",
            "--port", "30000"
        };
        bool result = interface.parse(argv.size(), const_cast<char**>(argv.data()));
        CHECK_EQUAL(true, result);
        
        Params params = interface.getParams();
        CHECK_EQUAL("test.conf", params.dbFile);
        CHECK_EQUAL("test.log", params.logFile);
        CHECK_EQUAL(30000, params.port);
    }

    TEST(InterfaceShortParameters) {
        Interface interface;
        
        std::vector<const char*> argv = {
            "program",
            "-c", "short.conf",
            "-l", "short.log",
            "-p", "55555"
        };
        
        bool result = interface.parse(argv.size(), const_cast<char**>(argv.data()));
        CHECK_EQUAL(true, result);
        
        Params params = interface.getParams();
        CHECK_EQUAL("short.conf", params.dbFile);
        CHECK_EQUAL("short.log", params.logFile);
        CHECK_EQUAL(55555, params.port);
    }

    TEST(InterfaceMixedParameters) {
        Interface interface;
        
        std::vector<const char*> argv = {
            "program",
            "--config", "mixed.conf",
            "-l", "mixed.log",
            "--port", "33333"
        };
        
        bool result = interface.parse(argv.size(), const_cast<char**>(argv.data()));
        CHECK_EQUAL(true, result);
        
        Params params = interface.getParams();
        CHECK_EQUAL("mixed.conf", params.dbFile);
        CHECK_EQUAL("mixed.log", params.logFile);
        CHECK_EQUAL(33333, params.port);
    }
    
};