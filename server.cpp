#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/bind.hpp>
#include <memory>
#include <iostream>
#include <iomanip>

using namespace std;
namespace asio = boost::asio;
namespace sys = boost::system;
using udp = asio::ip::udp;

static const size_t BufferSize = 256;

using shared_udp_socket = shared_ptr<udp::socket>;

void udp_send_to(asio::yield_context yield, shared_udp_socket socket, udp::endpoint peer) {
  const char *msg = "Hello from server";
  socket->async_send_to(asio::buffer(msg, strlen(msg)), peer, yield);
}

void udp_server(asio::yield_context yield, asio::io_service &service, unsigned short port) {
  shared_udp_socket socket = make_shared<udp::socket>(service, udp::endpoint{udp::v4(), port});
  char buffer[BufferSize];
  udp::endpoint remote_peer;
  sys::error_code ec;
  while (true) {
    socket->async_receive_from(asio::buffer(buffer, BufferSize), remote_peer, yield[ec]);
    if (!ec) {
      spawn(service, boost::bind(udp_send_to, ::_1, socket, remote_peer));
    }
  }
}

int main() {
  asio::io_service service;
  spawn(service, boost::bind(udp_server, ::_1, boost::ref(service), 55000));
  service.run();

  return 0;
}
