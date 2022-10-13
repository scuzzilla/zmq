#include <iostream>
#include <string>
#include <chrono>
#include "cppzmq/zmq.hpp"
#include "cppzmq/zmq_addon.hpp"


// CONNECT/RECEIVE - PULL
int main(void)
{
    // Message Buff preparation
    const size_t size = 1024;
    zmq::message_t message(size);

    // ZMQ Context & socket
    zmq::context_t ctx;
    zmq::socket_t sock_1(ctx, zmq::socket_type::pull);
    zmq::socket_t sock_2(ctx, zmq::socket_type::pull);
    sock_1.connect("ipc://sockets/1");
    sock_2.connect("ipc://sockets/2");
    //sock.connect("tcp://127.0.0.1:3001");

    // Actual receiving
    while(true) {
        auto res_1 = sock_1.recv(message, zmq::recv_flags::none);
        auto res_2 = sock_2.recv(message, zmq::recv_flags::none);
        assert(res_1.value() != 0);
	    std::cout << "zmq_pull_1: " << message.to_string() << "\n";
        assert(res_2.value() != 0);
	    std::cout << "zmq_pull_2: " << message.to_string() << "\n";
    }

    return EXIT_SUCCESS;
}

