#include "protocol.hpp"
#include <print>
#include <boost/archive/binary_iarchive.hpp>

using boost::asio::ip::tcp;

int main() {
  try {
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 25000));

    std::println("Server is running on port 25000...");

    // Note that we only create one acceptor but that acceptor is capable of accepting multiple connections (i.e. multiple sockets)
    for (;;) {
      tcp::socket socket(io_context);
      acceptor.accept(socket);

      boost::asio::streambuf recv_buffer;
      payload_t inbound_msg;
      boost::system::error_code error_code;

      try {
        size_t nbytes = boost::asio::read(socket, recv_buffer, error_code);
        if (error_code && error_code != boost::asio::error::eof) { // eof error indicates client closed connection, so we don't need to take action
          throw boost::system::system_error(error_code);
        }

        std::istream input_stream(&recv_buffer);
        boost::archive::binary_iarchive input_archive(input_stream);
        input_archive >> inbound_msg; // This will auto unserialize the data by calling the serialize func
      } catch (const boost::archive::archive_exception& e) {
        std::println("Archive Exception: {}", e.what());
      }

      std::println("Name: {}", inbound_msg.user);
      std::println("Msg: {}", inbound_msg.msg_content);
    }
  } catch (std::runtime_error& e) {
    std::println("Exception: ", e.what());
  }
  return 0;
}
