//
// Created by callu on 30/09/2023.
//

#ifndef BYTEFROST_SOCK_UTILITY_H
#define BYTEFROST_SOCK_UTILITY_H

// Utilities for multi-platform support between Unix and Win32

#ifdef _WIN32
#define SOCK_T SOCKET
#define SOCK_ERR(sock) sock == INVALID_SOCKET
#define SOCK_READ(sock, buf, len) recv(sock, buf, len, 0)
#define SOCK_WRITE(sock, buf, len) send(sock, buf, len, 0)
#define SOCK_POLL(fds, nfds, timeout) WSAPoll(fds, nfds, timeout)
#define SOCK_CLOSE(sock) closesocket(sock)
#else
#define SOCK_T int32_t
#define SOCK_ERR(sock) sock < 0
#define SOCK_READ(sock, buf, len) read(sock, buf, len)
#define SOCK_WRITE(sock, buf, len) write(sock, buf, len)
#define SOCK_POLL(fds, nfds, timeout) poll(fds, nfds, timeout)
#define SOCK_CLOSE(sock) close(sock)
#endif

#endif  // BYTEFROST_SOCK_UTILITY_H
