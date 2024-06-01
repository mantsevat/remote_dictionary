#include <boost/asio.hpp>
#include <memory>

#include "connection.h"

class server
{

public:
    server(unsigned short port, unsigned lim, boost::asio::io_context &context, std::shared_ptr<proxy_dict> p);
    void start();

private:
    std::shared_ptr<int> clients_now; 
    unsigned connectlim;
    boost::asio::ip::tcp::acceptor acceptor;
    boost::asio::io_context &io_context_;
    std::shared_ptr<proxy_dict> proxy;

    void start_accept();
    void handle_accept(std::shared_ptr<connection> new_connection,
                       const boost::system::error_code &error);
    void handle_refusal(const boost::system::error_code  &error, std::size_t bytes_transferred, std::shared_ptr<connection> & unfortunate);                   
};