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
//void *vec_reader(std::vector<std::string> &vec);
void *vec_reader(
    size_t sok,
    std::vector<std::string> &vec,
    std::vector<std::string> &thread_ids,
    zmq::context_t &ctx);
std::string random_string(std::size_t length);
//void zmq_push(const std::string &message,
//    const std::string &thread_id,
//    zmq::context_t &ctx);
//void zmq_push(const std::string &message);
//void zmq_pull(std::vector<std::string> &thread_ids, zmq::context_t &ctx);

int main(void)
{
    std::vector<std::string> thread_ids;
    std::vector<std::string> vec;
    std::vector<std::thread> th_fire;
    size_t th = 2;
    zmq::context_t ctx;

    size_t iterations = 0;
    while (iterations <= 1000) {
        std::string rnd_string = random_string(10);
        std::thread t1(&vec_writer, std::ref(rnd_string), std::ref(vec));
        // Firing the thread
        t1.join();
        iterations++;
    }

    std::cout << "Vector Ready\n";

    for (size_t t = 1; t <= th; ++t) {
        th_fire.push_back(std::thread (
            &vec_reader,
            t,
            std::ref(vec),
            std::ref(thread_ids),
            std::ref(ctx)));
    }

    //size_t t_counter = 1;
    //for (auto &tid : thread_ids) {
    //    std::cout << t_counter << ": " << tid << "\n";
    //    t_counter++;
    //    usleep(100);
    //}

    for (std::thread &t : th_fire) {
        if (t.joinable()) {
            t.join();
        }
    }

    return EXIT_SUCCESS;
}

void *vec_writer(std::string &message, std::vector<std::string> &vec)
{
    vec.push_back(message);
    //std::this_thread::sleep_for(std::chrono::milliseconds(50));
    return (0);
}

//void *vec_reader(std::vector<std::string> &vec)
//{
//    size_t vec_size = vec.size();
//    size_t index = (0 + (rand() % vec_size));
//
//	// --- Convert the thread ID into string --- //
//    auto t_id = std::this_thread::get_id();
//	std::stringstream ss;
//    ss << t_id;
//    std::string thread_id = ss.str();
//    thread_ids.push_back(thread_id);
//	// --- Convert the thread ID into string --- //
//
//    zmq::context_t ctx;
//	zmq_push(vec.at(index));
//	zmq_push(vec.at(index), thread_id, ctx);
//    std::cout << thread_id << " " << vec.at(index) << "\n";
//    std::this_thread::sleep_for(std::chrono::milliseconds(100));
//
//    return (0);
//}

void *vec_reader(
    size_t sok,
    std::vector<std::string> &vec,
    std::vector<std::string> &thread_ids,
    zmq::context_t &ctx)
{
    size_t vec_size = vec.size();

	// --- Convert the thread ID into string --- //
    auto t_id = std::this_thread::get_id();
	std::stringstream ss;
    ss << t_id;
    std::string thread_id = ss.str();
    thread_ids.push_back(thread_id);
    //size_t th_size = thread_ids.size();
    //std::cout << th_size << "\n";
	// --- Convert the thread ID into string --- //

	//zmq_push(vec.at(index));
	//zmq_push(vec.at(index), thread_id, ctx);
    zmq::socket_t sock(ctx, zmq::socket_type::push);
    //sock.bind("ipc://sockets/" + thread_id);
    sock.bind("ipc://sockets/" + std::to_string(sok));
    //sock.bind("ipc://sockets/0");
    //sock.bind("inproc://socket");
    while(true) {
        size_t index = (0 + (rand() % vec_size));
        std::cout << thread_id << " " << vec.at(index) << "\n";
        sock.send(zmq::buffer(vec.at(index)), zmq::send_flags::dontwait);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

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

//void zmq_push(const std::string &message,
//    const std::string &thread_id,
//    zmq::context_t &ctx)
//{
//    zmq::socket_t sock(ctx, zmq::socket_type::push);
//    //sock.bind("ipc://sockets/" + thread_id);
//    //sock.bind("ipc://sockets/0");
//    sock.bind("tcp://127.0.0.1:3001");
//    sock.send(zmq::buffer(message), zmq::send_flags::dontwait);
//}
//
//void zmq_push(const std::string &message)
//{
//    zmq::context_t ctx;
//    zmq::socket_t sock(ctx, zmq::socket_type::push);
//    //sock.bind("ipc://sockets/" + thread_id);
//    //sock.bind("ipc://sockets/0");
//    sock.bind("tcp://127.0.0.1:3001");
//    sock.send(zmq::buffer(message), zmq::send_flags::dontwait);
//}
//
//void zmq_pull(std::vector<std::string> &thread_ids, zmq::context_t &ctx)
//{
//    // Message Buff preparation
//    const size_t size = 1024;
//    zmq::message_t message(size);
//
//    zmq::socket_t sock_pull(ctx, zmq::socket_type::pull);
//    for (auto &tid : thread_ids) {
//        sock_pull.connect("ipc://sockets/" + tid);
//        // Actual receiving
//        auto res = sock_pull.recv(message, zmq::recv_flags::none);
//        assert(res.value() != 0);
//        std::cout << "zmq_pull: " << message.to_string() << "\n";
//    }
//}

