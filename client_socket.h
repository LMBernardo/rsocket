//--------------------------
// Client module header
//--------------------------
// Description:
// Provides basic TCP/IP socket client functions
//--------------------------
// Author: Layne Bernardo
// Email: lmbernar@uark.edu
//
// Created July 20th, 2018
// Modified: November 7th, 2018
// Version 0.5
//--------------------------

#ifndef _C_SOCKET_H_INCLUDED_
#define _C_SOCKET_H_INCLUDED_

// Default settings
//-----------------------------
// Default listening port
#define DEFAULT_PORT 9999
// Default addressing mode (IPV4)
#define DEFAULT_ADDRESSING AF_INET
// Default connection type (TCP)
#define DEFAULT_CONN_TYPE SOCK_STREAM
// Default protocol (Internet Protocol)
#define DEFAULT_PROTOCOL 0
// Default socket buffer size
#define DEFAULT_SOCKET_BUFFER_SIZE 4096
//-----------------------------

#include <cstdlib>
#include <iostream>
#include <cstring>

// Inet libraries
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// Read / write
#include <unistd.h>

class client_socket
{
public:
        // Socket file descriptor
        int c_sockfd = 0;
        // netinet/in.h defined address struct
        struct sockaddr_in c_server;
        socklen_t c_server_len = (socklen_t) sizeof(c_server);
        // Host struct
        struct hostent *c_host;

        // Socket port
        uint16_t c_port;
        // Addressing mode / domain (e.g., IPV4)
        sa_family_t c_domain;
        // Connection type (e.g., TCP)
        int c_type;
        // Protocol (e.g., Internet Protocol)
        int c_protocol;

        // "Receive" buffer
        char* socket_read_buffer;
        int socket_read_buffer_size;

        client_socket();
        client_socket(uint16_t c_p, sa_family_t c_d, int c_ty, int c_pr);
        ~client_socket();

        // Create socket
        int c_create();

        // Connect to host $hostname on port $port_input
        // Verbose
        int c_connect(const char* hostname, uint16_t port_input);

        // Read from socket into socket_read_buffer
        // Verbose
        int c_read();

        // Write to socket (prepend message length)
        // Verbose
        int c_write_len(const char* data);

        // Write to socket (append delimiter '!')
        // Verbose
        int c_write_delim(const char* data);

        // Convert string into const char[] and c_write to socket
        // Verbose
        int send_string(std::string data_string);

        void c_close();
};

#endif // c_socket.h
