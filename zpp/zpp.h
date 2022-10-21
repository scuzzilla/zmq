#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <string.h>


typedef struct {
    char *random_str;
    char *simple_str;
} __attribute__ ((packed)) Payload;

//extern Payload *pload;
void initPayload(
    Payload **pload_, const  char *random_str, const char *simple_str);
void freePayload(Payload *pload);
void vec_writer(std::string &message, std::vector<std::string> &vec);
void *zmq_push(std::vector<std::string> &vec, zmq::context_t &ctx);
void zmq_pull(zmq::context_t &ctx);
std::string random_string(std::size_t length);

