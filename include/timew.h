#ifndef TIMEW_H
#define TIMEW_H

#include <iostream>
#include <iomanip>

struct timew {
    unsigned short minutes;

    timew operator-(const timew &rhs) const;

    timew &operator+=(const timew &rhs) noexcept;

    bool operator==(const timew &other) const noexcept;
    bool operator!=(const timew &other) const noexcept;

    bool operator< (const timew &other) const noexcept;
    bool operator<=(const timew &other) const noexcept;
    bool operator>=(const timew &other) const noexcept;
    bool operator> (const timew &other) const noexcept;

    static const timew MIN_VALUE;
    static const timew MAX_VALUE;
};

inline const timew timew::MIN_VALUE = { 0 };
inline const timew timew::MAX_VALUE = { (24 * 60) - 1 };

std::istream &operator>>(std::istream &in, timew &t);
std::ostream &operator<<(std::ostream &out, const timew &t);

#endif // TIMEW_H
