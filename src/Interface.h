#pragma once
#include <boost/program_options.hpp>
#include <string>

namespace po = boost::program_options;

struct Params {
    std::string dbFile; // путь к файлу базы данных
    std::string logFile; // путь к файлу журнала
    unsigned short port; // порт
};

class Interface {
private:
    po::options_description desc; // описание опций
    po::variables_map vm; // для результатов парсинга
    Params params;
    
public:
    Interface(); // конструктор
    bool parse(int argc, char** argv);
    Params getParams() const {
        return params;
    }
    void printHelp() const;
};