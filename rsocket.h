//--------------------------
// rsocket Header
//--------------------------
// Description:
// Provides basic TCP/IP socket functions
//--------------------------
// Author: Layne Bernardo
// Email: lmbernar@uark.edu
//
// Created July 20th, 2018
// Modified: March 16th, 2019
// Version 0.5
//--------------------------

#ifndef _rsocket_H_INCLUDED
#define _rsocket_H_INCLUDED

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
// Default bind address (localhost)
#define DEFAULT_BIND_ADDRESS INADDR_ANY
// Default max # of pending client connections on inet socket
// NOTE: Pending clients are clients who are trying to connect but have not been accepted yet,
//       this is not a limit on the number of active clients
#define DEFAULT_BACKLOG 4
// Default socket buffer size
#define DEFAULT_SOCKET_BUFFER_SIZE 4096
//-----------------------------

// Includes
//-----------------------------
// Standard libraries
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <sys/ioctl.h>
#include <algorithm>
// Socket / inet libraries
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
//-----------------------------

// Allows storage of parameters for socket functions
// Also handles creation and configuration of sockaddr_in struct
// TODO: Add asynchronous read loop
class server_socket
{
    private:
        // Socket file descriptor
        int s_sockfd = 0;

    public:

        // FIXME: Make this private and have server_socket_handler keep track of its own clients
        // Connected clients list
        std::vector<int> client_list;

        // Port number
        uint16_t s_port;
        // What type of addressing (i.e., IPV4)
        sa_family_t s_domain;
        // Connection type (i.e., TCP)
        int s_type;
        // Connection protocol (i.e., internet protocol)
        int s_protocol;
        // Address to bind to (i.e., localhost)
        int s_bind_address;
        // How many clients can be waiting for a connection before new clients get rejected
        int backlog;

        // "Read" buffer
        char* socket_read_buffer;
        // "Read" buffer size
        int socket_read_buffer_size;

        // netinet/in.h defined address struct
        struct sockaddr_in s_address;
        socklen_t s_address_len = (socklen_t) sizeof(s_address);

        server_socket();
        server_socket(uint16_t s_p, sa_family_t s_d, int s_ty, int s_pr, int s_b_a, int bl, int s_b_s);
        //-----------s_port, s_domain, s_type, s_protocol, s_bind_address, backlog, socket_read_buffer_size
        ~server_socket();

        int client_count();

        int s_init();

        // Returns file descriptor or -1
        int s_create();

        // Setup socket for non-blocking select()
        int s_set_nonblocking();

        // Returns 0 or -1
        int s_bind();

        // Listen on port
        int s_listen();

        // Accept client connection
        int s_accept();

        // Remove a client
        int remove_client(int client_num);

        // Get largest client fd
        int max_client_fd();

        // Read from buffer
        long s_read(int);

        int s_write(char*);

        long socket_read(int client_number);

        std::vector<std::string> splitBuffer(int& start);

        int accept_pending_clients();

        std::vector<int> check_client_buffers();

};

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

#endif // rsocket.h
