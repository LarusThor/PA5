#include <iostream>
#include "net.h"
#include "client.h"

enum class CustomMsgTypes : uint32_t {
    Firebullet,
    MovePlayer
};

class CustomClient : public client_interface<CustomMsgTypes>{
    
};

int main(){
    CustomClient c;
    c.Connect("127.0.0.1", 60000);
    return 0;
}
