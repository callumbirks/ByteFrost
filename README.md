Library / CLI app for sending messages to other clients on LAN.
Written in C++. Linux, MacOS & Windows cross-platform, no dependencies.

Only tested so far with Apple Clang on MacOS (x86) and MinGW on Windows.

## High-Level Overview

`Client` is the outward-facing class for using the library. Simply instantiated with
a username string.
Each client identifies themselves via a username.
When the `Client` class is instantiated, it starts a `DiscoveryServer` and a `MessageServer`.
Each of these servers have their own thread to perform work.

The `DiscoveryServer`'s thread is used to send a message over multicast every second, and
listen for multicast messages from other clients. The `MessageServer`'s thread is used to
listen for incoming messages from currently connected clients.

When the `DiscoveryServer` discovers another client, it passes the username and IP address
to a callback which attempts a TCP connection with that client. The client with the higher
IP address (string comparison) will initiate the connection, and the other client will wait
to accept a connection.

Once the clients are connected, they will be added to each other's `MessageServer` list of
available clients, which stores username and socket. `MessageServer`'s thread continuously
polls all sockets of connected clients, and when a message is received through one of these
sockets will invoke a callback with that message.

Messages can be sent between connected clients with the `sendMessage` function of
the `Client` class, provided a username string and message string.

If a connection is dropped from either end, the other client will close that socket and remove
the dropped client from its available clients list.