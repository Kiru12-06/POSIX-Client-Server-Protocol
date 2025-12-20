#ifndef CHARGEN_HPP
#define CHARGEN_HPP

#include <vector>
#include <string>
#include <iostream>


class CharGen {
private:
    std::vector<char> v;
    int top;

public:
    CharGen();
    int push(char n); 
    char pop();
    bool isEmpty();
    void write_to_file(const std::string& filename);
};

#endif // CHARGEN_HPP