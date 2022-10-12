#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <thread>
#include <chrono>
#include "cppzmq/zmq.hpp"
#include "cppzmq/zmq_addon.hpp"


void *vec_writer(std::string &message, std::vector<std::string> &vec);
void *vec_reader(std::vector<std::string> &vec);
std::string random_string(std::size_t length);
void zmq_push(const std::string &message,
    const std::string &thread_id,
    zmq::context_t &ctx);
void zmq_pull(zmq::context_t &zmq);

int main(void)
{
    std::vector<std::string> vec;

    while(true) {
        size_t iterations = 0;
        if (iterations <= 10000) {
           std::string rnd_string = random_string(10);
           std::thread t1(&vec_writer, std::ref(rnd_string), std::ref(vec));
           t1.join();
        }
        iterations++;
        std::thread t2(&vec_reader, std::ref(vec));
        std::thread t3(&vec_reader, std::ref(vec));
        t2.join();
        t3.join();
    }

    return EXIT_SUCCESS;
}

void *vec_writer(std::string &message, std::vector<std::string> &vec)
{
    vec.push_back(message);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    return (0);
}

void *vec_reader(std::vector<std::string> &vec)
{
    zmq::context_t ctx;
    size_t vec_size = vec.size();
    size_t index = (0 + (rand() % vec_size));

	auto t_id = std::this_thread::get_id();
	std::stringstream ss;
    ss << t_id;
    std::string thread_id = ss.str();

	zmq_push(vec.at(index), thread_id, ctx);
    std::cout << thread_id << " " << vec.at(index) << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    return (0);
}

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

void zmq_push(const std::string &message,
    const std::string &thread_id,
    zmq::context_t &ctx)
{
    zmq::socket_t sock(ctx, zmq::socket_type::push);
    sock.bind("ipc://sockets/" + thread_id);
    sock.send(zmq::buffer(message), zmq::send_flags::dontwait);
}

void zmq_pull(zmq::context_t &ctx)
{
	zmq::message_t message;
    zmq::socket_t sock(ctx, zmq::socket_type::pull);
    sock.connect("inproc://socket");
    auto res = sock.recv(message, zmq::recv_flags::none);
	std::cout << "zmq_pull: " << res.value() << "\n";
}

