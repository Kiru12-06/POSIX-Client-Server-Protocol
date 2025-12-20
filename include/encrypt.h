#ifndef ENC_HPP
#define ENC_HPP

#include <vector>
#include <string>

using namespace std;

class Char_Encrypt{
    private:
        vector<char> file_buffer;
        char xorKey = 'P';
    public:
        Char_Encrypt();
        void read_file(const string);
        void encryptDecrypt();
        void write_to_file(const string);

};


#endif 