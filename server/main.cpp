#include "server.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <socket_path> <num_channels>" << std::endl;
        return 1;
    }

    return 0;
}