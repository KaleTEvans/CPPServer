#include <iostream>

#include "server/http/https_server.h"

int main(void) {

    if (__cplusplus == 201703L) std::cout << "C++17\n";
    else if (__cplusplus == 201402L) std::cout << "C++14\n";
    else if (__cplusplus == 201103L) std::cout << "C++11\n";
    else std::cout << "Pre-C++11\n";
    return 0;
}