#include "connect.h"

using namespace std;



int main(){
    ConnectServer cnc_ser;

    if (cnc_ser.setup_listener() != 0){
        cerr << "Trouble setting up the listen_fd channel to set up the queue of devices who succesfully executed the 3 way handshake" << endl;
        return -1;
    }

    while (true){
    
        if(cnc_ser.accept_device() != 0){
            cerr << "cant pull a device of the listen_fd queue. Could be a problem with the setup listener" << endl;
            continue;
        }

        if (cnc_ser.recive_message() == 0) {
            cnc_ser.encrypt_decrypt();

            cnc_ser.display_message();

            cout << "/nRead message" << endl; 
        }

        
    }


    return 0;

}
