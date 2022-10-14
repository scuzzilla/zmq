# ZMQ - multi-threads PUSH/PULL

### Requirements (debian):
```SHELL
$ sudo apt install libczmq-dev
```

### How-to build:
```SHELL
- ZMQ Push:
$ g++ -O2 -Wall -pedantic zmq_push.cc -o bin/zmq_push -lzmq

- ZMQ Pull:
$ g++ -O2 -Wall -pedantic zmq_pull.cc -o bin/zmq_pull -lzmq
```

### Run
```SHELL
shell_1$ bin/zmq_push

shell_2$ bin/zmq_pull
```

