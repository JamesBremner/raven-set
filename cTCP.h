#include <winsock2.h>
#include <ws2tcpip.h>

namespace raven
{
    namespace set {

/// C++ wrapper for winsock ( TCP communications )

class cTCP
{
public:
    cTCP();

    /// Configure server
    void server(
        const std::string &ServerIP,
        const std::string &ServerPort)
    {
        myServerPort = ServerPort;
        myServerIP = ServerIP;
    }

    /** Connect to server
     * 
     * Throws exeption if there is a configuration problem
     * Otherwise blocks until connection
     */
    void serverWait();

    /// Wait for client connection request
    void acceptClient();

    /// Send message to peer
    void send(const std::string &msg);

    /// Wait for message from peer
    void read();

    /// Get last message from peer
    std::string readMsg() const
    {
        return std::string(myReadbuf);
    }

    /// True if peer connected
    bool isConnected()
    {
        return myConnectSocket != INVALID_SOCKET;
    }

private:
    std::string myServerIP;
    std::string myServerPort;
    SOCKET myAcceptSocket;  //< socket listening for clients
    SOCKET myConnectSocket; //< socket connected to another tcp
    std::string myRemoteAddress;
    char myReadbuf[1024];

    void initWinSock();

    /** Connect to server
     * 
     * Throws exception on configuration error
     * 
     * Returns on success or failure of connection
     * Use isConnected() to determine success or failure
     */
    void connectToServer();
};

    }
}
