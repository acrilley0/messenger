#include "protocol.hpp"
#include <iostream>
#include <print>
#include <boost/archive/binary_oarchive.hpp>

using boost::asio::ip::tcp;

int main() {
  try {
    boost::asio::io_context io_context;

    tcp::resolver resolver(io_context);
    tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1", "25000");

    tcp::socket socket(io_context);
    boost::asio::connect(socket, endpoints);
    boost::system::error_code error_code;

    // Need to serialize payload_t so that we can send over some struct that holds whatever data we want
    payload_t outbound_msg = {};
    std::printf("Please enter a message: ");
    std::getline(std::cin, outbound_msg.msg_content);
    outbound_msg.user = "Aiden";

    std::ostringstream output_stream(std::ios::binary);
    try {
      boost::archive::binary_oarchive output_archive(output_stream);
      output_archive << outbound_msg; // This is what calls serialize
    } catch (boost::archive::archive_exception& e) {
        std::println("Archive Exception: {}", e.what());
    }

    std::string payload = output_stream.str();
    size_t nbytes = boost::asio::write(socket, boost::asio::buffer(payload), error_code);
    std::println("Sent {} bytes to the server", nbytes);

    if (error_code) {
      std::println("Send failed!");
    }
  } catch (std::exception& e) {
    std::println("Exception: ", e.what());
  }

  return 0;
}
