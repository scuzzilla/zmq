#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <zmq.hpp>
#include <zmq_addon.hpp>


typedef struct {
    const char *random_str;
} payload;

void vec_writer(std::string &message, std::vector<std::string> &vec);
void *zmq_push(std::vector<std::string> &vec, zmq::context_t &ctx, payload *blob);
void zmq_pull(zmq::context_t &ctx);
std::string random_string(std::size_t length);

