#ifndef SER_HPP
#define SER_HPP

#include <sys/types.h>
#include <sys/socket.h> // Core socket functions (socket, bind, listen, accept)
#include <netinet/in.h> // Internet address family definitions (sockaddr_in, INADDR_ANY)
#include <arpa/inet.h>  // Functions for IP address conversion (inet_ntop)
#include <unistd.h>    // POSIX API functions (close)
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>


using namespace std;

class ConnectServer{
    private:
        int listen_fd;
        int conn_fd;
        uint16_t hardcode_rec_port = 5000;
        vector<char> rec_enc_mssg_buffer;
        char xorKey = 'P';

    public:
        ConnectServer();
        int setup_listener();
        int accept_device();
        ssize_t read_all(int, void*, size_t); // the read_all returns a number of bytes so the type used is ssize_t also so that i can output a negative number when a error occurs
                                           // the int parameter is for a communication channel fd
                                          // the pointer to a destination in the buffer is of type void so the type of the buffer can be anything
                                          // size_t is the parameter that holds the length of the mssage coming through and its length is size_t because the legth size can vary

        int recive_message();
        void encrypt_decrypt();
        void display_message();
        

                          
};




#endif