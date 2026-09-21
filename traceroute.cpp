#include "traceroute.h"

#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>

using namespace std;


// --------------------------------------------------
// Validate hostname or IP
// --------------------------------------------------

bool isValidTraceTarget(const string& target)
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
// Traceroute Tool
// --------------------------------------------------

void tracerouteTool()
{
    string target;

    cout << endl;
    cout << "Opening Traceroute Tool..." << endl;

    cout << "Enter hostname or IP: ";
    cin >> target;


    // Validate input
    if (!isValidTraceTarget(target))
    {
        cout << "Invalid hostname or IP address." << endl;
        return;
    }


    cout << endl;
    cout << "Tracing route to " << target << "..."
         << endl;

    cout << endl;


    string command;


#ifdef _WIN32

    // Windows:
    //
    // tracert = Windows traceroute command
    // -h 30   = maximum of 30 hops
    // -w 1000 = wait 1000 ms for each reply

    command =
        "tracert -h 30 -w 1000 \"" +
        target +
        "\"";

#else

    // Linux:
    //
    // -m 30 = maximum of 30 hops
    // -w 1  = wait 1 second for each reply

    command =
        "traceroute -m 30 -w 1 \"" +
        target +
        "\"";

#endif


    // Run traceroute
    int result = system(command.c_str());


    cout << endl;
    cout << "------ Traceroute Finished ------"
         << endl;


    if (result == 0)
    {
        cout << "Traceroute completed successfully."
             << endl;
    }
    else
    {
        cout << "Traceroute failed or was interrupted."
             << endl;
    }
}