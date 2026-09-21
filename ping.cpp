#include "ping.h"

#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>

using namespace std;


// --------------------------------------------------
// Check that the hostname/IP contains only
// characters we expect.
// --------------------------------------------------

bool isValidPingTarget(const string& target)
{
    if (target.empty())
    {
        return false;
    }

    for (char c : target)
    {
        if (!isalnum(static_cast<unsigned char>(c)) &&
            c != '.' &&
            c != '-' &&
            c != ':')
        {
            return false;
        }
    }

    return true;
}


// --------------------------------------------------
// Ping Tool
// --------------------------------------------------

void pingTool()
{
    string target;

    cout << endl;
    cout << "Opening Ping Tool..." << endl;

    cout << "Enter hostname or IP: ";
    cin >> target;


    // Validate input
    if (!isValidPingTarget(target))
    {
        cout << "Invalid hostname or IP address." << endl;
        return;
    }


    cout << endl;
    cout << "Pinging " << target << "..." << endl;
    cout << endl;


    string command;


#ifdef _WIN32

    // Windows:
    //
    // -n 4     = send 4 echo requests
    // -w 1000  = wait up to 1000 ms per reply

    command =
        "ping -n 4 -w 1000 \"" +
        target +
        "\"";

#else

    // Linux:
    //
    // -c 4 = send 4 echo requests
    // -W 1 = wait up to 1 second per reply

    command =
        "ping -c 4 -W 1 \"" +
        target +
        "\"";

#endif


    // Execute the operating system's ping command
    int result = system(command.c_str());


    cout << endl;
    cout << "------ Ping Tool Finished ------"
         << endl;


    if (result == 0)
    {
        cout << "Host responded successfully."
             << endl;
    }
    else
    {
        cout << "Host did not respond or ping failed."
             << endl;
    }
}