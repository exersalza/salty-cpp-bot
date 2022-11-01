//
// Created by julian on 29.10.22.
//

#include <iostream>
#include <dpp/dpp.h>
#include "header/config.h"

using namespace std;
using namespace cfg;

int main() {
    string path = "../config.txt";
    Config config = Config();

    string token = config.getToken(path);
    cout << token;
}