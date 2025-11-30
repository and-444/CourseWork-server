/**
 * @file Interface.h
 * @brief Интерфейс командной строки с использованием Boost.Program_options
 * @author Савин А.В.
 * @version 1.0
 */

#pragma once
#include <boost/program_options.hpp>
#include <string>

namespace po = boost::program_options;

struct Params {
    std::string dbFile; // путь к файлу базы данных
    std::string logFile; // путь к файлу журнала
    unsigned short port; // порт
};


/**
 * @class Interface
 * @brief Парсер аргументов командной строки
 * 
 * Класс реализует профессиональный парсинг аргументов командной строки
 * с использованием библиотеки Boost.Program_options.
 */
class Interface {
private:
    po::options_description desc; ///< Описание опций командной строки
    po::variables_map vm;         ///< Результаты парсинга аргументов
    Params params;                ///< Параметры приложения
    
public:
    Interface(); // конструктор

    /**
     * @brief Парсинг аргументов командной строки
     * @param argc Количество аргументов
     * @param argv Массив аргументов
     * @return true если парсинг успешен, false если показана справка
     * @throws boost::program_options::error при ошибках парсинга
     */
    bool parse(int argc, char** argv);

    /**
     * @brief Получение параметров приложения
     * @return Структура с параметрами
     */
    Params getParams() const {
        return params;
    }

    /**
     * @brief Вывод справки по использованию
     */
    void printHelp() const;
};