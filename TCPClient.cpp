#include <boost/asio.hpp>
#include <iostream>
#include <print>

using namespace::boost::asio;

int main(int argc, char* argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "Usage: client <host>" << std::endl;
      return -1;
    }

    io_context io_context;

    ip::tcp::resolver resolver(io_context);
    ip::tcp::resolver::results_type endpoints = resolver.resolve(argv[1], "25000");

    boost::system::error_code ec;
    ip::tcp::socket socket(io_context);
    connect(socket, endpoints, ec);

    if (ec) { // This means that the above call to 'connect' failed
      throw boost::system::system_error(ec);
    }

    size_t nbytes = socket.write_some(buffer("hello there\n"), ec);
    std::println("Wrote {} bytes", nbytes);

    if (ec == boost::asio::error::eof) {
      // break; // Connection closed cleanly by peer
    } else if (ec) {
      throw boost::system::system_error(ec);
    }

  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
