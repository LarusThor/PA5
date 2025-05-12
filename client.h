#pragma once
#include "common.h"
#include "message.h"
#include "tsqueue.h"
#include "connection.h"

template<typename T>
class client_interface{

    client_interface(){}

    virtual ~client_interface(){
        Disconnect();
    }

    public:
        bool Connect(const std::string& host, const uint16_t port){
            
            try{

                asio::ip::tcp::resolver resolver(context);
                asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

                connection = std::make_unique<connection<T>>(
                    connection<T>::owner::client,
                    context,
                    asio::ip::tcp::socket(context),
                    m_qMessagesIn
                );
                
                connection->ConnectToServer(endpoints);

                thrContext = std::thread([this]() {context.run();});

            } catch (std::exception& e){
                std::cerr << "Client Exception: " << e.what() << "\n";
                return false;
            }


            return true;
        }

        void Disconnect(){
            if (IsConncted()){
                connection->Disconnect();
            }

            context.stop();
            if (thrContext.joinable())
                thrContext.join();

            connection.release();
        }

        bool IsConncted(){
            if (connection){
                return connection->IsConnected;
            } else {
                return false;
            }
        }

        tsqueue<owned_message<T>>& Incoming(){
            return m_qMessagesIn;
        }

    protected:
        asio::io_context context;
        std::thread thrContext;
        std::unique_ptr<connection<T>> connection;
        
    private:
        tsqueue<owned_message<T>> m_qMessagesIn;

};