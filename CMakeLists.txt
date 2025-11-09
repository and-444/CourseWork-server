cmake_minimum_required(VERSION 3.10)
project(NetworkServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск необходимых библиотек
find_package(PkgConfig REQUIRED)
pkg_check_modules(OPENSSL REQUIRED openssl)

# Настройка компилятора
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# Исходные файлы
set(SOURCES
    src/main.cpp
    src/Server.cpp
    src/AuthManager.cpp
    src/VectorProcessor.cpp
    src/Logger.cpp
)

# Заголовочные файлы
set(HEADERS
    src/Server.h
    src/AuthManager.h
    src/VectorProcessor.h
    src/Logger.h
)

# Создание исполняемого файла
add_executable(server ${SOURCES} ${HEADERS})

# Подключение библиотек
target_link_libraries(server ${OPENSSL_LIBRARIES} pthread)

# Настройка include директорий
target_include_directories(server PRIVATE ${OPENSSL_INCLUDE_DIRS})

# Отключение предупреждений о deprecated функциях для OpenSSL
target_compile_definitions(server PRIVATE -DOPENSSL_SUPPRESS_DEPRECATED)