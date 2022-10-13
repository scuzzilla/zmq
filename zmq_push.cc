#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <thread>
#include <chrono>
#include <unistd.h>
#include "cppzmq/zmq.hpp"
#include "cppzmq/zmq_addon.hpp"


void *vec_writer(std::string &message, std::vector<std::string> &vec);
void *vec_reader(
    std::vector<std::string> &vec,
    zmq::context_t &ctx,
    zmq::socket_t &sock);
std::string random_string(std::size_t length);

int main(void)
{
    std::vector<std::string> vec;

    zmq::context_t ctx;
    zmq::socket_t sock(ctx, zmq::socket_type::push);

    // Populate the vector - single thread
    size_t iterations = 0;
    while (iterations <= 1000) {
        std::string rnd_string = random_string(10);
        std::thread t1(&vec_writer, std::ref(rnd_string), std::ref(vec));
        // Firing the thread
        t1.join();
        iterations++;
    }

    std::cout << "Vector Ready ...\n";

    // Read from the vector - multiple threads
    std::vector<std::thread> th_fire;
    size_t th = 1;
    for (size_t t = 0; t < th; ++t) {
        th_fire.push_back(std::thread (
            &vec_reader,
            std::ref(vec),
            std::ref(ctx),
            std::ref(sock)));
    }

    for (std::thread &t : th_fire) {
        if (t.joinable()) {
            t.join();
        }
    }

    return EXIT_SUCCESS;
}

// Populate the vector with random strings - single thread
void *vec_writer(std::string &message, std::vector<std::string> &vec)
{
    vec.push_back(message);
    //std::this_thread::sleep_for(std::chrono::milliseconds(50));
    return (0);
}

// Read from the vector - multiple threads
void *vec_reader(
    std::vector<std::string> &vec,
    zmq::context_t &ctx,
    zmq::socket_t &sock)
{
    size_t vec_size = vec.size();

	// --- Convert the thread ID into string --- //
    auto t_id = std::this_thread::get_id();
	std::stringstream ss;
    ss << t_id;
    std::string thread_id = ss.str();
	// --- Convert the thread ID into string --- //

    sock.bind("ipc://sockets/0");
    while(true) {
        size_t index = (0 + (rand() % vec_size));
        //std::cout << thread_id << " " << vec.at(index) << "\n";
        sock.send(zmq::buffer(vec.at(index)), zmq::send_flags::dontwait);
        //std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return (0);
}

// Random string generation
std::string random_string(std::size_t length)
{
    const std::string CHARACTERS =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

    std::string random_string;

    for (std::size_t i = 0; i < length; ++i)
    {
        random_string += CHARACTERS[distribution(generator)];
    }

    return random_string;
}

