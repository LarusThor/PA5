#pragma once
#include "common.h"

#pragma pack(push, 1)

template <typename T>
struct message_header{
    T id{};
    uint32_t size = 0;
};

#pragma pack(pop)

template<typename T>
class connection;

template <typename T>
struct message{
    message_header<T> header{};
    std::vector<uint8_t> body;

    size_t size() const{
        return sizeof(message_header<T>) + body.size();
    }

    friend std::ostream& operator << (std::ostream& os, const message<T>& msg){
        os << "ID: " << int(msg.header.id) << "Size: " << msg.header.size;
        return os;
    }

    template <typename DataType>
    friend message<T>& operator<<(message<T>& msg, const DataType& data)
    {
        static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to serialize");

        size_t i = msg.body.size();
        msg.body.resize(i + sizeof(DataType));
        std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

        msg.header.size = static_cast<uint32_t>(msg.body.size());
        return msg;
    }
        friend message<T>& operator<<(message<T>& msg, const std::string& data)
    {
        uint32_t length = static_cast<uint32_t>(data.size());

        // Reserve space for length + content
        size_t i = msg.body.size();
        msg.body.resize(i + sizeof(uint32_t) + length);

        std::memcpy(msg.body.data() + i, &length, sizeof(uint32_t));
        std::memcpy(msg.body.data() + i + sizeof(uint32_t), data.data(), length);

        msg.header.size = static_cast<uint32_t>(msg.body.size());
        return msg;
    }


    template <typename DataType>
    friend message<T>& operator >> (message<T>& msg, DataType& data){

        static_assert(std::is_standard_layout<DataType>::value, "Data is too complex");
        size_t i = msg.body.size() - sizeof(DataType);

        std::memcpy(&data, msg.body.data() + i, sizeof(DataType));

        msg.body.resize(i);
        msg.header.size = static_cast<uint32_t>(msg.body.size());

        return msg;
    }

    // Deserialize std::string from the message
    friend message<T>& operator>>(message<T>& msg, std::string& data)
    {
        uint32_t length;
        msg >> length; // Read string length

        data.resize(length);
        std::memcpy(data.data(), msg.body.data() + msg.body.size() - length, length);
        msg.body.resize(msg.body.size() - length);
        msg.header.size = static_cast<uint32_t>(msg.body.size());

        return msg;
    }

    
};

template<typename U>
struct owned_message
{
    std::shared_ptr<connection<U>> remote = nullptr;
    message<U> msg;

    friend std::ostream& operator<<(std::ostream& os, const owned_message<U>& msg)
    {
        os << msg.msg;
        return os;
    }
};