#include <vector>
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
    /** Configure serverWait() blocking
     * 
     * true: keep trying until connection made ( default on construction )
     * false: if connection refused return after one attempt
     */
    void RetryConnectServer( bool f )
    {
        myfRetryServer = f;
    }
    /** Connect to server
     * 
     * Throws exeption if there is a configuration problem
     * Otherwise blocks until connection
     */
    bool serverWait();

    /// Wait for client connection request
    void acceptClient();

    /// Send message to peer
    void send(const std::string &msg);
    void send( const std::vector< unsigned char >& msg );

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
    bool myfRetryServer;

    void initWinSock();

    /** Connect to server
     * 
     * Throws exception on configuration error
     * 
     * Returns true on success  of connection
     * Returns false if connection timed out
     */
    bool connectToServer();
};

    }
}
