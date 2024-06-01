#include <string>

#include "settings.h"

class cli_parser
{
private:
    app_settings s;

public:
    cli_parser();
    bool parse_cli(int argc, char* argv[]);
    app_settings get_settings() const;
};
