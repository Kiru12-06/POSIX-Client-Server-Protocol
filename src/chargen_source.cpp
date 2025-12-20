#include "chargen.h"
#include <fstream>
#include <iostream>

CharGen::CharGen()
{
    top = -1;
}

bool CharGen::isEmpty()
{
    return top == -1;
}

int CharGen::push(char n)
{
    ++top;
    v.push_back(n);
    return 1;
}

// --- pop ---
char CharGen::pop()
{
    if (isEmpty()) {
        std::cerr << "Error: Stack is empty. Cannot pop." << std::endl;
        return '\0'; // Return null character on failure
    }

    char temp = v.back();

    v.pop_back();

    --top;

    return temp;
}

void CharGen::write_to_file(const std::string& filename)
{
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << " for writing." << std::endl;
        return;
    }

    for (int i = 0; i <= top; ++i) {
        file << v[i];
    }

    
    file.close();

    std::cout << "Successfully wrote " << (top + 1) << " characters to " << filename << std::endl;
}
