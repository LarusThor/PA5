#define ASIO_STANDALONE
#include <asio.hpp>
#include <iostream>

std::vector<char> vBuffer(20 * 1024);

struct ClientSession {
    std::string username;
    asio::ip::tcp::socket socket;
};

std::vector<std::shared_ptr<ClientSession>> clients;

int main(){
    
    try{
        asio::io_context io_context;
    
        asio::ip::tcp::acceptor acceptor(io_context, 
        asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 12345));
    
        std::cout << "Server is running at port 12345" << '\n';

        asio::ip::tcp::socket socket(io_context);
        acceptor.accept(socket);

        std::cout << "Client conntected \n";

        std::string response = "Message receieved.\n";

        while(true){
            char data[1024] = {0};
            asio::error_code error;

            size_t length = socket.read_some(asio::buffer(data), error);

            if(error == asio::error::eof){
                std::cout << "Client disconnected \n";
                break;
            } else if(error) {
                throw asio::system_error(error);
            }

            std::cout << "Rec: " << std::string(data, length) << '\n';
        }
        
    }

    catch(std::exception& e){
        std::cerr << "Exception: " << e.what() << '\n';
    }
    
    return 0;

}