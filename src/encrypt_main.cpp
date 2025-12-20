#include "encrypt.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>


using namespace std;


int main(){
    Char_Encrypt enc;


    
    enc.read_file("/home/raspberry/CIS27_Final/layer2/input/message_L1.txt");

    enc.encryptDecrypt();
    

    cout<< "\nwriting to encrypt_mssg.txt\n" << endl;
    
    enc.write_to_file("/home/raspberry/CIS27_Final/layer3/input/enc_message.txt");

}
