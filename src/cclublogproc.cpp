#include "cclublogproc.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

#include "cclub.h"
#include "eventstream.h"

#define SAVE_LINE(in, buf, iss)  \
    std::getline(in, line);     \
    iss.clear();                 \
    iss.str(line);               \

#define SAFE_READ_EX(in, ref, bad_line, err, check_eof)  \
    if (!(in >> ref) || (check_eof && !in.eof())) {      \
        err << bad_line;                                 \
        return false;                                    \
    }                                                    \

#define SAFE_READ(in, ref, bad_line, err) SAFE_READ_EX(in, ref, bad_line, err, true)

bool process_club_log(std::ifstream &in, std::ostream &out, std::ostream &err)
{
    std::string line;
    std::istringstream iss;

    //

    SAVE_LINE(in, line, iss);
    computer_club::table_identifier_t tables_count;
    SAFE_READ(iss, tables_count, line, err);

    SAVE_LINE(in, line, iss);
    timew opening{}, closing{};
    SAFE_READ_EX(iss, opening, line, err, false);
    SAFE_READ_EX(iss, closing, line, err, true);

    SAVE_LINE(in, line, iss);
    computer_club::price_t hourly_rate;
    SAFE_READ(iss, hourly_rate, line, err);

    //

    computer_club club(tables_count, hourly_rate, opening, closing);
    event_stream stream(club);

    while (!in.eof()) {
        SAVE_LINE(in, line, iss);
        if (line.empty()) {
            if (!in.eof()) {
                err << line << std::endl;
                return false;
            }
            break;
        }
        try {
            // can throw out of order
            if (!(iss >> stream) || !iss.eof()) {
                throw std::exception();
            }
        } catch (...) {
            err << line << std::endl;
            return false;
        }
    }
    //
    out << club.schedule.opening << '\n';
    //
    while (!stream.empty()) {
        out << stream.poll() << '\n';
    }
    //
    club.close();
    while (!stream.empty()) {
        out << stream.poll() << '\n';
    }
    out << club.schedule.closing << '\n';
    //
    const auto &tables = club.report_tables();
    const size_t size = tables.size();
    if (size > 0) {
        for (size_t i = 0; i < size - 1; i++) {
            out << tables[i]->first << ' '
                      << tables[i]->second.revenue << ' '
                      << tables[i]->second.busy_time << '\n';
            for (size_t j = tables[i]->first + 1; j < tables[i + 1]->first; j++) {
                out << j << " 0 00:00\n";
            }
        }
        out << tables.back()->first << ' '
                  << tables.back()->second.revenue << ' '
                  << tables.back()->second.busy_time << '\n';
        for (size_t j = tables.back()->first + 1; j < club.tables_count; j++) {
            out << j << " 0 00:00\n";
        }
    }

    return true;
}