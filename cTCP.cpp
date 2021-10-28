#include <string>
#include <iostream>
#include "cTCP.h"


namespace raven
{
    namespace set {

cTCP::cTCP() : myConnectSocket(INVALID_SOCKET)
{
}

void cTCP::initWinSock()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData))
    {
        throw std::runtime_error("Winsock init failed");
    }
}

void cTCP::acceptClient()
{
    if (isConnected())
        throw std::runtime_error("second connection rejected");
    if (myServerPort.empty())
        throw std::runtime_error("Server not configured");

    initWinSock();

    struct addrinfo *result = NULL,
                    hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    int error = getaddrinfo(
        NULL, myServerPort.c_str(),
        &hints, &result);
    if (error)
    {
        throw std::runtime_error(
            "getaddrinfo failed " + std::to_string(error));
    }

    myAcceptSocket = ::socket(
        result->ai_family,
        result->ai_socktype,
        result->ai_protocol);
    if (myAcceptSocket == INVALID_SOCKET)
    {
        throw std::runtime_error("socket failed");
    }

    if (::bind(myAcceptSocket,
               result->ai_addr,
               (int)result->ai_addrlen) == SOCKET_ERROR)
    {
        closesocket(myAcceptSocket);
        myAcceptSocket = INVALID_SOCKET;
        throw std::runtime_error("bind failed");
    }

    if (::listen(
            myAcceptSocket,
            SOMAXCONN) == SOCKET_ERROR)
    {
        closesocket(myAcceptSocket);
        myAcceptSocket = INVALID_SOCKET;
        throw std::runtime_error("listen failed");
    }

    std::cout << "listening for client on port " << myServerPort << "\n";

    struct sockaddr_in client_info;
    int size = sizeof(client_info);
    SOCKET s = ::accept(
        myAcceptSocket,
        (sockaddr *)&client_info,
        &size);
    if (s == INVALID_SOCKET)
    {
        std::cout << "invalid socket\n";
        return;
    }

    myConnectSocket = s;
    myRemoteAddress = inet_ntoa(client_info.sin_addr);

    closesocket(myAcceptSocket);

    std::cout << "client " << myRemoteAddress << " accepted\n";
}
void cTCP::read()
{
    if (!isConnected())
        throw std::runtime_error("TCP read on invalid socket");

    // clear old message
    ZeroMemory(myReadbuf, 1024);

    // wait to receive message
    int r = ::recv(
        myConnectSocket,
        (char *)myReadbuf, 1024, 0);

    // check for message received
    // if no message or error, assume connection closed
    if (r <= 0)
    {
        std::cout << "connection closed\n";
        closesocket(myConnectSocket);
        myConnectSocket = INVALID_SOCKET;
    }
}
void cTCP::serverWait()
{
    while (1)
    {
        connectToServer();
        if (isConnected())
        {
            return;
        }
    }
}
void cTCP::connectToServer()
{
    if (myServerPort.empty())
        throw std::runtime_error("Server not configured");

    initWinSock();

    struct addrinfo *result = NULL,
                    hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(
            myServerIP.c_str(),
            myServerPort.c_str(),
            &hints, &result))
    {
        throw std::runtime_error("getaddrinfo failed");
    }

    myConnectSocket = ::socket(
        result->ai_family,
        result->ai_socktype,
        result->ai_protocol);
    if (myConnectSocket == INVALID_SOCKET)
    {
        throw std::runtime_error("socket failed");
    }

    if (::connect(
            myConnectSocket,
            result->ai_addr,
            (int)result->ai_addrlen) == SOCKET_ERROR)
    {
        closesocket(myConnectSocket);
        myConnectSocket = INVALID_SOCKET;
    }
}
void cTCP::send(const std::string &msg)
{
    if (myConnectSocket == INVALID_SOCKET)
        throw std::runtime_error("send on invalid socket");
    ::send(
        myConnectSocket,
        msg.c_str(),
        (int)msg.length(), 0);
}

    }
}
