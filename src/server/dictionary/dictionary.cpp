#include <stdexcept>
#include <iostream>
#include "spdlog/spdlog.h"

#include "dictionary.h"

const unsigned char r1key = 1;
const unsigned char r1val = 2;
const unsigned char r3key = 3;
const unsigned char r4file = 4;
const unsigned char r5key = 5;

const std::regex commandrgx("^PUT ([a-zA-Z0-9]+) ([a-zA-Z0-9]+)$|^DEL ([a-zA-Z0-9]+)$|^COUNT$|"
                            "DUMP ([a-zA-Z0-9.]+)$|^GET ([a-zA-Z0-9]+)$");

const std::regex cmdtypergx("PUT|GET|DEL|COUNT|DUMP");

dictionary::dictionary(std::shared_ptr<saver> saver) : _saver(saver)
{
    cmd_map["PUT"] = &dictionary::put_pair;
    cmd_map["GET"] = &dictionary::get_value;
    cmd_map["DEL"] = &dictionary::delete_key;
    cmd_map["COUNT"] = &dictionary::count_keys;
    cmd_map["DUMP"] = &dictionary::dump_dictionary;
}

std::string dictionary::process_command(std::string cmd)
{
    spdlog::get("logger")->debug("Received a command: {}", cmd);
    std::smatch match1;
    if (std::regex_search(cmd, match1, commandrgx))
    {
        std::smatch type;
        std::regex_search(cmd, type, cmdtypergx);
        std::string result = (this->*cmd_map.at(type[0]))(match1);
        spdlog::get("logger")->info("Succesfully processed command");
        spdlog::get("logger")->info("The result of processing: {}", result);
        return result;
    }
    spdlog::get("logger")->warn("User passed wrong command: {}", cmd);
    return "Wrong command";
}

std::string dictionary::put_pair(std::smatch m)
{
    std::string key = m[r1key];
    std::string value = m[r1val];

    spdlog::get("logger")->debug("Trying to put this pair to the dict: key - {} , value - {}", key, value);
    std::string result = "";
    if (_dict.count(key))
    {
        result = _dict.at(key);
        spdlog::get("logger")->warn("User rewrited existing value: {}", result);
    }
    _dict[key] = value;
    return "OK " + result;
}

std::string dictionary::get_value(std::smatch m)
{
    std::string key = m[r5key];

    spdlog::get("logger")->debug("Trying to get value of this key: {}", key);
    std::string result = "";
    if (_dict.count(key))
        return "OK " + _dict.at(key);

    spdlog::get("logger")->warn("User passed a non-existing key while trying to get its value : {}", key);
    return "NE";
}

std::string dictionary::delete_key(std::smatch m)
{
    std::string key = m[r3key];
    spdlog::get("logger")->debug("Trying to delete value of this key: {}", key);
    std::string result = "NE";
    if (_dict.count(key))
    {
        result = "OK " + _dict.at(key);
        _dict.erase(key);
    }
    spdlog::get("logger")->warn("User passed a non-existing key while trying to delete it: {}",key);
    return result;
}

std::string dictionary::count_keys(std::smatch m)
{
    spdlog::get("logger")->debug("Counting pairs in the dictionary");
    return "OK " + std::to_string(_dict.size());
}

std::string dictionary::dump_dictionary(std::smatch m)
{
    std::string f = m[r4file];
    try
    {
        _saver->save(_dict, f);
    }
    catch (std::exception &e)
    {
        spdlog::get("logger")->error("User passed name of the file that can't be opened. Name: {}", f);
        return "ERR DUMP";
    }
    return "OK DUMP";
}

void dictionary::set_dictionary(std::map<std::string, std::string> nd)
{
    _dict = nd;
}

std::map<std::string, std::string> dictionary::get_dictionary() const
{
    return _dict;
}
