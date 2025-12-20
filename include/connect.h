#ifndef CON_HPP
#define CON_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

class Connection{
    private:
        int fd;
        vector<char> IP_file_buffer;
        vector<char> mssg_file_buffer; // this buffer will be filled with encrypted chars
        string destination_ip;
        uint16_t header_port;
        uint16_t hardcoded_rec_port = 5000;
    public:
        Connection();
        int port_creation();
        void write_to_file(const string); 
        string output_read_ip_file(const string);
        int read_encrypt_mssg(const string); 
        int establish_connection();
        int send_message_packet();
         

};


#endif