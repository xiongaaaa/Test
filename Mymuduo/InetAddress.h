#pragma once

#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>

//封装socket地址类型
class Inetaddress
{
public:
    explicit Inetaddress(uint16_t port,std::string ip = "127.0.0.1");
    explicit Inetaddress(const struct sockaddr_in &addr):addr_(addr){}

    std::string toIp() const;
    std::string toIpPort() const;
    uint16_t toPort() const;

    const struct sockaddr_in* getSockAddr() const {return &addr_; }
    void setSockAddr(const struct sockaddr_in &addr) { addr_ = addr;}
private:
    struct sockaddr_in addr_;
};