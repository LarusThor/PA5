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
    
    return 0;
}
