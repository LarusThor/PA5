#define ASIO_STANDALONE
#include <asio.hpp>

int main(){
    
    asio::io_context io_context;

    io_context.run();
    
    return 0;

}