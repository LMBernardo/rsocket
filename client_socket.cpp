//--------------------------
// Client module
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

#include "client_socket.h"

//------------------------
// Class: client_socket
//------------------------
//TODO: Store host address and port

client_socket::client_socket(){
        // Socket port (default 9999)
        c_port = DEFAULT_PORT;
        // Use IPV4 addressing
        c_domain = DEFAULT_ADDRESSING;
        // Use TCP
        c_type = DEFAULT_CONN_TYPE;
        // Use Internet Protocol (IP)
        c_protocol = DEFAULT_PROTOCOL;

        // Create receive buffer
        socket_read_buffer = new char[DEFAULT_SOCKET_BUFFER_SIZE];
        socket_read_buffer_size = DEFAULT_SOCKET_BUFFER_SIZE;
        for (int i=0; i<socket_read_buffer_size; i++) {
                socket_read_buffer[i] = 0;
        }
}

client_socket::client_socket(uint16_t c_p, sa_family_t c_d, int c_ty, int c_pr){
        // Socket port (default 9999)
        c_port = c_p;
        // Addressing / domain (e.g., IPV4)
        c_domain = c_d;
        // Connection type (e.g., TCP)
        c_type = c_ty;
        // Protocol (e.g., Internet Protocol)
        c_protocol = c_pr;

        // Create receive buffer
        socket_read_buffer = new char[DEFAULT_SOCKET_BUFFER_SIZE];
        socket_read_buffer_size = DEFAULT_SOCKET_BUFFER_SIZE;
        for (int i=0; i<socket_read_buffer_size; i++) {
                socket_read_buffer[i] = 0;
        }
}


client_socket::~client_socket(){
        // Clean up read buffer
        delete[] socket_read_buffer;
}

// Create socket
int client_socket::c_create(){
        return c_sockfd = socket(c_domain, c_type, c_protocol);
}

// Connect to host $hostname on port $port_input
int client_socket::c_connect(const char* hostname, uint16_t port_input){
        c_port = port_input;
        // IPV4
        c_server.sin_family = c_domain;
        // Convert port to network byte order (?)
        c_host = gethostbyname(hostname);
        if (c_host == nullptr) {
                int errsv = errno;
                std::cout << "Host not found: " << hostname << std::endl;
                std::cout << "Errno: " << errsv << std::endl;
                return -1;
        }
        // Taken from: http://www.linuxhowtos.org/C_C++/socket.htm
        // Copies IP address from c_host struct to c_server struct
        // (c_host struct contains results of dns query on the hostname, we need to put the IP address into our c_server struct to connect)
        bcopy(c_host->h_addr,
              (char *)&c_server.sin_addr.s_addr,
              (size_t) c_host->h_length);

        // Convert port to network byte order (?)
        c_server.sin_port = htons( c_port );

        // Connect to $c_server on socket $c_sockfd
        if ( connect(c_sockfd,(struct sockaddr*)&c_server,c_server_len) < 0 ) {
                int errsv = errno;
                std::cout << "Error connecting to host: " << hostname << std::endl;
                std::cout << "Errno: " << errsv << std::endl;
                return -1;
        } else {
                return 0;
        }
}

// Read bytes from socket into socket_read_buffer
int client_socket::c_read(){
        if ( read(c_sockfd, socket_read_buffer, (size_t) socket_read_buffer_size) < 0) {
                int errsv = errno;
                std::cout << "Error reading from host" << std::endl;
                std::cout << "Errno: " << errsv << std::endl;
                return -1;
        } else {
                return 0;
        }
}

// Write bytes to socket, appending message length
// TODO: This has become massively overcomplicated
int client_socket::c_write_len(const char* s_data){

        //---------------------
        // Append message length to data so we can separate commands

        // Get length of data
        auto s_data_length = (int) strlen(s_data);
        // Convert int length to char*
        char s_data_length_char[10];
        sprintf(s_data_length_char,"%d,",s_data_length);

        // Get length of the length string (>.>)
        auto s_length_length = (int) strlen(s_data_length_char);

        // Make new array which will be length of message appended to original data
        char s_data_new[s_length_length + s_data_length];

        // Create new array [length,data1,data2,...]
        std::copy(s_data_length_char, s_data_length_char + s_length_length, s_data_new);
        // +1 to catch the null character
        std::copy(s_data, s_data + s_data_length+1, s_data_new + s_length_length);

        // Convert to const char* to send
        const char* s_data_new_const = s_data_new;
        //---------------------

        // Finally send the data
        if ( write(c_sockfd, s_data_new_const, strlen(s_data_new_const)) < 0 ) {
                int errsv = errno;
                std::cout << "Error sending to host" << std::endl;
                std::cout << "Errno: " << errsv << std::endl;
                std::cout << "Data: \n" << s_data << std::endl;
                return -1;
        } else {
                return 0;
        }
}

// Write bytes to socket with delimiter '!'
int client_socket::c_write_delim(const char* s_data){

        auto s_data_length = (int) strlen(s_data);

        // Make new array which will of size strlen(message) + 2 (! and \0)
        char s_data_new[s_data_length + 2];
        char delim[2] = {'!','\0'};

        // Append '!' to char array
        std::strcpy(s_data_new,s_data);
        std::strcat(s_data_new,delim);

        // Convert to const char* to send
        const char* s_data_new_const = s_data_new;
        //---------------------

        // Finally send the data
        if ( write(c_sockfd, s_data_new_const, strlen(s_data_new_const)) < 0 ) {
                int errsv = errno;
                std::cout << "Error sending to host" << std::endl;
                std::cout << "Errno: " << errsv << std::endl;
                std::cout << "Data: \n" << s_data << std::endl;
                return -1;
        } else {
                return 0;
        }
}

// Convert string to const char[] and write to socket
int client_socket::send_string(std::string data_string){
        const char * data_c_str = data_string.c_str();

        if ( c_write_delim(data_c_str) < 0 ) {
                int errsv = errno;
                std::cout << "Failed to send data" << std::endl;
                std::cout << "Errno: " << errsv << std::endl;
                return -1;
        } else {
                std::cout << "Data sent." << std::endl;
                return 0;
        }
}

// Close socket
void client_socket::c_close(){
        close(c_sockfd);
}
