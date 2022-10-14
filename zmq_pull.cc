#include <iostream>
#include <thread>
#include <string>
#include <chrono>
#include "include/zmq.hpp"
#include "include/zmq_addon.hpp"


// Read from socks - multiple threads
void *zmq_pull(
    zmq::context_t &ctx)
{
    zmq::socket_t sock(ctx, zmq::socket_type::pull);

    // Message Buff preparation
    const size_t size = 1024;
    zmq::message_t message(size);

    std::string sok = "ipc:///tmp/my_socket.sock";
    //std::string sok = "tcp://*:4242/";
    std::cout << "PULL-ing from " << sok << "\n";
    sock.bind(sok);

    while(true) {
        auto res = sock.recv(message, zmq::recv_flags::none);
        if (res.value() != 0) {
            std::cout << "zmq_pull: " << message.to_string() << "\n";
        }
    }

    return (0);
}

int main(void)
{
    // ZMQ Context
    zmq::context_t ctx;

    // Actual receiving
    std::vector<std::thread> th_fire;
    size_t th = 3;
    std::cout << "Firing " << th << " threads, Reading & PULL-ing\n";
    zmq_pull(ctx);

    return EXIT_SUCCESS;
}


