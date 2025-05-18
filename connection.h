#pragma once

#include "common.h"
#include "message.h"
#include "tsqueue.h"

enum class owner{
    server,
    client
};

template<typename T>
class connection : public std::enable_shared_from_this<connection<T>>{
    public:

        connection(owner parent, 
            asio::io_context& asioContext,
             asio::ip::tcp::socket socket,
              tsqueue<owned_message<T>>& qIn) : context(asioContext),
               socket(std::move(socket)),
                m_qMessagesIn(qIn),
                nOwnerType(parent)
        {
        }

        virtual ~connection(){}

        uint32_t GetID() const{
            return id;
        }

        void SetUsername(const std::string& name) {
            username = name;
        }

        std::string GetUsername() const {
            return username;
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
                [this](std::error_code ec, asio::ip::tcp::endpoint){
                    if (!ec){
                        ReadHeader();
                    }
                });
            }
            return true;
        };

        bool Disconnect(){
            if (IsConnected()){
                asio::post(context, [this]() { socket.close(); });
            }
            return true;
        };

        bool IsConnected() const{
            return socket.is_open();
        };
    
    public:
        void Send(const message<T>& msg){
            asio::post(context, [this, msg](){
                bool WritingMessage = !m_qMessagesOut.empty();
                m_qMessagesOut.push_back(msg);
                if (!WritingMessage){
                    std::cout << "Testing to see if we enter write header func\n";
                    WriteHeader();
                }
            });
        };

    private:

        void ReadHeader(){
            std::cout << "[" << id << "] Server: Waiting to read header...\n";
            asio::async_read(socket, asio::buffer(&msgTemporaryIn.header, sizeof(message_header<T>)),
            [this](std::error_code ec, std::size_t length){
            if (!ec){
                std::cout << "[" << id << "] Server: Header read, size = " << msgTemporaryIn.header.size << "\n";
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
        );}
        
        void ReadBody(){
            asio::async_read(socket, 
                asio::buffer(msgTemporaryIn.body.data(), msgTemporaryIn.body.size()),
            [this](std::error_code ec, std::size_t length){
                
                if(!ec){
                    AddToIncomingMessageQueue();
                } else {
                    std::cout << "[" << id << "] Read Body Fail.\n";
                    socket.close();
                }

            }
        );
    }

        void WriteHeader(){
            std::cout << "We entered the write header function\n";
            asio::async_write(socket, 
                asio::buffer(&m_qMessagesOut.front().header, sizeof(message_header<T>)),
                [this](std::error_code ec, std::size_t length){
                    if (!ec){
                            std::cout << "We didn't encounter an error\n";
                        if (m_qMessagesOut.front().body.size() > 0)
							{
                                std::cout << "We are entering WriteBody function\n";
								WriteBody();
							}
							else
							{
								m_qMessagesOut.pop_front();

								if (!m_qMessagesOut.empty())
								{
									WriteHeader();
								}
							}

                    } else {
                        std::cout << "[" << id << "] Write Header Fail.\n";
							socket.close();
                        }
                    }
        );
    }

        void WriteBody(){
            std::cout << "We entered the WriteBody function\n";
            asio::async_write(socket, asio::buffer(m_qMessagesOut.front().body.data(), m_qMessagesOut.front().body.size()),
                [this](std::error_code ec, std::size_t length){
                    if (!ec){
                            std::cout << "We didn't encounter an error\n";
                            m_qMessagesOut.pop_front();

                            if (!m_qMessagesOut.empty())
                            {   
                                std::cout << "We are going recursively now\n";
                                WriteHeader();
                            }

                    } else {
                        std::cout << "[" << id << "] Write Body Fail\n";
                        socket.close();
                    }
                });
        }

        void AddToIncomingMessageQueue(){
            if(nOwnerType == owner::server){
                m_qMessagesIn.push_back({ this->shared_from_this(), msgTemporaryIn });
            } else {
                m_qMessagesIn.push_back({ nullptr, msgTemporaryIn});
            }
            std::cout << "[" << id << "] Server: Finished processing message, waiting for next...\n";
            ReadHeader();
        }
    protected:
        asio::ip::tcp::socket socket;
        asio::io_context& context;
        tsqueue<message<T>> m_qMessagesOut;
        tsqueue<owned_message<T>>& m_qMessagesIn;
        message<T> msgTemporaryIn;
        owner nOwnerType = owner::server;
        uint32_t id = 0;
        std::string username;
};
