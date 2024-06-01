#include <map>
#include <string>
#include <regex>
#include <memory>

#include "saver.h"

#ifndef DICTIONARY_H
#define DICTIONARY_H
class dictionary
{
public:
    dictionary(std::shared_ptr<saver> saver);
    void set_dictionary(std::map<std::string, std::string> dictionary);
    std::map<std::string, std::string> get_dictionary() const;
    std::string process_command(std::string cmd);

private:
    std::map<std::string, std::string> _dict;
    std::map<std::string, std::string (dictionary::*)(std::smatch)> cmd_map;
    std::shared_ptr<saver> _saver;

    std::string put_pair(std::smatch m);
    std::string get_value(std::smatch m);
    std::string delete_key(std::smatch m);
    std::string count_keys(std::smatch m);
    std::string dump_dictionary(std::smatch m);
    
};
#endif
