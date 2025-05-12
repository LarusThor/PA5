#pragma once

#include "common.h"
#include "message.h"
#include "tsqueue.h"

template<typename T>
class connection : public std::enable_shared_from_this<connection<T>>{
    public:

        enum class owner{
            server,
            client
        };

        connection(owner parent, asio::io_context& asioContext, assio::ip::Tcp::socket socket, tsqueue<owned_message<T>>& qIn) : asioContext(asioContext), socket(std::move(socket)), m_qMessagesIn(qIn)
        {
            m_nOwnerType = parent;
        }

        virtual ~connection(){}

        uint32_t GetID() const{
            return id;
        }
    
    public:
        void ConnectToClient(uint32_t uid = 0){
            if (nOwnerType == owner::server){
                if (socket.is_open()){
                    id = uid;
                    ReadHeader();
                }
            }
            
        }

        bool ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints){
            if (nOwnerType == owner::client){
                asio::async_connect(socket, endpoints, 
                [this](std::error_code ec, std::size_t length){
                    if (!ec){
                        ReadHeader();
                    }
                })
            }
        };

        bool Disconnect(){
            if (IsConnected()){
                asio::post(context, [this]() { socket.close() });
            }
        };

        bool IsConnected() const{
            return socket.is_open();
        };
    
    public:
        bool Send(const message<T>& msg){
            asio::post(context, [this, msg](){
                bool WritingMessage = !m_qMessagesOut.empty();
                m_qMessagesOut.push_back(msg);
                if (!WritingMessage){
                    WriteHeader();
                }
            })

        };

    private:
        void ReadHeader(){
            asio::async_read(socket, asio::buffer(&msgTemporaryIn.header, sizeof(message_header<T>)),
            [this](std::error_code ec, std::size_t length){
            if (!ec){
                if (msgTemporaryIn.header.size > 0){
                    msgTemporaryIn.body.resize(msgTemporaryIn.header.size);
                    ReadBody();
                } else {
                    AddToIncomingMessageQueue();
                }
            } else {
                std::cout << "[" << id << "] Read Header Fail.\n";
                socket.close();
            }}
        )};
        
        void ReadBody(){
            asio::async_read(socket, asio::buffer(msgTemporaryIn.body.data(), msgTemporaryIn.body.size())
            [this](std::error_code ec, std::size_t length){
                
                if(!ec){
                    AddToIncomingMessageQueue();
                } else {
                    std::cout << "[" << id << "] Read Body Fail.\n";
                    socket.close();
                }

            })
        }

        void WriteHeader(){
            asio::async_write(socket, asio::buffer(&m_qMessagesOut.front().header, sizeof(message_header<T>))
                [this](std::error_code ec, std::size_t length){
                    if (!ec){

                        if (m_qMessagesOut.front().body.size() > 0){
                            WriteBody();
                        }

                    } else {
                        m_qMessagesOut.pop_front();

                        if (!m_qMessagesOut.empty()){
                            WriteHeader();
                        }
                    } 
                }
        )}

        void WriteBody(){
            asio::async_write(socket, asio::buffer(m_qMessagesOut.front().body.data(), m_qMessagesOut.front().body.size()),
                [this](std::error_code ec, std::size_t length){
                    if (!ec){
                        m_qMessagesOut.pop_front();

                        if (!m_qMessagesOut.empty()){
                            WriteHeader();
                        }

                    } else {
                        std::cout << "[" << id << "] Write Body Fail\n";
                        socket.close();
                    }
                })
        }

        void AddToIncomiingMessageQueue(){
            if(nOwnerType == owner::server){
                m_qMessagesIn.push_back({ this->shared_from_this(), msgTemporaryIn });
            } else {
                m_qMessagesIn.push_back({ nullptr, msgTemporaryIn});
            }

            ReadHeader();
        }
    protected:
        asio::ip::tcp::socket socket;
        asio::io_context context;
        tsqueue<message<T>> m_qMessagesOut;
        tsqueue<owned_message<T>>& m_qMessagesIn;
        message<T> msgTemporaryIn;
        owner nOwnerType = owner::server;
        uint32_t id = 0;
};
