#pragma once
#include "common.h"
#include "message.h"
#include "tsqueue.h"
#include "connection.h"

template<typename T>
class client_interface{

    public:
        client_interface(){}

        virtual ~client_interface(){
            Disconnect();
        }

        
        void Send(const message<T>& msg)
        {
            std::cout << "Entering Send function \n";
            if (IsConnected()){
                    std::cout << "IsConnected resolves to true.. \n";
                    std::cout << msg << "\n";
                    mainConnection->Send(msg);
                }
        }

    public:
        bool Connect(const std::string& host, const uint16_t port){
            
            try{

                asio::ip::tcp::resolver resolver(context);
                asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

                
                mainConnection = std::make_unique<connection<T>>(
                    owner::client,
                    context,
                    asio::ip::tcp::socket(context),
                    m_qMessagesIn
                );
                
                mainConnection->ConnectToServer(endpoints);

                thrContext = std::thread([this]() {context.run();});

            } catch (std::exception& e){
                std::cerr << "Client Exception: " << e.what() << "\n";
                return false;
            }


            return true;
        }

        void Disconnect(){
            if (IsConnected()){
                mainConnection->Disconnect();
            }

            context.stop();
            if (thrContext.joinable())
                thrContext.join();

            mainConnection.release();
        }

        bool IsConnected(){
            if (mainConnection){
                return mainConnection->IsConnected();
            } else {
                return false;
            }
        }

        tsqueue<owned_message<T>>& Incoming(){
            return m_qMessagesIn;
        }

    public:
        
        virtual void OnMessage(message<T>& msg)
            {
            }

    protected:
        asio::io_context context;
        std::thread thrContext;
        std::unique_ptr<connection<T>> mainConnection;
        
    private:
        tsqueue<owned_message<T>> m_qMessagesIn;

};