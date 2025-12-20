#include "connect.h"

#include <arpa/inet.h>



using namespace std;

Connection :: Connection() : fd(-1){

    mssg_file_buffer.clear();
    
}

int Connection :: port_creation(){
    this -> fd = socket(AF_INET, SOCK_STREAM, 0);  // fd is a ID to the communication channel that the kernel assigns memory to
                                               // so essentially this is the start of the communication channel and we need the 
                                               // port of this channel and what type of communication it uses. 
     if (this ->fd < 0){                               
        perror("kernel cant assign memory to start the communication channel");
        return -1;
    }

    sockaddr_in address_port{};
    address_port.sin_family = AF_INET;
    address_port.sin_addr.s_addr = htonl(INADDR_ANY);
    address_port.sin_port = htons(0);  // kernel looks for a open port and writes that number inside a specific field of 
                                       // address_port{}
    
                                       
    int bnd = bind(this ->fd, (sockaddr*)&address_port, sizeof(address_port)); // bind assigns the port to the kernel's communication channel
                                                                        // id named fd
    if ( bnd < 0){
        perror ("binding port address to kernel ID failed");
        close(this ->fd);
        return -1;
    }
    
    sockaddr_in port_assign{};
    socklen_t len = sizeof(port_assign);

    int sock = getsockname(this->fd, (sockaddr*)&port_assign, &len);  // get sockname writes the port number from fd inside a specific field
                                                                // of port_assign{}

    if (sock < 0){
        perror("the kernel cant read the port number inside fd");
        close(this ->fd);
        return -1;
    }

    uint16_t port = ntohs(port_assign.sin_port);
    cout << "Port assigned: " << port << endl;

    //close(fd); for now we will comment this out because I want to keep a port open continuously so that this port would be assigned
    // to communication using my protol but I may change this if instead of communicating to a server im communicating to another device
    
    

    return 0;


}
string Connection :: output_read_ip_file(const string filename) {
    FILE* file_ptr = fopen(filename.c_str(), "r"); 
    if (file_ptr == NULL) {
        cerr << "Error: Cannot open file " << filename << " for reading." << endl;
        return "";
    }

    IP_file_buffer.clear();    // these lines clear up the buffer and the destination_ip for when incase im sending a method to a 
    destination_ip.clear(); // new device

    
    char value;

    cout << "\n--- Reading Values from " << filename << " ---" << endl;
    
    // Read all integers from the file
    while (fscanf(file_ptr, "%c", &value) == 1) {
        IP_file_buffer.push_back(value); // dynamically expands
    
    }
    cout << endl;

    fclose(file_ptr);

    cout << "\nSuccess: " << IP_file_buffer.size() 
         << " values were read and printed from the file." << endl;

    cout << "\nThe message read:\n";

    // for loop helps you read the message that is being read for troubleshooting and debugging
    for(int i = 0; i < IP_file_buffer.size(); i++){
        cout << IP_file_buffer[i];
    }
    cout << endl;

    
    destination_ip.assign(IP_file_buffer.begin(), IP_file_buffer.end());  // the assign object places each char from the file_buffer that holds the
                                                                    // destination ip into the string destination_ip
    size_t last_char = destination_ip.find_last_not_of(" \t\n\r\f\v");  // this line asigns the last_char of destination_ip to be the one after it runs
    if (string :: npos != last_char){                                   // into one of these characters. also understand the first empty space is there so a blank space is also a notifier
        destination_ip.erase(last_char + 1);
    }               // this if statment is a checker to erase the character after the last

    return destination_ip;
 
}

int Connection :: read_encrypt_mssg(const string filename){
    FILE* file_ptr = fopen(filename.c_str(), "rb"); 
    if (file_ptr == NULL) {
        cerr << "Error: Cannot open file " << filename << " for reading." << endl;
        return -1;
    }

    mssg_file_buffer.clear();    // these lines clear up the buffer for when a new message is coming in

  
    
    fseek(file_ptr, 0, SEEK_END); // fseek moves the file pointer (file_ptr) to the end of the file so when
    long file_size = ftell(file_ptr); // ftell() returns the current position it will set file size to be the full length of file ptr in bytes
    
    
    rewind(file_ptr); // Reset the file pointer to the beginning for reading

    if (file_size < 0) {
        std::cerr << "Error determining file size for " << filename << std::endl;
        fclose(file_ptr);
        return -1; 
    }
    
    mssg_file_buffer.resize(file_size); //resize() is a method that sizes up the vector to fit the entire file content

    
    size_t bytes_read = fread(  // fread isnt just reading the values from the file from the source (file_ptr)
        mssg_file_buffer.data(), //  its also copying them into the destination (mssg_file_buffer.data()) and the buffer has 
        sizeof(char),            // the .data() behind it because the data function for vectors retuns a pointer/destination of
        file_size,        // the buffer in actual memory. Stating that each element in that buffer is one byte (sizeof(char)) and also understanding       
        file_ptr          // the full size of the file ptr using the file_size method variable           
    );
    
    //Check for success and clean up
    fclose(file_ptr);
    
    if (bytes_read != (size_t)file_size) {
             cerr << "Error: Only read " << bytes_read << " of " << file_size 
                  << " bytes from file " << filename << "." << std::endl;
        mssg_file_buffer.clear();
        return -1;
    }
    
    cout << "\n Successfully loaded encrypted message into the file buffer. Size: " 
              << file_size << " bytes." << std::endl;

    return 0; // Success
}

