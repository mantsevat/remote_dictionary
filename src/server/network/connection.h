#include <boost/asio.hpp>
#include <functional>
#include <memory>
#include <vector>

#include "proxy_dict.h"

class connection : public std::enable_shared_from_this<connection>
{

private:
    const unsigned max_buf_size = 4096;
    boost::asio::ip::tcp::socket _socket;
    boost::asio::streambuf buf {max_buf_size};
    std::shared_ptr<proxy_dict> proxy;
    std::weak_ptr<int> clients;

    connection(boost::asio::io_context &io_context, std::shared_ptr<proxy_dict> p, std::weak_ptr<int> c);
    void reply(std::string bs);
    void handle_read(const boost::system::error_code &, std::size_t bytes_transferred);
    void handle_write(const boost::system::error_code &, std::size_t bytes_transferred);
    

public:
    static std::shared_ptr<connection> create(boost::asio::io_context &io_context, std::shared_ptr<proxy_dict> p, std::weak_ptr<int> c);
    boost::asio::ip::tcp::socket &socket();
    void read();
};