#include <iostream>
#include <fstream>
#include <cstring>

#include "cclub.h"
#include "eventstream.h"

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

    //

    computer_club::table_identifier_t tables_count;
    if (!(fin >> tables_count)) return EXIT_FAILURE;

    timew opening{}, closing{};
    if (!(fin >> opening >> closing)) return EXIT_FAILURE;

    computer_club::price_t hourly_rate;
    if (!(fin >> hourly_rate)) return EXIT_FAILURE;

    //

    computer_club club(tables_count, hourly_rate, opening, closing);
    event_stream stream(club);

    std::cout << club.schedule.opening << '\n';
    //
    while (!fin.eof()) {
        if (!(fin >> stream)) {
            return EXIT_FAILURE;
        }
        while (!stream.empty()) {
            std::cout << stream.poll() << '\n';
        }
    }
    //
    club.close();
    while (!stream.empty()) {
        std::cout << stream.poll() << '\n';
    }
    std::cout << club.schedule.closing << '\n';
    //
    const auto &tables = club.report_tables();
    const size_t size = tables.size();
    if (size > 0) {
        for (size_t i = 0; i < size - 1; i++) {
            std::cout << tables[i]->first << ' '
                      << tables[i]->second.revenue << ' '
                      << tables[i]->second.busy_time << '\n';
            for (size_t j = tables[i]->first + 1; j < tables[i + 1]->first; j++) {
                std::cout << j << " 0 00:00\n";
            }
        }
        std::cout << tables.back()->first << ' '
                  << tables.back()->second.revenue << ' '
                  << tables.back()->second.busy_time << '\n';
        for (size_t j = tables.back()->first + 1; j < club.tables_count; j++) {
            std::cout << j << " 0 00:00\n";
        }
    }

    return 0;
}
