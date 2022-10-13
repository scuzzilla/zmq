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
    zmq::socket_t sock(ctx, zmq::socket_type::pull);
    sock.connect("ipc://sockets/0");

    // Actual receiving
    while(true) {
        auto res = sock.recv(message, zmq::recv_flags::none);
        assert(res.value() != 0);
	    std::cout << "zmq_pull: " << message.to_string() << "\n";
    }

    return EXIT_SUCCESS;
}

