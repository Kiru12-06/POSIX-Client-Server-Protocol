#include "connect.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

int main(){
    string ip_file_path = "/home/raspberry/CIS27_Final/layer3/input/IP_destination.txt";
    string enc_message_path = "/home/raspberry/CIS27_Final/layer3/input/enc_message.txt";

    Connection conn_to_client;

    
    
    if (conn_to_client.port_creation() != 0){
        cerr << "Error in socket creation" << endl;
        return -1;
    }

    string dest_path = conn_to_client.output_read_ip_file(ip_file_path);
    if(dest_path.empty()){
        cerr << "Nothing in the IP_destination file" << endl;
        return -1;
    }

    if(conn_to_client.read_encrypt_mssg(enc_message_path) != 0){
        cerr << "Encrypted message isnt entering the message buffer" << endl;
        return -1;
    }

    if(conn_to_client.establish_connection() != 0){
        cerr << "Error in establishign the 3 way handshake" << endl;
        return -1;
    }

    if (conn_to_client.send_message_packet() != 0){
        cerr << "Error in sending the user generated method" << endl;
        return -1;
    }
    
    return 0;


}
