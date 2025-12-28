#include "connect.h"

using namespace std; 

ConnectServer :: ConnectServer(){

}

int ConnectServer :: setup_listener(){
     listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (listen_fd < 0){
        cerr << "failed to create the reciving channel" << endl;
        return -1;
    }

    int optval = 1; // this is a value that the method will read and use to turn on the feature passed into the parrameter
    if(setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0){
        // this function allows for the server to reause the port whenever a crash happens so that when you restart quickly the
        // address can be reused and wont be populated and the optvalue 1 is the value that will be passed into the function
        // to turn the SO_REUSEADDR option on

        cerr << "cant set the socket to be reused in case of crash" << endl;
    }


    // these next lines are simmilar to my port creation method on the transmitting side because they are just populating the
    // sercer_addr object with the specific information that will be binded to the socket that im creating. the socket needs
    // the port and also needs to know what type of IP will be passed in (AD_INET wich specifies IPV4) and the type of iterfaces
    // that can be passed through the port is anything

    // NOTE* the memset function is a function that i didnt use in the transmitting side but is good for safty and practice. look
    // up what its for
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(hardcode_rec_port); 
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);


    // this is the bind wich I talk about on the transmitting side but a short summary is to bind all of the info that I wrote to
    // the server_addr object to the communication channel for the server (listen fd)
    if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "SERVER ERROR: Bind failed on port " << hardcode_rec_port << ": " << strerror(errno) << endl;
        close(listen_fd);
        return -1;
    }



    // the listen method is a method that limits the number devices that can complete the three way handshake
    // and the max is 10 and in the next method we will decide witch device to accept messages from
    if (listen(listen_fd, 10) < 0){
        cerr << "Cant set up the Backlogs of the current devices that have completed the 3 way handshake" << endl;
        close(listen_fd);
        return -1;
    }

    cout << "Port for server has been set up" << endl;

    return 0;

}

int ConnectServer :: accept_device(){
    if (listen_fd < 0){
        cerr << "the setup listener method probably hasnt ran correctly" << endl;
        return -1;
    }

    struct sockaddr_in output_addr;     // becuase this is the output addr for the conn_fd it dosnt need to have specified types
    socklen_t output_add_len = sizeof(output_addr);// like the other sockaddr_in structs for the other file descriptors (fd's)
                                        // because it will be copying the type of address that the device's being accepted are with the accept function below


    // something diffrent about this new communication channel is that it gets duplicated based on the device that is next up
    // on the backlog queue of the listen_fd channel that was created in the setup listener method.
    // the listen_fd channel holds the diffrent devices that have completed the 3 way handshake and when the accept function runs
    // it will accept connection based on wich device has been held in the queue the longest. the acceptance of the message comes later 
    conn_fd = accept(listen_fd, (struct sockaddr*)&output_addr, &output_add_len);

    if (conn_fd < 0) {
        cerr << "SERVER ERROR: Accept failed: " << strerror(errno) << endl; // the errno just tell me specifically what failled
        return -1;
    }

    char conn_device_ip [INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(output_addr.sin_addr), conn_device_ip, INET_ADDRSTRLEN);

    cout << "The connected devices IP is: " << conn_device_ip << ". The port that the divice is operating on is: " << ntohs(output_addr.sin_port) << endl;

    

    return (0);
    
}

// the transmitter side will send the inital SYN packet to begin the three way handshake with its establish connection method and the 
// servers kernel will respond back and forth with the transmitting device and once the three way handshake is completed is when the 
// kernel will save the connection in the listen fd channel and once the accept device method is ran on the reciver side the saved connection
// will have its own conn_fd channel wich we will use the recive method to recive the message being sent through that conn_fd channel.

