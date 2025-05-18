#include <iostream>
#include "net.h"
#include "client.h"
#include "connection.h"

enum class CustomMsgTypes : uint32_t {
    ServerAccept,
    ServerPing,
	MessageAll,
    ServerMessage,
    SetUsername
};

class CustomClient : public client_interface<CustomMsgTypes>{
public:
    void PingServer()	
        {
            message<CustomMsgTypes> msg;
            msg.header.id = CustomMsgTypes::ServerPing;

            int64_t now = std::chrono::system_clock::now().time_since_epoch().count();

            msg << now;
            this->Send(msg);
        }
    void MessageAll(const std::string& text)
	{
        std::cout << "Testing for entering message \n";
		message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::MessageAll;	
        msg << text;	
		Send(msg);
	}

    void SetUsername(const std::string& name){
        message<CustomMsgTypes> msg;
        msg.header.id = CustomMsgTypes::SetUsername;
        msg << name;
        Send(msg);
    }

public:
    virtual void OnMessage(message<CustomMsgTypes>& msg) override
    {
        switch (msg.header.id)
        {

        case CustomMsgTypes::ServerAccept:
            std::cout << "[Client] Successfully connected to the server!\n";
            break;

        case CustomMsgTypes::ServerMessage:
        {
            std::string sender;
            std::string content;
            msg >> content >> sender;

            std::cout << "[" << sender << "]: " << content << "\n";
            break;
        }

        case CustomMsgTypes::MessageAll:
        {
            std::string sender;
            std::string content;
            msg >> content >> sender;

            std::cout << "[" << sender << "]: " << "\n";
            break;
        }

        default:
            std::cout << "Unknown message from server. ID: " << (int)msg.header.id << "\n";
            break;
        }
    }

};

int main(){
    
    //TODO: Make messages write out in the server and display username of client, currently does nothing but ask for username

    CustomClient client;
    client.Connect("127.0.0.1", 60000);

    // Wait for ServerAccept
    while (client.Incoming().empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Handle first incoming message (should be ServerAccept)
    auto msg = client.Incoming().pop_front();
    client.OnMessage(msg.msg);

    // ✅ NOW safe to send username
    std::string username;
    std::cout << "Enter your username: ";
    std::getline(std::cin, username);
    client.SetUsername(username);

    // Proceed as normal
    while (true) {
        if (!client.Incoming().empty()) {
            auto msg = client.Incoming().pop_front();
            client.OnMessage(msg.msg);
        }

        std::string text;
        std::getline(std::cin, text);
        if (text == "quit")
            break;
        client.MessageAll(text);
    }
    return 0;
}
