#include "dnslookup.h"

#include <iostream>
#include <string>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <arpa/inet.h>
#endif

using namespace std;


void dnsLookup()
{
#ifdef _WIN32
    // Windows requires Winsock to be started before
    // using networking functions.
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cout << "Failed to initialize Winsock." << endl;
        return;
    }
#endif


    string hostname;

    cout << "Enter hostname: ";
    cin >> hostname;


    // Stores the type of network information we want
    addrinfo hints{};

    // This will point to the results returned by getaddrinfo()
    addrinfo* result = nullptr;


    // Only request IPv4 addresses
    hints.ai_family = AF_INET;

    // Request addresses that can be used with TCP
    hints.ai_socktype = SOCK_STREAM;


    // Try to resolve the hostname
    int status = getaddrinfo(
        hostname.c_str(),
        nullptr,
        &hints,
        &result
    );


    // Check if the DNS lookup failed
    if (status != 0)
    {
        cout << "DNS lookup failed: "
             << gai_strerror(status)
             << endl;

#ifdef _WIN32
        WSACleanup();
#endif

        return;
    }


    cout << endl;
    cout << "IPv4 addresses for "
         << hostname
         << ":" << endl;


    // Go through every IP address returned
    for (addrinfo* current = result;
         current != nullptr;
         current = current->ai_next)
    {
        // Convert the generic address into an IPv4 address
        sockaddr_in* address =
            reinterpret_cast<sockaddr_in*>(
                current->ai_addr
            );


        // Space to store the readable IP address
        char ip[INET_ADDRSTRLEN];


        // Convert the binary IP into normal dotted format
        const char* converted = inet_ntop(
            AF_INET,
            &(address->sin_addr),
            ip,
            INET_ADDRSTRLEN
        );


        if (converted != nullptr)
        {
            cout << ip << endl;
        }
    }


    // Free the memory created by getaddrinfo()
    freeaddrinfo(result);


#ifdef _WIN32
    // Shut down Winsock when we're finished
    WSACleanup();
#endif
}