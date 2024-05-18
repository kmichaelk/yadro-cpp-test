#include "timew.h"
#include <sstream>
#include <regex>
#include <cstring>

bool timew::operator==(const timew &other) const noexcept { return minutes == other.minutes; }
bool timew::operator!=(const timew &other) const noexcept { return minutes != other.minutes; }

bool timew::operator< (const timew &other) const noexcept { return minutes <  other.minutes; }
bool timew::operator<=(const timew &other) const noexcept { return minutes <= other.minutes; }
bool timew::operator>=(const timew &other) const noexcept { return minutes >= other.minutes; }
bool timew::operator> (const timew &other) const noexcept { return minutes >  other.minutes; }

timew timew::operator-(const timew &rhs) const {
    return { static_cast<unsigned short>(minutes - rhs.minutes) };
}

timew &timew::operator+=(const timew &rhs) noexcept {
    minutes += rhs.minutes;
    return *this;
}

const std::regex time_regex(R"(^(0[0-9]|1[0-9]|2[0-3]):[0-5][0-9]$)");
std::istream &operator>>(std::istream &in, timew &t) {
    constexpr size_t len = 2 + 1 + 2;

    char buf[len + 1];
    if (!(in >> buf)) return in;

    if ((std::strlen(buf) != len) || !std::regex_match(buf, time_regex)) {
        in.setstate(std::ios::failbit);
        return in;
    }

    std::istringstream iss(buf);

    unsigned short hh, mm;
    //
    iss >> hh;
    iss.ignore(); // ':'
    iss >> mm;
    //
    t.minutes = (hh * 60) + mm;
    //

    in.peek();

    return in;
}

std::ostream &operator<<(std::ostream &out, const timew &t) {
    out << std::setw(2) << std::setfill('0') << (t.minutes / 60)
        << ':'
        << std::setw(2) << std::setfill('0') << (t.minutes % 60);
    return out;
}
