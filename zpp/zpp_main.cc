#include "zmq_push_pull.h"


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
            &zmq_push,
            std::ref(vec),
            std::ref(ctx)));
        th_fire.push_back(std::thread (
            &zmq_pull,
            std::ref(ctx)));
    }

    for (std::thread &t : th_fire) {
        if (t.joinable()) {
            t.join();
        }
    }

    return EXIT_SUCCESS;
}

