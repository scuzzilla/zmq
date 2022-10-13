#include <cstdlib>
#include <iostream>
#include <iterator>
#include <string>
#include <chrono>
#include <unistd.h>
#include "cppzmq/zmq.hpp"
#include "cppzmq/zmq_addon.hpp"


// BIND/SEND - PUSH
int main(void)
{
    zmq::context_t ctx;
    zmq::socket_t sock(ctx, zmq::socket_type::push);
    sock.bind("ipc://sockets/0");
    //sock.bind("tcp://127.0.0.1:3001");

    while(true) {
        usleep(100);
        sock.send(zmq::str_buffer("message"), zmq::send_flags::dontwait);
    }

    return EXIT_SUCCESS;
}

