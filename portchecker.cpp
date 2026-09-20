#include "portchecker.h"

#include <iostream>
#include <string>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <unistd.h>
#endif

using namespace std;

void portChecker()
{
#ifdef _WIN32
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cout << "Failed to initialize Winsock." << endl;
        return;
    }
#endif

    string hostname;
    string port;

    cout << "Enter hostname or IP: ";
    cin >> hostname;

    cout << "Enter port: ";
    cin >> port;

    addrinfo hints{};
    addrinfo* result = nullptr;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(
        hostname.c_str(),
        port.c_str(),
        &hints,
        &result
    );

    if (status != 0)
    {
        cout << "Could not resolve host." << endl;

#ifdef _WIN32
        WSACleanup();
#endif

        return;
    }

    bool connected = false;

    for (addrinfo* current = result;
         current != nullptr;
         current = current->ai_next)
    {
        int sock = socket(
            current->ai_family,
            current->ai_socktype,
            current->ai_protocol
        );

        if (sock < 0)
        {
            continue;
        }

        if (connect(
                sock,
                current->ai_addr,
                current->ai_addrlen
            ) == 0)
        {
            connected = true;

#ifdef _WIN32
            closesocket(sock);
#else
            close(sock);
#endif

            break;
        }

#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif
    }

    freeaddrinfo(result);

    if (connected)
    {
        cout << "Port " << port
             << " is OPEN on "
             << hostname << endl;
    }
    else
    {
        cout << "Could not connect to port "
             << port << " on "
             << hostname << endl;
    }

#ifdef _WIN32
    WSACleanup();
#endif
}