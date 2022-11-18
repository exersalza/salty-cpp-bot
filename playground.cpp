//
// Created by julian on 11/18/22.
//
#include <iostream>

#define BUFFER_SIZE 512

int stoc(const std::string &src, char* dest) {
    for (auto i : src) {
        std::cout << i;
    }
    std::cout << std::endl;
    return 0;
}

int main() {
    char dest[BUFFER_SIZE];
    std::string src = "some string";

    stoc(src, dest);

    return 0;
}