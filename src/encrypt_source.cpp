#include "encrypt.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

Char_Encrypt :: Char_Encrypt(){}




void Char_Encrypt :: read_file(const string filename) {
    FILE* file_ptr = fopen(filename.c_str(), "rb");  // reading the file in binary mode in case one of the chars sent isnt a pute txt
    
    if (file_ptr == NULL) {
        cerr << "Error: Cannot open file " << filename << " for reading." << endl;
        return;
    }

    
    char value;

    cout << "\n--- Reading Values from " << filename << " ---" << endl;
    
    // Read all integers from the file
    while (fscanf(file_ptr, "%c", &value) == 1) {
        file_buffer.push_back(value); // dynamically expands
    
    }
    cout << endl;

    fclose(file_ptr);

    cout << "\nSuccess: " << file_buffer.size() 
         << " values were read and printed from the file." << endl;

    cout << "\nThe message read:\n";

    // for loop helps you read the message that is being read for troubleshooting and debugging
    for(int i = 0; i < file_buffer.size(); i++){
        cout << file_buffer[i];
    }
    cout << endl;
 
}

void Char_Encrypt :: encryptDecrypt()
{
    // perform XOR operation of key
    // with every character in string
    for (int i = 0; i < file_buffer.size(); i++)
    {
        file_buffer[i] = file_buffer[i] ^ xorKey;
        
    }
}

void Char_Encrypt::write_to_file(const string to_filename){
    FILE* file_ptr = fopen(to_filename.c_str(), "wb"); // instead of opening the file_ptr struct in write mode im opening it in write binary
                                            // so that incase my encyption turns a char into a non printable asci char then it will    
                                        // write it in binary so that it dosnt mess with the encrypted file
    if (file_ptr == NULL){
                cerr << "Error: Cannot open file " << to_filename << " for writing." << endl;
                return;
    }

    for (int i = 0; i < file_buffer.size(); i++){
                // Write the char value
                fprintf(file_ptr, "%c", file_buffer[i]);    
            }
        
    fclose(file_ptr);
}
