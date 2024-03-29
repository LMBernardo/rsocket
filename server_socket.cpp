//--------------------------
// Server module
//--------------------------
// Description:
// Provides basic TCP/IP socket server functions
//--------------------------
// Author: Layne Bernardo
// Email: lmbernar@uark.edu
//
// Created July 20th, 2018
// Modified: November 7th, 2018
// Version 0.5
//--------------------------

//-----------------------------

#include "server_socket.h"

//-----------------------------
// Class: server_socket
//-----------------------------
// Simplifies use of socket functions
// Provides storage of parameters (e.g., TCP or UDP)
// Also handles creation and configuration of sockaddr_in struct
// Default settings can be found in server_socket.h
//-----------------------------

server_socket::server_socket(){
        // Port to listen on
        s_port = DEFAULT_PORT;
        // Use IPV4 addressing
        s_domain = DEFAULT_ADDRESSING;
        // Use TCP
        s_type = DEFAULT_CONN_TYPE;
        // Use Internet Protocol (IP)
        s_protocol = DEFAULT_PROTOCOL;
        // Bind to localhost
        s_bind_address = DEFAULT_BIND_ADDRESS;
        // How many clients can be waiting for a connection before new clients get rejected
        backlog = DEFAULT_BACKLOG;

        // "Read" buffer
        socket_read_buffer = new char[DEFAULT_SOCKET_BUFFER_SIZE];
        socket_read_buffer_size = DEFAULT_SOCKET_BUFFER_SIZE;

        // Initialize read buffer
        for (int i = 0; i < socket_read_buffer_size; i++) {
                socket_read_buffer[i] = 0;
        }
}

server_socket::server_socket(uint16_t s_p, sa_family_t s_d, int s_ty, int s_pr, int s_b_a, int bl, int s_b_s){
        s_port = s_p;
        s_domain = s_d;
        s_type = s_ty;
        s_protocol = s_pr;
        s_bind_address = s_b_a;
        backlog = bl;
        socket_read_buffer = new char[s_b_s];
        socket_read_buffer_size = s_b_s;

        // Initialize read buffer
        for (int i = 0; i < socket_read_buffer_size; i++) {
                socket_read_buffer[i] = 0;
        }
}

server_socket::~server_socket(){
        // Clean up read buffer
        delete[] socket_read_buffer;
        // Close socket (destructor, can't output to console)
        close(s_sockfd);
}

int server_socket::client_count(){
        return (int) client_list.size();
}

// Initialize socket and listen
// 0 = success, -1 = invalid port, anything else is an errno
int server_socket::s_init(){

        // Check port range
        if ( (s_port < 0) || (s_port > 65534) ) {
                std::cout << "\nPort out of range! (0 to 65534)\n\n";
                return -1;
        }

        // Create and configure socket
        if ( s_create() < 0 ) {
                int errsv = errno;
                std::cout << "Failed to create socket!" << std::endl;
                return errsv;
        }

        // Bind socket to port (or something)
        if ( s_bind() < 0 ) {
                int errsv = errno;
                std::cout << "Failed to bind socket on port " << s_port << std::endl;
                return errsv;
        }

        // Listen for client connections
        if ( s_listen() < 0 ) {
                int errsv = errno;
                std::cout << "Failed to listen on port " << s_port << ", sockfd " << s_sockfd << std::endl;
                return errsv;
        }

        std::cout << "Socket configured. Listening on port " << s_port << std::endl;
        return 0;
}

// Returns file descriptor or -1
int server_socket::s_create(){
        s_sockfd = socket(s_domain, s_type, s_protocol);
        return s_sockfd;
}

