#include "zpp.h"


//Payload *pload = (Payload *) malloc(sizeof(Payload));

void initPayload(
    Payload **pload_, const char *random_str, const char *simple_str)
{
    Payload *pload = (Payload *) malloc(sizeof(Payload));

    size_t length_random_str = (strlen(random_str) + 1);
    size_t length_simple_str = (strlen(simple_str) + 1);
    pload->random_str = strndup(random_str, strlen(random_str));
    pload->simple_str = strndup(simple_str, strlen(random_str));

    *pload_ = pload;
    //fprintf(stderr, "Alloced: %p\n", pload);
}

void freePayload(Payload *pload)
{
    free(pload->random_str);
    free(pload->simple_str);
    free(pload);
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
    const size_t size = sizeof(Payload*);

    zmq::socket_t sock(ctx, zmq::socket_type::push);
    size_t vec_size = vec.size();

    // --- Convert the thread ID into string --- //
    auto t_id = std::this_thread::get_id();
    std::stringstream ss;
    ss << t_id;
    std::string thread_id = ss.str();
    // --- Convert the thread ID into string --- //

    std::string sok = "ipc:///tmp/sockets0";
    //std::cout << "Thread " << thread_id << " PUSH-ing to " << sok << "\n";
    sock.connect(sok);
    while(true) {
        // Randomly reading from the the Random's string vector
        size_t index = (0 + (rand() % vec_size));
        initPayload(&pload, vec.at(index).c_str(), "simple");
	std::cerr << "[" << t_id <<  "] Sending (" << pload << ") " <<
		"simple: " << pload->simple_str << ", random: "
		<< pload->random_str << std::endl;
        zmq::message_t message(&pload, size);
        sock.send(message, zmq::send_flags::none);
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
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

    std::string sok = "ipc:///tmp/sockets0";
    //std::cout << "PULL-ing from " << sok << "\n";
    sock.bind(sok);
    while(true) {
        auto res = sock.recv(message, zmq::recv_flags::none);

	Payload* pload = *(Payload **)message.data();
	std::cerr << "[PULL] Receiving (" << pload << ") " <<
		"simple: " << pload->simple_str << ", random: "
		<< pload->random_str << std::endl;
        //std::this_thread::sleep_for(std::chrono::milliseconds(200));
        freePayload(pload);
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

