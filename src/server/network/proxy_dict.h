#include <string>
#include <memory>

#include "../dictionary/dictionary.h"

#ifndef PROXY_DICT_H
#define PROXY_DICT_H
class proxy_dict
{
public:
    std::string process_command(std::string cmd);
    proxy_dict(std::shared_ptr<dictionary> h);
private:
    std::shared_ptr<dictionary> hidden_dict;
};
#endif