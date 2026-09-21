# C++ Network Toolkit

A command-line networking toolkit written in C++ for practicing networking fundamentals, socket programming, multithreading, and network troubleshooting.

The project combines concepts from **TCP/IP networking** with **C++ systems programming** and provides several common networking utilities through a simple terminal-based menu.

## Features

### Subnet Calculator

Calculates IPv4 subnet information from an IP address and CIDR prefix.

Displays:

* IP address
* CIDR prefix
* Subnet mask
* Network address
* Broadcast address
* Host bits
* Total addresses
* Usable hosts

The subnet calculations are performed using **32-bit integers, bit masks, bit shifts, and bitwise operations** rather than relying on an external networking library.

### DNS Lookup

Resolves a hostname into its associated IPv4 addresses using the system socket API.

Example:

```text
Enter hostname: google.com

IPv4 addresses for google.com:
142.250.x.x
```

### TCP Port Checker

Tests whether a specific TCP port on a hostname or IP address accepts a connection.

Example:

```text
Enter hostname or IP: example.com
Enter port: 443

Port 443 is OPEN
```

This tool uses TCP sockets and `connect()` to test connectivity.

### Multithreaded Port Scanner

Scans a user-defined range of TCP ports on a target host.

The scanner uses:

* Non-blocking sockets
* TCP connection attempts
* Connection timeouts
* Up to 20 worker threads
* `std::atomic`
* `std::mutex`
* `std::thread`

Ports may be reported as:

```text
OPEN
CLOSED
FILTERED / TIMED OUT
UNREACHABLE
ERROR
```

To keep scans controlled while developing and learning, the scanner currently supports ranges of up to **1024 ports at a time**.

### Ping

Tests whether a host is reachable using the operating system's ping utility.

The tool sends four ping requests and reports whether the target responded successfully.

### Traceroute

Displays the network path between the local machine and a destination.

The program automatically uses:

```text
tracert
```

on Windows and:

```text
traceroute
```

on Linux.

The traceroute is limited to a maximum of 30 hops.

---

## Menu

When the program starts, the following menu is displayed:

```text
===== C++ Network Toolkit =====

1. Subnet Calculator
2. DNS Lookup
3. TCP Port Checker
4. Port Scanner
5. Ping
6. Traceroute
7. Exit

Choose an option:
```

---

## Technologies and Concepts

This project was built to practice and demonstrate:

* C++
* TCP/IP networking
* IPv4 addressing
* CIDR subnetting
* Bitwise operations
* TCP sockets
* DNS resolution
* Winsock
* POSIX sockets
* Non-blocking sockets
* Multithreading
* Atomic operations
* Mutex synchronization
* Network troubleshooting
* Cross-platform programming

---

## Project Structure

```text
Network-Toolkit/
│
├── main.cpp
│
├── subnetcalc.cpp
├── subnet.h
│
├── dnslookup.cpp
├── dnslookup.h
│
├── portchecker.cpp
├── portchecker.h
│
├── portscanner.cpp
├── portscanner.h
│
├── ping.cpp
├── ping.h
│
├── traceroute.cpp
├── traceroute.h
│
├── README.md
└── .gitignore
```

Each networking tool is separated into its own source and header files, while `main.cpp` provides the menu-driven interface.

---

## Requirements

### Windows

* C++17-compatible compiler
* MinGW / g++ or another compatible C++ compiler
* Windows Winsock library

### Linux

* C++17-compatible compiler
* g++
* pthread support
* `ping`
* `traceroute`

Some Linux distributions may require the `traceroute` utility to be installed separately.

---

## Building the Project

Clone the repository:

```bash
git clone https://github.com/GabrielBGN/Network-Toolkit.git
cd Network-Toolkit
```

### Windows

Using MinGW / g++:

```bash
g++ -std=c++17 main.cpp subnetcalc.cpp dnslookup.cpp portchecker.cpp portscanner.cpp ping.cpp traceroute.cpp -o network-toolkit.exe -lws2_32
```

Run:

```powershell
.\network-toolkit.exe
```

### Linux

Compile using:

```bash
g++ -std=c++17 main.cpp subnetcalc.cpp dnslookup.cpp portchecker.cpp portscanner.cpp ping.cpp traceroute.cpp -o network-toolkit -pthread
```

Run:

```bash
./network-toolkit
```

---

## Example: Subnet Calculator

```text
Enter IP address: 192.168.1.130
Enter CIDR prefix: /24

------ Subnet Information ------
IP Address: 192.168.1.130
CIDR Prefix: /24
Subnet Mask: 255.255.255.0
Network Address: 192.168.1.0
Broadcast Address: 192.168.1.255
Host Bits: 8
Total Addresses: 256
Usable Hosts: 254
```

This calculation is performed directly with bitwise operations:

```cpp
uint32_t network = ip & mask;
uint32_t broadcast = network | ~mask;
```

---

## Why I Built This

I built this project to strengthen my understanding of both **computer networking and C++** by implementing networking concepts instead of only studying them theoretically.

The project gives me hands-on experience with concepts such as:

* IPv4 subnetting
* TCP connections
* DNS resolution
* Ports and services
* Network reachability
* Routing paths
* Socket programming
* Concurrent network operations

It also provides a practical environment for applying networking concepts while developing stronger systems programming skills.

---

## Future Improvements

Planned improvements include:

*  More robust user input validation
*  First and last usable host calculations
*  Wildcard mask calculation
*  Binary subnet representation
*  Improved port scanner performance and configuration
*  Service identification for common ports
*  CMake build support
*  Unit tests
*  Improved error handling
*  Additional network diagnostic tools

---

## Educational Use

This project is intended for **educational purposes and authorized network testing only**.

Only perform port scans or other network tests against systems that you own or have explicit permission to test.

---

## Author

**Gabriel Balogun**

Computer Science student interested in networking, systems programming, and software development.

GitHub: [GabrielBGN](https://github.com/GabrielBGN)
