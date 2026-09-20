#include "portscanner.h"

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <algorithm>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <cerrno>
#endif

using namespace std;


// How long each connection attempt can wait
const int CONNECTION_TIMEOUT_MS = 500;


// Possible results for a TCP port
enum class PortState
{
    PORT_OPEN,
    PORT_CLOSED,
    PORT_FILTERED_OR_TIMED_OUT,
    PORT_UNREACHABLE,
    SCAN_ERROR
};


// Stores the result of one scanned port
struct PortResult
{
    int port;
    PortState state;
};


// --------------------------------------------------
// Convert operating-system errors into port states
// --------------------------------------------------

#ifdef _WIN32

PortState errorToState(int errorCode)
{
    if (errorCode == WSAECONNREFUSED)
    {
        return PortState::PORT_CLOSED;
    }

    if (errorCode == WSAETIMEDOUT)
    {
        return PortState::PORT_FILTERED_OR_TIMED_OUT;
    }

    if (errorCode == WSAEHOSTUNREACH ||
        errorCode == WSAENETUNREACH)
    {
        return PortState::PORT_UNREACHABLE;
    }

    return PortState::SCAN_ERROR;
}

#else

PortState errorToState(int errorCode)
{
    if (errorCode == ECONNREFUSED)
    {
        return PortState::PORT_CLOSED;
    }

    if (errorCode == ETIMEDOUT)
    {
        return PortState::PORT_FILTERED_OR_TIMED_OUT;
    }

    if (errorCode == EHOSTUNREACH ||
        errorCode == ENETUNREACH)
    {
        return PortState::PORT_UNREACHABLE;
    }

    return PortState::SCAN_ERROR;
}

#endif


// --------------------------------------------------
// Check one TCP port
// --------------------------------------------------

PortState checkPort(const string& hostname, int port)
{
    addrinfo hints{};
    addrinfo* result = nullptr;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    string portString = to_string(port);


    int status = getaddrinfo(
        hostname.c_str(),
        portString.c_str(),
        &hints,
        &result
    );


    if (status != 0)
    {
        return PortState::PORT_UNREACHABLE;
    }


    PortState finalState = PortState::SCAN_ERROR;


    for (addrinfo* current = result;
         current != nullptr;
         current = current->ai_next)
    {

#ifdef _WIN32

        SOCKET sock = socket(
            current->ai_family,
            current->ai_socktype,
            current->ai_protocol
        );

        if (sock == INVALID_SOCKET)
        {
            continue;
        }


        // Put the socket into non-blocking mode
        u_long mode = 1;

        if (ioctlsocket(sock, FIONBIO, &mode) != 0)
        {
            closesocket(sock);
            continue;
        }

#else

        int sock = socket(
            current->ai_family,
            current->ai_socktype,
            current->ai_protocol
        );

        if (sock < 0)
        {
            continue;
        }


        int flags = fcntl(sock, F_GETFL, 0);

        if (flags == -1)
        {
            close(sock);
            continue;
        }


        // Put the socket into non-blocking mode
        if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1)
        {
            close(sock);
            continue;
        }

#endif


        // Attempt the TCP connection
        int connectResult = connect(
            sock,
            current->ai_addr,
            current->ai_addrlen
        );


        // Connection succeeded immediately
        if (connectResult == 0)
        {
            finalState = PortState::PORT_OPEN;

#ifdef _WIN32
            closesocket(sock);
#else
            close(sock);
#endif

            break;
        }


        // --------------------------------------------------
        // Check immediate connection errors
        // --------------------------------------------------

#ifdef _WIN32

        int connectError = WSAGetLastError();

        if (connectError != WSAEWOULDBLOCK &&
            connectError != WSAEINPROGRESS)
        {
            finalState = errorToState(connectError);

            closesocket(sock);
            continue;
        }

#else

        int connectError = errno;

        if (connectError != EINPROGRESS)
        {
            finalState = errorToState(connectError);

            close(sock);
            continue;
        }

#endif


        // --------------------------------------------------
        // Connection is still in progress
        // --------------------------------------------------

        fd_set writeSet;

        FD_ZERO(&writeSet);
        FD_SET(sock, &writeSet);


        timeval timeout;

        timeout.tv_sec =
            CONNECTION_TIMEOUT_MS / 1000;

        timeout.tv_usec =
            (CONNECTION_TIMEOUT_MS % 1000) * 1000;


#ifdef _WIN32

        int selectResult = select(
            0,
            nullptr,
            &writeSet,
            nullptr,
            &timeout
        );

#else

        int selectResult = select(
            sock + 1,
            nullptr,
            &writeSet,
            nullptr,
            &timeout
        );

#endif


        // Nothing responded before the timeout
        if (selectResult == 0)
        {
            finalState =
                PortState::PORT_FILTERED_OR_TIMED_OUT;
        }

        // select() itself failed
        else if (selectResult < 0)
        {
            finalState =
                PortState::SCAN_ERROR;
        }

        // Socket responded
        else
        {
            int socketError = 0;


#ifdef _WIN32

            int errorLength =
                sizeof(socketError);

            int socketStatus = getsockopt(
                sock,
                SOL_SOCKET,
                SO_ERROR,
                reinterpret_cast<char*>(&socketError),
                &errorLength
            );

#else

            socklen_t errorLength =
                sizeof(socketError);

            int socketStatus = getsockopt(
                sock,
                SOL_SOCKET,
                SO_ERROR,
                &socketError,
                &errorLength
            );

#endif


            if (socketStatus != 0)
            {
                finalState =
                    PortState::SCAN_ERROR;
            }
            else if (socketError == 0)
            {
                finalState =
                    PortState::PORT_OPEN;
            }
            else
            {
                finalState =
                    errorToState(socketError);
            }
        }


#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif


        // Once an open port is found,
        // we don't need to check other addresses
        if (finalState == PortState::PORT_OPEN)
        {
            break;
        }
    }


    freeaddrinfo(result);

    return finalState;
}


