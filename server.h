#pragma once

#include "common.h"
#include "tsqueue.h"
#include "message.h"
#include "connection.h"
#include <algorithm>

template<typename T>
class server_interface{

    public:
        server_interface(uint16_t port) : asioAcceptor(asioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)){

        }

        virtual ~server_interface(){
            Stop();
        }

        bool Start(){

            try {
                WaitforClientConnection();

                threadContext = std::thread([this]() {asioContext.run();});
            } catch(std::exception& e) {
                std::cerr << "[SERVER] Exception: " << e.what() << std::endl;
            }

            std::cout << "[SERVER] Started!\n";
            return true;
        }

        bool Stop(){
           asioContext.stop();
            
           if(threadContext.joinable()){
            threadContext.join();
           }

           std::cout << "[SERVER] Stopped!\n";
           return true;
        }

        // ASYNC
        void WaitforClientConnection(){
            asioAcceptor.async_accept([this](std::error_code ec, asio::ip::tcp::socket socket){

                if(!ec){
                    std::cout << "[SERVER] New Connection: " << socket.remote_endpoint() << "\n";
                    
                    std::shared_ptr<connection<T>> newConn = 
                        std::make_shared<connection<T>>(owner::server, 
                            asioContext, 
                            std::move(socket), 
                            m_qMessageIn);

                    if(OnClientConnect(newConn)){
                        deqConnections.push_back(std::move(newConn));
                        deqConnections.back()->ConnectToClient(nIDCounter++);
                        std::cout << "[" << deqConnections.back()->GetID() << "] Connection Approved\n";
                    } else {
                        std::cout << "[-----] Connection Denied.\n";
                    }
                } else {
                    std::cout << "[SERVER] New Connection Error: " << ec.message() << "\n";
                }

                WaitforClientConnection();
            });
        }

        void MessageClient(std::shared_ptr<connection<T>> client, const message<T>& msg){
            if(client && client.IsConected()){
                client->Send(msg);
            } else {
                OnClientDisconnect(client);
                client.reset();
                deqConnections.erase(std::remove(deqConnections.begin(), deqConnections.end(), client),deqConnections.end());
            }
        }

        void MessageAllClients(const message<T>& msg, std::shared_ptr<connection<T>> pIgnoreClient = nullptr){
            bool InvalidClientExists = false;

            for(auto& client : deqConnections){
                if(client && client->IsConnected()){
                    if(client != pIgnoreClient){
                        client->Send(msg);
                    }
                } else {
                    OnClientDisconnect(client);
                    client.reset();
                    InvalidClientExists = true;
                }
            }
        }

        void Update(size_t nMaxMessages = -1, bool bWait = false){
            if(bWait) m_qMessageIn.wait();
            
            size_t nMessages = 0;
            while(nMessages < nMaxMessages && !m_qMessageIn.empty()){
                auto msg = m_qMessageIn.pop_front();
                this->OnMessage(msg.remote, msg.msg);
                nMessages++;
            }
        }

    protected:
        virtual bool OnClientConnect(std::shared_ptr<connection<T>> client){
            return false;
        }

        virtual void OnClientDisconnect(std::shared_ptr<connection<T>> client){
            std::cout << "[Server] Client disconnected.\n";
        }

        virtual void OnMessage(std::shared_ptr<connection<T>> client, message<T>& msg)
        {

		}
    
    protected:
        tsqueue<owned_message<T>> m_qMessageIn;
        asio::io_context asioContext;
        std::thread threadContext;
        std::deque<std::shared_ptr<connection<T>>> deqConnections;

        asio::ip::tcp::acceptor asioAcceptor;
        uint32_t nIDCounter = 100;



};