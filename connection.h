#pragma once

#include "common.h"
#include "message.h"
#include "tsqueue.h"

template<typename T>
class connection : public std::enable_shared_from_this<connection<T>>{
    public:
        connection(){}

        virtual ~connection(){}
    
    public:
        bool ConnectToServer();
        bool DisconnectFromServer();
        bool IsConnected() const;
    
    public:
        bool Send(const message<T>& msg);
    
    protected:
        asio::ip::tcp::socket socket;
        asio::io_context context;
};
