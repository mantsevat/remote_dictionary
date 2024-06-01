#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "app.h"
#include "./dictionary/dictionary.h"
#include "./cli_parser/cliparser.h"
#include "./network/server.h"
#include "./network/proxy_dict.h"

#define SPDLOG_LEVEL_NAMES                                           \
    {                                                                \
        "trace", "debug", "info", "warning", "error", "fatal", "off" \
    }

void app::start(int argc, char *argv[])
{
    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("server.log");
    std::vector<spdlog::sink_ptr> sinks{stdout_sink, file_sink};
    auto logger = std::make_shared<spdlog::logger>("logger", sinks.begin(), sinks.end());
    spdlog::register_logger(logger);
    spdlog::set_pattern("[%H:%M:%S.%e] %^[%l]%$ %v");

#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

    cli_parser parser;
    bool settings_ok = false;

    try
    {
        settings_ok = parser.parse_cli(argc, argv);
    }
    catch (std::exception &e)
    {
        spdlog::get("logger")->critical("CLI err: {}", e.what());
        return;
    }

    if (!settings_ok)
        return;

    app_settings settings = parser.get_settings();
    std::shared_ptr<saver> saver_ptr(new saver());
    std::shared_ptr<dictionary> dict_ptr(new dictionary(saver_ptr));

    if (!settings.restore_file.empty())
    {
        try
        {
            auto restored_map = saver_ptr->load(settings.restore_file.c_str());
            dict_ptr->set_dictionary(restored_map);
        }
        catch (std::exception &e)
        {
            spdlog::get("logger")->critical("Restore err: {}", e.what());
            return;
        }
    } else {
        spdlog::get("logger")->info("no restore file");
    }
    
    boost::asio::io_context context;
    std::shared_ptr<proxy_dict> proxy(new proxy_dict(dict_ptr));
    server s(settings.port, settings.lim, context, proxy);
    
    s.start();
    
}