// the method above stores each value in the vector mssg_file_buffer as 1byte (8 bits) that represents an encrypted char




int Connection :: establish_connection(){
    if (destination_ip.empty()){
        cerr << "cant find destination IP to send to" << endl;
        return -1;
    }

    if (fd < 0){
        cerr << "problem with socket file descriptor (fd) you can check the port creation method to start" << endl;
        return -1;
    }

    sockaddr_in remote_address{};
    remote_address.sin_family = AF_INET; // sets the IP address that the remote address object take to be of IPV4
    remote_address.sin_port = htons(hardcoded_rec_port); // converts the hardcoded port number into network byte order for the kernel

    if (inet_pton(AF_INET, destination_ip.c_str(), &remote_address.sin_addr) <= 0){  
        // inet_pton is the method in subject. the method takes the first parameter that tells the kernel that the IP coming in is in type 
        // IPV4 and the second parameter is the class variable destination_ip wich has been converted into a readable c++ string
        // the thrid parameter is the pointer to the object structure we are creating called remote_address and we already set the IP address
        // type and the port for it now we are fixing up the destination IP for it and if it is less then or equal to 0 then the method failed
        
        cerr << "problem with the IP address " << destination_ip << " when it comes to formating for the remote address" << endl;
        close(fd);
        return -1;


    }


    // start of the three way handshake in wich the SYN and ACK is being set back and forth the devices
    if (connect(fd, (struct sockaddr*)&remote_address, sizeof(remote_address)) < 0){
        cerr << "Problem initiating the three way handshake" << endl;
        close(fd);
        return -1;
    }


    cout << "Three way handshake has been succesfull\n" << endl;
    cout << "Solid TCP connection has been set up\n" << endl;

    return 0;

}

int Connection :: send_message_packet(){
    if (mssg_file_buffer.empty()){
        cerr << "Cant send packet becuase mssg buffer is empy. Maybe problem in read enc message method" << endl;
        return -1;
    }
    
    size_t payload_size = mssg_file_buffer.size();
    
    uint32_t header_message = htonl(payload_size);
    size_t header_messg_len = sizeof(header_message); 
    
    ssize_t header_sent = send(fd, &header_message, header_messg_len, 0);
    if (header_sent < (ssize_t)header_messg_len){
        cerr << "cant send the initial header message holding size of message packet" << endl;
    }
        

    const char* buffer_ptr = mssg_file_buffer.data(); 
    // line above is in charge of returning a pointer to the location of the values in the mssg buffer starting from the front thanks to the data method
    // even if the the values in the buffer are wrttin in there as 1 byte in each index it was instantiated as a char so the type is considered on as well

    size_t bytes_to_send = mssg_file_buffer.size();
    // the size method returns the total size of the mssg file and becuase the buffer hholds bytes in each index it returns the total number in bytes
    // the type is size_t so when the size of the buffer is calculated and saved to bytes_to_send the type will automaticaly fit the size givin

    size_t total_size_sent = 0;
    // starts at 0 before the loop

    cout << "Starting Transmission of message packet. Total bytes to be sent: " << bytes_to_send << endl;

    while (total_size_sent < bytes_to_send){
        size_t remaining_bytes = bytes_to_send - total_size_sent;

        ssize_t sent = send(fd, buffer_ptr + total_size_sent, remaining_bytes, 0);
        // send can return a negative number so the unasigned types need to be able to return signed values so its ssize_t instead of just size_t
        // the paramaters put into the send() is fd for the communication channel being used 
        // buffer_ptr + total_size_sent is the pointer to the next byte being sent in the next index of the mssg buffer
        // remaining bytes is updated incase the kernel cant send the entire packet in one go wich is why we have the entire call inside a while loop

        if (sent < 0) {
            perror("Error during data transmission");
            return -1;
        }
        
        // If 0 bytes were sent, the connection was likely closed by the other side
        if (sent == 0) {
            std::cerr << "Warning: Remote host closed the connection gracefully." << std::endl;
            break; 
        }

        // Update the counter for the loop
        total_size_sent += sent;
    }

        // Final Check and Return
    if (total_size_sent == bytes_to_send) {
        std::cout << "\nMessage sent successfully. All " << total_size_sent << " bytes transmitted." << std::endl;
        return 0; // Success
    } else {
        // This is only reached if the loop broke due to sent == 0 and when it equals 0 most likly the reciving closed the program wich shut the connection down
        std::cerr << "\nWarning: Transmission ended prematurely. Sent " << total_size_sent << " of " << bytes_to_send << " bytes." << std::endl;
        return 0; // if this happens its not exactly a error in the program so ill return as 0
    }

}







