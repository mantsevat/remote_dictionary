#include "proxy_dict.h"
#include "spdlog/spdlog.h"

std::string proxy_dict::process_command(std::string cmd)
{
    spdlog::get("logger")->debug("Proxy passing this command: {}", cmd);
    auto res = hidden_dict->process_command(cmd);
    spdlog::get("logger")->debug("Proxy received this result: {}", res);
    return res + '\n';
}

proxy_dict::proxy_dict(std::shared_ptr<dictionary> h) : hidden_dict(h)
{
}