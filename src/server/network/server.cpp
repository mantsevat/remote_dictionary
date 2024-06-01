#include "boost/bind.hpp"
#include <iostream>
#include "spdlog/spdlog.h"

#include "server.h"

void server::start()
{
    start_accept();
    io_context_.run();
    spdlog::get("logger")->info("Server started");
}
void server::start_accept()
{
    spdlog::get("logger")->debug("Set async accept operation");
    std::shared_ptr<connection> new_connection = connection::create(io_context_, proxy, clients_now);

    acceptor.async_accept(new_connection->socket(),
                          boost::bind(&server::handle_accept, this, new_connection,
                                      boost::asio::placeholders::error));
}
void server::handle_accept(std::shared_ptr<connection> new_connection,
                           const boost::system::error_code &error)
{
    spdlog::get("logger")->debug("Async accept handler began");
    if (!error)
    {
        if (*(clients_now) > connectlim)
        {
           
            boost::asio::async_write(new_connection->socket(), boost::asio::buffer("Connection refused: too many clients :("),
                                     boost::bind(&server::handle_refusal, this,
                                                 boost::asio::placeholders::error,
                                                 boost::asio::placeholders::bytes_transferred, new_connection));
        }
        *(clients_now)+=1;
        spdlog::get("logger")->debug("Succefully accepted a new client. Number of clients: {}", *(clients_now));
        new_connection->read();
    }
    else
    {
        spdlog::get("logger")->error("Accept err: {}", error.message());
    }

    start_accept();
}

server::server(unsigned short port, unsigned lim, boost::asio::io_context &context, std::shared_ptr<proxy_dict> p)
    : connectlim(lim), acceptor(context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)), io_context_(context), proxy(p)
{
    clients_now = std::shared_ptr<int> (new int);
    *clients_now = 0;
    spdlog::get("logger")->info("Server created with these options: port - {}, limit - {}", port, lim);
}

void server::handle_refusal(const boost::system::error_code &error, std::size_t bytes_transferred, std::shared_ptr<connection> &unfortunate)
{
    spdlog::get("logger")->debug("Refusal handler began");

    if (error)
        spdlog::get("logger")->error("Writing to refused err: {}", error.message());
    
    unfortunate->socket().close();

    spdlog::get("logger")->info("Refused connection");

}