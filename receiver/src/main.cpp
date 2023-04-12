#include <boost/asio.hpp>
#include <fstream>
#include <iostream>

using boost::asio::ip::tcp;
const int PORT = 8686;

void session(tcp::socket sock) {
    try {
        std::string user_name;
        boost::asio::streambuf user_name_buf;
        boost::asio::read_until(sock, user_name_buf, "\n");
        std::istream user_name_stream(&user_name_buf);
        std::getline(user_name_stream, user_name);

        std::string ip_addr = sock.remote_endpoint().address().to_string();
        std::string filename = user_name + "-" + ip_addr + ".txt";

        std::ofstream outfile(filename);

        boost::asio::streambuf buf;
        while (boost::asio::read_until(sock, buf, "\n")) {
            std::string data(boost::asio::buffers_begin(buf.data()),
                             boost::asio::buffers_end(buf.data()));
            outfile << data;
            std::flush(outfile);
            buf.consume(buf.size());
        }
    } catch (std::exception &e) {
        std::cerr << "Exception in session: " << e.what() << "\n";
    }
}

int main() {
    try {
        boost::asio::io_context io_context;

        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), PORT));
        std::cout << "The reciever is running\n";
        while (true) {
            tcp::socket sock(io_context);
            acceptor.accept(sock);
            tcp::endpoint endpoint = sock.remote_endpoint();
            std::cout << "New connection from " << endpoint.address().to_string() << ":"
                      << endpoint.port() << std::endl;
            std::thread(session, std::move(sock)).detach();
        }
    } catch (std::exception &e) {
        std::cerr << "Exception in main: " << e.what() << "\n";
    }

    return 0;
}
