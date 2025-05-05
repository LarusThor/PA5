#define ASIO_STANDALONE
#include <asio.hpp>
#include <iostream>

std::vector<char> vBuffer(20 * 1024);

struct ClientSession {
    std::string username;
    asio::ip::tcp::socket socket;

    ClientSession(asio::io_context& io_context) : socket(io_context){}
};
// [1]: Hello

std::vector<std::shared_ptr<ClientSession>> clients;

int main(){
    


    try{
        asio::io_context io_context;
        
        // Create an acceptor to listen on TCP port 12345
        asio::ip::tcp::acceptor acceptor(io_context, 
            asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 12345));
        
        //asio::error_code ec;
        //asio::ip::tcp::endpoint endpoint(asio::ip::make_address("93.184.216.34", ec), 12345);
        std::cout << "Server is running at port 12345" << '\n';

        asio::ip::tcp::socket socket(io_context); 
        acceptor.accept(socket);

        ClientSession client(io_context);

        std::string username;
        std::cout << "Write your username: \n";
        std::cin >> username;

        client.socket = std::move(socket);
        client.username = username;

        std::cout << "Client conntected \n";

        std::string response = "Message receieved.\n";

        while(true){
            char data[1024] = {0};
            asio::error_code error;
            std::string message = "[ " + client.username + " ]:\n";
            asio::write(client.socket, asio::buffer(message), error);

            size_t length = client.socket.read_some(asio::buffer(data), error);

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