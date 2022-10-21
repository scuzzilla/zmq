#include "zpp.h"


//Payload *pload = (Payload *) malloc(sizeof(Payload));

void initPayload(
    Payload **pload_, const char *random_str, const char *simple_str)
{
    Payload *pload = *pload_;
    pload = (Payload *) malloc(sizeof(Payload));
    //memset(pload, 0, sizeof(Payload));

    size_t length_random_str = (strlen(random_str) + 1);
    size_t length_simple_str = (strlen(simple_str) + 1);
    pload->random_str = (char *) malloc(length_random_str);
    memset(pload->random_str, 0, (length_random_str * sizeof(char)));
    strncpy(pload->random_str, random_str, length_random_str);
    pload->simple_str = (char *) malloc(length_simple_str);
    memset(pload->simple_str, 0, (length_simple_str * sizeof(char)));
    strncpy(pload->simple_str, simple_str, length_simple_str);
}

void freePayload(Payload *pload)
{
    free(pload->random_str);
    free(pload->simple_str);
    //free(pload);
}

// Populate the vector with random strings - single thread
void vec_writer(std::string &message, std::vector<std::string> &vec)
{
    vec.push_back(message);
    //std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

// Read from the vector & ZMQ PUSH - multiple threads
void *zmq_push(
    std::vector<std::string> &vec,
    zmq::context_t &ctx)
{
    Payload *pload = NULL;

    // Message Buff preparation
    // PUSH-ing only the pointer to the data-struct
    const size_t size = sizeof(Payload);

    zmq::socket_t sock(ctx, zmq::socket_type::push);
    size_t vec_size = vec.size();

    // --- Convert the thread ID into string --- //
    auto t_id = std::this_thread::get_id();
    std::stringstream ss;
    ss << t_id;
    std::string thread_id = ss.str();
    // --- Convert the thread ID into string --- //

    std::string sok = "ipc://sockets/0";
    //std::cout << "Thread " << thread_id << " PUSH-ing to " << sok << "\n";
    sock.connect(sok);
    while(true) {
        // Randomly reading from the the Random's string vector
        size_t index = (0 + (rand() % vec_size));
        // Populating the struct with rnd string
        initPayload(&pload, vec.at(index).c_str(), "simple");
        //std::cout << thread_id << " " << vec.at(index) << "\n";
        zmq::message_t message(&pload, size);
        sock.send(message, zmq::send_flags::none);
        //std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return (NULL);
}

// Read from socks - multiple threads
void zmq_pull(zmq::context_t &ctx)
{
    zmq::socket_t sock(ctx, zmq::socket_type::pull);

    // Message Buff preparation
    const size_t size = sizeof(Payload);
    zmq::message_t message(size);

    // --- Convert the thread ID into string --- //
    auto t_id = std::this_thread::get_id();
    std::stringstream ss;
    ss << t_id;
    std::string thread_id = ss.str();
    // --- Convert the thread ID into string --- //

    std::string sok = "ipc://sockets/0";
    //std::cout << "PULL-ing from " << sok << "\n";
    sock.bind(sok);
    while(true) {
        auto res = sock.recv(message, zmq::recv_flags::none);
        if (res.value() != 0) {
            std::cout << thread_id << " PULL-ing from " << sok << ": "
                << ((Payload *) message.handle())->random_str << " "
                << ((Payload *) message.handle())->simple_str << "\n";
        }
        //std::this_thread::sleep_for(std::chrono::milliseconds(200));
        freePayload(((Payload *) message.data()));
    }
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

