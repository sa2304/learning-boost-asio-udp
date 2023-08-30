#include <boost/asio.hpp>
#include <iostream>
#include <iomanip>

using namespace std;
namespace asio = boost::asio;
namespace sys = boost::system;

class UDPAsyncServer {
 public:
  using socket = boost::asio::ip::udp::socket;
  using endpoint = boost::asio::ip::udp::endpoint;
  static const int BufferSize = 256;

  UDPAsyncServer(asio::io_service &service, unsigned short port)
      : socket_(service, endpoint{asio::ip::udp::v4(), port}) {
    WaitForReceive();
  }

  void WaitForReceive() {
    socket_.async_receive_from(asio::buffer(buffer_, BufferSize), remote_peer_,
                               [this](const sys::error_code &ec, size_t sz) {
                                 buffer_[sz] = 0;
                                 cout << "Received "s << quoted(buffer_) << " from "s << remote_peer_ << endl;

                                 WaitForReceive();

                                 const char *msg = "Hello from server";
                                 socket_.async_send_to(asio::buffer(msg, strlen(msg)), remote_peer_,
                                                       [this](const sys::error_code &ec, size_t sz) {});
                               });
  }

 private:
  socket socket_;
  endpoint remote_peer_;
  char buffer_[BufferSize];
};

int main() {
  asio::io_service service;
  UDPAsyncServer server{service, 55000};
  service.run();

  return 0;
}