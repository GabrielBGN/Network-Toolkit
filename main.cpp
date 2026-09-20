#include <iostream>
#include "subnet.h"
#include "dnslookup.h"
#include "portchecker.h"
using namespace std;


void showMenu()
{
    cout << endl;
    cout << "===== C++ Network Toolkit =====" << endl;
    cout << endl;

    cout << "1. Subnet Calculator" << endl;
    cout << "2. DNS Lookup" << endl;
    cout << "3. TCP Port Checker" << endl;
    cout << "4. Port Scanner" << endl;
    cout << "5. Ping" << endl;
    cout << "6. Traceroute" << endl;
    cout << "7. Exit" << endl;

    cout << endl;
    cout << "Choose an option: ";
}


int main()
{
    int choice = 0;

    while (choice != 7)
    {
        showMenu();

        cin >> choice;

        cout << endl;

        if (choice == 1)
        {
            cout << "Opening Subnet Calculator..." << endl;
            subnetcalc();
        }
        else if (choice == 2)
        {
            cout << "Opening DNS Lookup..." << endl;
            dnsLookup();
        }
        else if (choice == 3)
        {
            cout << "Opening TCP Port Checker..." << endl;
            portChecker();
        }
        else if (choice == 4)
        {
            cout << "Opening Port Scanner..." << endl;
        }
        else if (choice == 5)
        {
            cout << "Opening Ping..." << endl;
        }
        else if (choice == 6)
        {
            cout << "Opening Traceroute..." << endl;
        }
        else if (choice == 7)
        {
            cout << "Goodbye!" << endl;
        }
        else
        {
            cout << "Invalid option." << endl;
        }
    }

    return 0;
}