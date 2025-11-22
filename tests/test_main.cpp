#include <UnitTest++/UnitTest++.h>
#include <iostream>

int main() {
    std::cout << "=== ЗАПУСК МОДУЛЬНЫХ ТЕСТОВ ===" << std::endl;
    std::cout << "Тестирование сетевого сервера" << std::endl;
    std::cout << "=============================================" << std::endl;
    
    int result = UnitTest::RunAllTests();
    
    std::cout << "=============================================" << std::endl;
    if (result == 0) {
        std::cout << "ВСЕ ТЕСТЫ ПРОЙДЕНЫ УСПЕШНО" << std::endl;
    } else {
        std::cout << "НЕКОТОРЫЕ ТЕСТЫ ПРОВАЛЕНЫ: " << result << " ошибок" << std::endl;
    }
    
    return result;
}