// --------------------------------------------------
// Convert PortState into readable text
// --------------------------------------------------

string stateToString(PortState state)
{
    switch (state)
    {
        case PortState::PORT_OPEN:
            return "OPEN";

        case PortState::PORT_CLOSED:
            return "CLOSED";

        case PortState::PORT_FILTERED_OR_TIMED_OUT:
            return "FILTERED / TIMED OUT";

        case PortState::PORT_UNREACHABLE:
            return "UNREACHABLE";

        case PortState::SCAN_ERROR:
            return "ERROR";
    }

    return "UNKNOWN";
}


// --------------------------------------------------
// Main port scanner
// --------------------------------------------------

void portScanner()
{

#ifdef _WIN32

    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cout << "Failed to initialize Winsock."
             << endl;

        return;
    }

#endif


    string hostname;

    int startPort;
    int endPort;


    cout << "Enter hostname or IP: ";
    cin >> hostname;

    cout << "Enter starting port: ";
    cin >> startPort;

    cout << "Enter ending port: ";
    cin >> endPort;


    // Validate port range
    if (startPort < 1 ||
        endPort > 65535 ||
        startPort > endPort)
    {
        cout << "Invalid port range." << endl;

#ifdef _WIN32
        WSACleanup();
#endif

        return;
    }


    // Keep the scan small while learning
    if ((endPort - startPort + 1) > 1024)
    {
        cout << "Please scan 1024 ports or fewer."
             << endl;

#ifdef _WIN32
        WSACleanup();
#endif

        return;
    }


    cout << endl;

    cout << "Scanning "
         << hostname
         << " from port "
         << startPort
         << " to "
         << endPort
         << "..."
         << endl;

    cout << "Timeout: "
         << CONNECTION_TIMEOUT_MS
         << " ms"
         << endl;


    // Stores the next port waiting to be scanned
    atomic<int> nextPort(startPort);


    // Stores all scan results
    vector<PortResult> results;


    // Prevent multiple threads from modifying
    // the vector at the same time
    mutex resultsMutex;


    int totalPorts =
        endPort - startPort + 1;


    // Use up to 20 threads
    int workerCount =
        min(20, totalPorts);


    vector<thread> workers;


    // --------------------------------------------------
    // Start worker threads
    // --------------------------------------------------

    for (int i = 0; i < workerCount; i++)
    {
        workers.emplace_back([&]()
        {
            while (true)
            {
                int port =
                    nextPort.fetch_add(1);


                if (port > endPort)
                {
                    break;
                }


                PortState state =
                    checkPort(hostname, port);


                lock_guard<mutex>
                    lock(resultsMutex);


                results.push_back(
                    {port, state}
                );
            }
        });
    }


    // --------------------------------------------------
    // Wait for all worker threads to finish
    // --------------------------------------------------

    for (thread& worker : workers)
    {
        worker.join();
    }


    // Sort results by port number
    sort(
        results.begin(),
        results.end(),

        [](const PortResult& a,
           const PortResult& b)
        {
            return a.port < b.port;
        }
    );


    int openCount = 0;
    int closedCount = 0;
    int filteredCount = 0;
    int unreachableCount = 0;
    int errorCount = 0;


    cout << endl;
    cout << "------ Scan Results ------"
         << endl;


    for (const PortResult& result : results)
    {
        switch (result.state)
        {
            case PortState::PORT_OPEN:
                openCount++;
                break;

            case PortState::PORT_CLOSED:
                closedCount++;
                break;

            case PortState::PORT_FILTERED_OR_TIMED_OUT:
                filteredCount++;
                break;

            case PortState::PORT_UNREACHABLE:
                unreachableCount++;
                break;

            case PortState::SCAN_ERROR:
                errorCount++;
                break;
        }


        // Don't print every closed port.
        // Only show the interesting results.
        if (result.state != PortState::PORT_CLOSED)
        {
            cout << "Port "
                 << result.port
                 << ": "
                 << stateToString(result.state)
                 << endl;
        }
    }


    cout << endl;
    cout << "------ Summary ------"
         << endl;

    cout << "Open:              "
         << openCount << endl;

    cout << "Closed:            "
         << closedCount << endl;

    cout << "Filtered/Timeout:  "
         << filteredCount << endl;

    cout << "Unreachable:       "
         << unreachableCount << endl;

    cout << "Errors:            "
         << errorCount << endl;


#ifdef _WIN32
    WSACleanup();
#endif
}