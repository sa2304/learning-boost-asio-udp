#include <boost/asio.hpp>
#include <iostream>
#include <exception>
#include <iomanip>

using namespace std;
namespace asio = boost::asio;

int main() {
  const unsigned short port = 55000;
  const string greet{"Hello, World!"};

  asio::io_service service;
  asio::ip::udp::endpoint endpoint{asio::ip::udp::v4(), port};
  asio::ip::udp::socket socket{service, endpoint};
  asio::ip::udp::endpoint ep;

  while (true) {
    try {
      char msg[256];
      size_t recvd = socket.receive_from(asio::buffer(msg, sizeof(msg)), ep);
      msg[recvd] = 0;
      cout << "Received "s << quoted(msg) << " from "s << ep.address() << ':' << ep.port() << endl;

      socket.send_to(asio::buffer(greet), ep);
    } catch (const exception &e) {
      cerr << e.what() << endl;
    }
  }

  return 0;
}