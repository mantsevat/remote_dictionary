#include "boost/bind.hpp"
#include <iostream>
#include "spdlog/spdlog.h"

#include "connection.h"

connection::connection(boost::asio::io_context &io_context, std::shared_ptr<proxy_dict> p, std::weak_ptr<int> c) : _socket(io_context), proxy(p), clients(c)
{
}

std::shared_ptr<connection> connection::create(boost::asio::io_context &io_context, std::shared_ptr<proxy_dict> p, std::weak_ptr<int> c)
{
    spdlog::get("logger")->debug("Creating a connection instance");
    return std::shared_ptr<connection>(new connection(io_context, p, c));
}

boost::asio::ip::tcp::socket &connection::socket()
{
    return _socket;
}

void connection::read()
{
    spdlog::get("logger")->debug("Set async read operation");
    boost::asio::async_read_until(
        _socket, buf, '\n',
        boost::bind(&connection::handle_read, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void connection::handle_read(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    spdlog::get("logger")->debug("Began the async read handler");
    std::string rs((std::istreambuf_iterator<char>(&buf)),std::istreambuf_iterator<char>());
    
    if (error)
    {
        if (error == boost::asio::error::eof || error == boost::asio::error::connection_reset){
            auto lock = clients.lock();
            if(lock)
                (*lock)-=1;
            spdlog::get("logger")->info("Client disconnected. Clients now: {}", *(lock));    
        }

        spdlog::get("logger")->error("Err while reading a client message: {}", error.message());
        return;
    }

    spdlog::get("logger")->info("Successfully received {} bytes from client", bytes_transferred);
  
    reply(rs.substr(0, bytes_transferred-1));
}

void connection::reply(std::string bs)
{
    spdlog::get("logger")->debug("Set async write operation");
    std::string answer = proxy->process_command(bs);

    boost::asio::async_write(_socket, boost::asio::buffer(answer),
                             boost::bind(&connection::handle_write, shared_from_this(),
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred));
}

void connection::handle_write(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    spdlog::get("logger")->debug("Began the async write handler");
    if (error)
    {
        spdlog::get("logger")->error("Err while writing to the client: {}", error.message());
        return;
    }
    spdlog::get("logger")->info("Successfully sent {} bytes to the client", bytes_transferred);
    read();
}