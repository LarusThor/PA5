#include <iostream>
#include "net.h"

enum class CustomMsgTypes : uint32_t
{
    ServerAccept,
    ServerDeny,
	ServerPing,
	MessageAll,
	ServerMessage,
	SetUsername
};

class CustomServer : public server_interface<CustomMsgTypes>
{
public:
    CustomServer(uint16_t nPort) : server_interface<CustomMsgTypes>(nPort)
	{

	};

protected:
    virtual bool OnClientConnect(std::shared_ptr<connection<CustomMsgTypes>> client)
	{
		message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::ServerAccept;
		client->Send(msg);
		return true;
	}

    virtual void OnClientDisconnect(std::shared_ptr<connection<CustomMsgTypes>> client)
	{
		std::cout << "Removing client [" << client->GetID() << "]\n";
	}

    virtual void OnMessage(std::shared_ptr<connection<CustomMsgTypes>> client, message<CustomMsgTypes>& msg)
	{
		switch (msg.header.id)
		{
		case CustomMsgTypes::ServerPing:
		{
			std::cout << "[" << client->GetID() << "]: Server Ping\n";

			client->Send(msg);
		}
		break;

		case CustomMsgTypes::SetUsername:
		{
			std::string name;
			msg >> name;
			client->SetUsername(name);  // Save it on the server side
			std::cout << "[SERVER] Client " << client->GetID() << " set username to: " << name << "\n";
			break;
		}

		case CustomMsgTypes::MessageAll:
		{
			std::string content;
			msg >> content;

			std::string sender = client->GetUsername();
			message<CustomMsgTypes> broadcast;
			broadcast.header.id = CustomMsgTypes::ServerMessage;
			broadcast << content << sender;
			MessageAllClients(broadcast, client);
			std::cout << "[SERVER] Broadcast from " << sender << ": " << content << "\n";
		}
		break;

		case CustomMsgTypes::ServerMessage:
		{
			std::string sender;
			std::string content;
			msg >> content >> sender;

			std::cout << "[" << sender << "]: " << content << "\n";
		}
		break;
		}
	}
};

int main()
{

    CustomServer server(60000);
    server.Start();

    while(1){

        server.Update(-1, true);

    }
    
    return 0;
}

