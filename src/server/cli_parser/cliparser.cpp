#include <boost/program_options.hpp>
#include <iostream>
#include <regex>
#include "spdlog/spdlog.h"

#include "cliparser.h"

bool cli_parser::parse_cli(int argc, char *argv[])
{
    spdlog::get("logger")->debug("Started parsing command line options");

    namespace po = boost::program_options;

    auto check_port = [this](int port)
    {
        spdlog::get("logger")->debug("Port value from passed options: {}", port);
        if (1 > port || port > 65535)
        {
            throw po::validation_error(po::validation_error::invalid_option_value, "port", std::to_string(port));
        }
        this->s.port = port;
    };
    auto check_limit = [this](int limit)
    {
        spdlog::get("logger")->debug("Limit value from passed options: {}", limit);
        if (limit < 1 || limit > 100)
        {
            throw po::validation_error(po::validation_error::invalid_option_value, "limit", std::to_string(limit));
        }
        this->s.lim = limit;
    };
    auto check_filename = [this](std::string f)
    {
        spdlog::get("logger")->debug("File from passed options: {}", f);
        if (!std::regex_search(f, std::regex("([a-zA-Z0-9.]+)")))
        {
            throw po::validation_error(po::validation_error::invalid_option_value, "restore file name", f);
        }
        this->s.restore_file = f;
    };

    po::options_description desc("App options");
    desc.add_options()("help,h", "View manual(other options ignored)")
    ("port,p", po::value<int>()->notifier(check_port), "Set a tcp connection port. This value belongs to [1; 65535]")
    ("limit,l", po::value<int>()->notifier(check_limit), "Set a connection limit. It's value belongs to [1; 100]")
    ("file,f", po::value<std::string>()->notifier(check_filename), "Set a restore file for the dictionary")
    ("default,d", "Set default settings: port - 8888, limit - 20 without a restore file. Other options will be ignored if this option is chosen");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << '\n';
        spdlog::get("logger")->warn("Help option was chosen");
        return false;
    }

    if (vm.count("default"))
    {
        spdlog::get("logger")->info("Default settings were chosen");
        return true;
    }

    if (!(vm.count("port") ||  vm.count("limit") || vm.count("file")))
    {
        std::cout << "You haven't chosen anything! Rerun program with at least one option or with default settings(-d option)\n";
        spdlog::get("logger")->critical("No options were chosen");
        return false;
    }

    spdlog::get("logger")->info("Options were parsed");

    return true;
}

app_settings cli_parser::get_settings() const
{
    return s;
}

cli_parser::cli_parser()
{
    s.port = 8888;
    s.lim = 20;
}