#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <thread>
#include <chrono>
#include <unistd.h>
#include "include/zmq.hpp"
#include "include/zmq_addon.hpp"


void *vec_writer(std::string &message, std::vector<std::string> &vec);
void *vec_reader(
    std::vector<std::string> &vec,
    zmq::context_t &ctx,
    size_t socket_fd);
std::string random_string(std::size_t length);

int main(void)
{
    // Random's strings vector - simulating a generic source of data
    std::vector<std::string> vec;

    // ZMQ context shared among threads
    zmq::context_t ctx;

    // Populate the Random's strings vector (single thread)
    size_t iterations = 0;
    while (iterations <= 1000) {
        std::string rnd_string = random_string(10);
        std::thread t1(&vec_writer, std::ref(rnd_string), std::ref(vec));
        // Firing the thread
        t1.join();
        iterations++;
    }

    std::cout << "Random's strings vector Ready ...\n";

    // Read from the vector - multiple threads
    // Firing a ZMQ PUSH per thread with a dedicated socket
    // the socket fd is derived from the thread number sequentially
    std::vector<std::thread> th_fire;
    size_t th = 3;
    std::cout << "Firing " << th << " threads, Reading & PUSH-ing\n";
    for (size_t t = 0; t < th; ++t) {
        th_fire.push_back(std::thread (
            &vec_reader,
            std::ref(vec),
            std::ref(ctx),
            t));
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
    size_t socket_fd)
{
    zmq::socket_t sock(ctx, zmq::socket_type::push);
    size_t vec_size = vec.size();

    // --- Convert the thread ID into string --- //
    auto t_id = std::this_thread::get_id();
    std::stringstream ss;
    ss << t_id;
    std::string thread_id = ss.str();
    // --- Convert the thread ID into string --- //

    std::string sok = "ipc://sockets/" + std::to_string(socket_fd);
    std::cout << "PUSH-ing to " << sok << "\n";
    sock.bind(sok);
    while(true) {
        // Randomly reading from the the Random's string vector
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

