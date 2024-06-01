#include <string>

#ifndef SETTINGS_H
#define SETTINGS_H

struct app_settings
{
    unsigned short port = 0;
    unsigned lim = 0;
    std::string restore_file = "";
};
#endif