ssize_t ConnectServer :: read_all(int fd, void* buffer, size_t length){
    size_t total_size = 0;
    char* ptr = (char*)buffer; // casting the buffer in the parameter as char so that i can have a pointer to a destination in a char buffer

    while(length > total_size){ // this while loop is used to keep using the recv function until the total number of bytes that have been read equal to 
                    // the length passed through 
        ssize_t recived_bytes = recv(fd, ptr + total_size, length - total_size, 0); // for the recv function first you pass in the communication channel you 
                                // want it to read hence the fd in the first and the ptr + total_size is the current destination of the loop incase i havent 
                                // read the total number of bytes during the first iteration so first if the total_size is 0 and ptr points to the start of the buffer
                            // then you start at the start of the netwrok buffer that holds the message packet and if the first iteration dosnt read the entire buffre
                        // during the next iteration it picks up where it left off and starts reading and it also updates the length it reads for when it picks up along the nextiteration
                        // all the way until it reads the whole buffer
        if (recived_bytes == 0){
            cerr << "connection has been closed by the client" << endl;
            return -1;
        }

        if (recived_bytes < 0){
            cerr << "failed to read the incoming message packet" << strerror(errno) << endl;
            return -1;
        }
        total_size += recived_bytes;
    }

    return total_size;
}

// the method above lets us do two things. 
// based on what the targeted start of the parameter passed inside the second argument the read_all dose two things
// first if you were to pass in a byte size type variable in the second parameter the function calculates the total size of the message packet
// but if you pass in the start of a buffer then you will begin writing the message packet into the buffer

int ConnectServer :: recive_message(){
    if (conn_fd < 0){
        cerr << "cant read message packet inside conn_fd channel" << endl;
        return -1;
    }

    // A variable to temporarily hold the initial 4 byte header that will come in before the actual message packet
    uint32_t header_mssg_size; // this 32 bit header will tell us how many bytes of encrypted message will be coming through 
    size_t initial_header_size = sizeof(header_mssg_size); // and with this information we will know what size to expect for the message buffer

    ssize_t bytes_recived = read_all(conn_fd, &header_mssg_size, initial_header_size);

    if (bytes_recived != initial_header_size){
        cerr << "cant read the full size of the header, it may not be in 32bits like we expected" << endl;
        return -1;
    }

    // now that header _mssg_size holds the value of the 4 byte header that came thru we can read it to know how big the size of the
    // message packet comming through the conn_fd is but we need to conver the size from network byte order into host byte order
    // with the function we worked with already

    uint32_t header_mssg_host = ntohl(header_mssg_size);
    cout << "the size of the message coming through the channel is: " << header_mssg_host << " bytes." << endl;

    if (header_mssg_host == 0) {
        cout << "Not an error, we just read the header mssg coming through the channel, there is no message packet being sent thotough the channel yet" << endl;
        return 0;
    }

    rec_enc_mssg_buffer.resize(header_mssg_host); // now we have sized the buffer reciving the message and it knows the size of the message coming through


    // now we re run the read_all but this time the start destination is a buffers biggining point (rec_enc_mssg_buffer.data()) so it starts copying the encrypted message comming thorugh
    // the channel conn_fd into the rec_enc_mssg_buffer
    bytes_recived = read_all(conn_fd, rec_enc_mssg_buffer.data(), header_mssg_host);

    if (bytes_recived != header_mssg_host) { 
        cerr << "RECEIVE ERROR: Failed to read the full message data." << endl;
        return -1;
    }

    cout << "Successfully received " << bytes_recived << " bytes of encrypted data." << endl;

    return 0; // Success


}

void ConnectServer :: encrypt_decrypt()
{
    // perform XOR operation of key
    // with every character in string
    for (int i = 0; i < rec_enc_mssg_buffer.size(); i++)
    {
        rec_enc_mssg_buffer[i] = rec_enc_mssg_buffer[i] ^ xorKey;
        
    }
}

void ConnectServer :: display_message() {
    std::cout << "\nDecrypted Message Received" << std::endl;
    
    // Uses the classic loop structure with an index 'i'
    for (size_t i = 0; i < rec_enc_mssg_buffer.size(); i++) {
        // Accesses the character using the index 'i'
        std::cout << rec_enc_mssg_buffer[i];
    }
    
  
}

