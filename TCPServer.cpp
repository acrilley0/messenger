#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/placeholders.hpp>
#include <iostream>
#include <print>

using namespace boost::asio;

/* Structure:
   - Note that the typical structure of a TCP implementation using boost involves having
   a server class and a session class
   - The TCPServer class is responsible for defining the context and the acceptor, which
   involves accepting a TCPSession
   - The TCPSession class defines a connection which is made up of a socket and a message and
   it will handle starting a connection and writing to that connection
*/

std::string make_daytime_string() {
  std::time_t now = time(0);
  return ctime(&now);
}

// enable_shared_from_this creates a shared_ptr to 'this' when all we have is 'this
// Without this we have no way of getting a shared_ptr to 'this'
class TCPSession : public std::enable_shared_from_this<TCPSession> {
  public:
    typedef std::shared_ptr<TCPSession> tcp_conn_p;

    static tcp_conn_p create_session(io_context& ctx) {
      return tcp_conn_p(new TCPSession(ctx));
    }

    ip::tcp::socket& get_socket() {
      return socket_;
    }

    void start_session() {
      async_read_until(socket_, incoming_msg_, "\n",
          std::bind(&TCPSession::handle_read, shared_from_this(),
            placeholders::error,
            placeholders::bytes_transferred));
    }

  private:
    TCPSession(io_context& ctx)
      : socket_(ctx) {}

    void handle_write(const boost::system::error_code& error, size_t bytes_transferred) {}
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
      if (!error) {
        std::string incoming_msg_str(
            boost::asio::buffers_begin(incoming_msg_.data()),
            boost::asio::buffers_end(incoming_msg_.data()));
        std::println("Received msg! {}", incoming_msg_str);
      }
    }

    ip::tcp::socket socket_;
    std::string message_;
    boost::asio::streambuf incoming_msg_;
};

class TCPServer {
  public:
    TCPServer(boost::asio::io_context& ctx) :
      io_context_(ctx),
      acceptor_(ctx, ip::tcp::endpoint(ip::tcp::v4(), 25000))
  {
    start_accept();
  }

  private:
    void start_accept() {
      TCPSession::tcp_conn_p new_session = TCPSession::create_session(io_context_);

      acceptor_.async_accept(new_session->get_socket(),
          std::bind(&TCPServer::handle_accept, this, new_session,
            boost::asio::placeholders::error));
    }

    void handle_accept(TCPSession::tcp_conn_p new_session, const boost::system::error_code& error) {
      if (!error) {
        new_session->start_session();
      }

      start_accept();
    }

    io_context& io_context_;
    ip::tcp::acceptor acceptor_;
};

int main() {
  try {
    io_context io_context;
    TCPServer server(io_context);
    io_context.run();
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
