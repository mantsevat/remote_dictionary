#include <regex>
#include <fstream>
#include <exception>
#include "spdlog/spdlog.h"

#include "saver.h"

void saver::save(std::map<std::string, std::string> map, const std::string filename)
{
    spdlog::get("logger")->debug("Began saving the dictionary. Filename: {}", filename);
    std::ofstream dict_file;
    dict_file.open(filename);
    if (!dict_file)
        throw std::runtime_error("Error while opening the file to dump dict!");

    for (const auto &[key, value] : map)
    {
        dict_file << key << ' ' << value << '\n';
    }
    spdlog::get("logger")->info("Succesfully saved the dictionary");
    return;
}

std::map<std::string, std::string> saver::load(const char *filename)
{
    spdlog::get("logger")->debug("Began restoring the dictionary. Filename: {}", filename);
    std::ifstream dict_file;
    dict_file.open(filename);
    if (!dict_file)
        throw std::runtime_error("Error while opening the file to restore dict!");

    std::regex map_rgx("^([a-zA-Z0-9]+) ([a-zA-Z0-9]+)$");
    std::smatch m;

    std::string currentline;
    std::map<std::string, std::string> new_map;

    while (std::getline(dict_file, currentline))
    {
        spdlog::get("logger")->debug("Reading the restore file. The current line is :{}", currentline);
        
        if (std::regex_search(currentline, m, map_rgx))
        {
            new_map.insert({m[1], m[2]});
        }
        else
        {
            throw std::runtime_error("This string doesn't match map format: " + currentline);
        }
    }

    spdlog::get("logger")->info("Succesfully restored the dictionary");

    return new_map;
}
