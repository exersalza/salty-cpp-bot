//
// Created by julian on 11/18/22.
//
#include <iostream>

#define BUFFER_SIZE 512

int stoc(const std::string &src, char* dest) {
    for (int i = 0; i < src.length(); ++i) {
        dest[i] = src[i];
    }

    return 0;
}

int main() {
    std::string src = "some string";
    char dest[512];
    size_t c = 0;

    stoc(src, dest);

    std::cout << dest << std::endl;

    return 0;
}