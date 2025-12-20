#include "chargen.h"
#include <iostream>
#include <string>
#include <cstdio>

using namespace std;

int main() {
    CharGen stk;

    int choice;
    char CHAR;
    int temp;
    char prev;
    
    do {
            std ::cout << "inputing a 0 breaks out the program" << std :: endl;
            std ::cout << "inputing a 1 pushes your char into the stak" << std :: endl;
            //cout << "inputing a 2 pops the top character in the stack and displays the char being removed" << endl;
                                                        // (This case isnt needed in the program but Ill keep it inside the program)
            //cout << "inputing a 3  to display the string of character";
                                                        // (when displaying the string it will come out backwards because stack info
                                                        // is last in first out)
            
            std ::cout << "enter the choioce (0, or 1)" << endl;

            std ::cin >> choice;

            switch(choice){
                case 0:
                    break;
                
                case 1: {
                    prev = '\0';
                    std ::cout << "Enter the message you want to send (Type ## when once you finish your message) ";
                    
                    while (true){
                        std::cin.get(CHAR);
                        
                        if (prev == '#' && CHAR == '#'){
                            stk.pop();
                            std ::cout << "writing to message_L1.txt" << endl;
    
                            stk.write_to_file("/home/raspberry/CIS27_Final/comm_exchange/message_L1.txt");

   
                            system("echo /home/raspberry/CIS27_Final/comm_exchange/message_L1.txt > /home/raspberry/CIS27_Final/pipeline/ready.txt");

                            
                            break;
                            std ::cout << "message being sent" << endl;
                        }

                        prev = CHAR;
                        stk.push(CHAR);
                    }

                    break;
                }
                
                default:
                    std ::cout << "You didnt input a known number" << endl;
            }
    } while (choice != 0);

    //string file_to_write;

    //std ::cout << "writing to message_L1.txt" << endl;
    
    //stk.write_to_file("/home/raspberry/CIS27_Final/comm_exchange/message_L1.txt");

   
    //system("echo /home/raspberry/CIS27_Final/comm_exchange/message_L1.txt > /home/raspberry/CIS27_Final/pipeline/ready.txt");

    //cout << "Layer 1 output ready for pipeline." << endl;

    return 0;

}
