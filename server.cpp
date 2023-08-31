#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/bind.hpp>
#include <boost/shared_array.hpp>
#include <boost/thread.hpp>
#include <memory>
#include <iostream>

using namespace std;
namespace asio = boost::asio;
namespace sys = boost::system;
using udp = asio::ip::udp;

static const size_t BufferSize = 256;

using shared_udp_socket = shared_ptr<udp::socket>;

mutex mtx;
void log(string message) {
  lock_guard guard{mtx};
  cout << "["s << boost::this_thread::get_id() << "] "s << message;
}

void udp_send_to(asio::yield_context yield, shared_udp_socket socket, udp::endpoint peer, boost::shared_array<char> buf,
                 size_t size) {
  log("udp_send_to\n");
  const char *msg = "Hello from server";
  socket->async_send_to(asio::buffer(msg, strlen(msg)), peer, yield);
  socket->async_send_to(asio::buffer(buf.get(), size), peer, yield);
}

void udp_server(asio::yield_context yield, asio::io_service &service, unsigned short port) {
  shared_udp_socket socket = make_shared<udp::socket>(service, udp::endpoint{udp::v4(), port});
  udp::endpoint remote_peer;
  sys::error_code ec;
  while (true) {
    boost::shared_array<char> buffer{new char[BufferSize]};
    log("Waiting for incoming data...\n");
    size_t size = socket->async_receive_from(asio::buffer(buffer.get(), BufferSize), remote_peer, yield[ec]);
    if (!ec) {
      spawn(service, boost::bind(udp_send_to, ::_1, socket, remote_peer, buffer, size));
    }
  }
}

int main(int argc, char** argv) {
  log("Starting UDP server...\n");
  asio::io_service service;
  spawn(service, boost::bind(udp_server, ::_1, boost::ref(service), 55000));
  auto func = [&service] {
    service.run();
  };
  int num_threads = (1 < argc) ? atoi(argv[1]) : 1;
  if (num_threads <= 0) { num_threads = 1; }
  cout << "Running "s << num_threads << " threads...\n";
  boost::thread_group pool;
  for (int i = 0; i < num_threads; ++i) {
    pool.create_thread(func);
  }
  pool.join_all();

  return 0;
}
