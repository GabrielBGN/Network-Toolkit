# C++ Network Toolkit

A simple command-line network toolkit written in C++.

## Features

- Subnet Calculator
- DNS Lookup
- TCP Port Checker
- Port Scanner
- Ping
- Traceroute

## Compile

### Windows

Using MinGW / g++:

```powershell
g++ main.cpp portscanner.cpp portchecker.cpp dnslookup.cpp subnetcalc.cpp ping.cpp traceroute.cpp -o network-toolkit.exe -lws2_32
Run
.\network-toolkit.exe
Menu
===== C++ Network Toolkit =====

1. Subnet Calculator
2. DNS Lookup
3. TCP Port Checker
4. Port Scanner
5. Ping
6. Traceroute
7. Exit
Purpose

This project was built to practice C++, socket programming, TCP/IP networking, DNS, multithreading, and basic network troubleshooting.


For educational purposes only. Only scan or test systems you own or have permission to use.


This version is better for GitHub because it gets straight to the point and still tells someone exactly how to compile and run your project.
