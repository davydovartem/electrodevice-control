#include "server.hpp"
#include <iostream>
#include <filesystem>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <socket_path> <num_channels>" << std::endl;
        return 1;
    }

    try
    {
        int num_channels = std::stoi(argv[2]);
        if (num_channels < 2)
        {
            throw std::invalid_argument("Number of channels must be >= 2");
        }

        if (std::filesystem::exists(argv[1]))
        {
            std::cout << "Socket file existed. Deleting..." << std::endl;
            unlink(argv[1]);
        }

        Server server(argv[1], num_channels);
        server.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}