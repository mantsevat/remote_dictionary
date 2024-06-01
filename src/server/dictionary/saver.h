#include <string>
#include <map>

#ifndef SAVER_H
#define SAVER_H

class saver
{

public:
    void save(std::map<std::string, std::string> m, const std::string filename);
    std::map<std::string, std::string> load(const char *filename);
};
#endif