#include <iostream>
#include <fstream>
#include <cstring>

#include "cclublogproc.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << *argv << " <file>" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream fin(argv[1]);
    if (fin.fail()) {
        std::cerr << "Failed to open file: " << std::strerror(errno) << " (code " << errno << ")" << std::endl;
        return EXIT_FAILURE;
    }

    return process_club_log(fin, std::cout, std::cerr) ? EXIT_SUCCESS : EXIT_FAILURE;
}