// Return < 0 if failed
int server_socket::s_set_nonblocking(){
        // setsockopt(int sockfd, int level, int optname, const void* optval, socklen_t optlen)
        // SOL_SOCKET is at the socket API level
        // Set socket descriptor to be reusable

        int on = 1;
        int err = setsockopt(s_sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
        if (err < 0) {
                err = errno;
                std::cout << "setsockopt() failed" << std::endl;
                return err;
        }
        // Set socket to be non-blocking. Affects all client sockets as well
        err = ioctl(s_sockfd, FIONBIO, (char *)&on);
        if (err < 0)
        {
                err = errno;
                std::cout << "setsockopt() failed" << std::endl;
                return err;
        }
        return 0;
}

// Returns 0 or -1
int server_socket::s_bind(){
        // IPV4
        s_address.sin_family = s_domain;
        // Bind to localhost
        s_address.sin_addr.s_addr = (in_addr_t) s_bind_address;
        // Convert port to network byte order (?)
        s_address.sin_port = htons( s_port );
        return bind(s_sockfd, (struct sockaddr *) &s_address, s_address_len);
}

// Listen on port
int server_socket::s_listen(){
        return listen(s_sockfd, backlog);
}

int server_socket::s_accept(){
        // Open a socket for the client
        int new_client = accept(s_sockfd, (struct sockaddr*) &s_address, &s_address_len);
        if ( new_client  < 0) {
                return -1;
        } else {
                std::cout << "New client: " << new_client << std::endl;
                // new_client is a file descriptor for the new socket
                client_list.push_back(new_client);
                return new_client;
        }
}

int server_socket::remove_client(int client_num){
        client_list.erase(client_list.begin()+client_num);
        return 0;
}

// Get value of largest client fd for select()
int server_socket::max_client_fd(){
        return *( std::max_element(client_list.begin(), client_list.end()) );
}

// Read from the socket buffer of the specified client
long server_socket::s_read(int s_client){
        return (long) read(client_list[s_client], socket_read_buffer, (size_t) socket_read_buffer_size);
}

int server_socket::s_write(char* data){
        std::cout << data << std::endl;
        return -1;
}

long server_socket::socket_read(int client_number){
        long num_bytes = s_read(client_number);
        if ( num_bytes < 0 ) {
                int errsv = errno;
                std::cout << "Error reading from socket!" << std::endl;
                std::cout << "Errno: " << errsv << std::endl;
                return -1;
        } else if ( num_bytes > 0 ) {
                std::cout << num_bytes << " bytes read from socket: " << std::endl;
                for (int i=0; i < num_bytes; i++) {
                        std::cout << socket_read_buffer[i];
                }
                std::cout << std::endl;
                return num_bytes;
        } else {
                return 0;
        }
}

// Splits comma delimited data from the socket buffer into a vector of strings
// Seperates messages via message delimiter '!'
std::vector<std::string> server_socket::splitBuffer(int& start){

        std::string temp_string;
        std::vector<std::string> data_strings;

        // Debug
        std::cout << "Message:";

        char next_char = '0';
        // Put data into vector, stop on '!' or null terminator
        while (next_char != '!' && next_char != '\0') {
                next_char = socket_read_buffer[start];
                if (next_char == ',' || next_char == '!') {
                        data_strings.push_back(temp_string);
                        // Debug
                        std::cout << " " << temp_string;
                        temp_string = "";
                } else {
                        temp_string += next_char;
                }
                start++;
        }

        // Debug
        std::cout << " " << temp_string << std::endl;

        return data_strings;
}

int server_socket::accept_pending_clients(){
        // Create fd_set of clients for select()
        fd_set socket_set;
        int select_result;
        int err;

        // Wait for 100ms on select() calls
        struct timeval delay = {0};
        delay.tv_sec = 0;
        delay.tv_usec = 100;

        // Initialize set
        FD_ZERO(&socket_set);
        // Add server socket to socket settings
        FD_SET(s_sockfd, &socket_set);

        select_result = select(s_sockfd + 1, &socket_set, nullptr, nullptr, &delay);
        err = errno;

    if ( FD_ISSET(s_sockfd, &socket_set ) ){
                // Attempt to accept client connection
                if ( s_accept() < 0 ) {
                        int errsv = errno;
                        std::cout << "Failed to accept client connection" << std::endl;
                        std::cout << "Errno: " << errsv << std::endl;
                } else {
                        std::cout << "Accepted new client connection" << std::endl;
                        std::cout << std::endl;
                }
        }

        if (err == EWOULDBLOCK) {
                return select_result;
        } else {
                return err;
        }
}

std::vector<int> server_socket::check_client_buffers(){
        // Create fd_set of clients for select()
        fd_set socket_set;
        int select_result;
        int err;

        // Wait for 100ms on select() calls
        struct timeval delay = {0};
        delay.tv_sec = 0;
        delay.tv_usec = 100;

        // Result vector. Returns -1,err on error, 0 on timeout, and 1,fd,fd,... for ready fds
        std::vector<int> results;

        // Check for data on each client socket
                // Initialize set
                FD_ZERO(&socket_set);
                // Add clients to socket_set;
                for (auto i : client_list) {
                        FD_SET(i, &socket_set);
                }

                int max_fd = max_client_fd();

                // select(max fd + 1, read socket_set, write socket_set, except socket_set, timeout)
                select_result = select(max_fd + 1, &socket_set, nullptr, nullptr, &delay);
                err = errno;

        // Something went wrong
        if (select_result < 0){
                results.push_back(-1);
                results.push_back(err);
                return results;
        } else if (select_result > 0) {
                results.push_back(1);
                // Check which client sockets are readable
                int desc_ready = select_result;
                for (int i = 0; i < (int) client_list.size() && desc_ready > 0; i++) {
                        if (FD_ISSET(client_list[i], &socket_set)) {
                                // Client socket has data to read
                                desc_ready--;
                                results.push_back(i);
                        }
                }
                return results;
        }
        // No socket has pending data
        results.push_back(0);
        return results;
}
