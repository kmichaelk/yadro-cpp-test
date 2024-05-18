#ifndef EVENT_H
#define EVENT_H

#include <iostream>
#include <string>
#include <memory>

#include "timew.h"

namespace event {
    using identifier_t = unsigned int;

    enum class direction : unsigned char {
        Inbound,
        Outbound,
    };

    enum class inbound : identifier_t;
    enum class outbound : identifier_t;

    namespace data {
        struct base_data;
        template<typename derived_t> struct cloneable_data;
    }

    struct record;
}

enum class event::inbound : event::identifier_t {
    ClientArrived = 1,
    ClientSettled,
    ClientWaiting,
    ClientLeft,
};

enum class event::outbound : event::identifier_t {
    ClientLeft = 11,
    ClientSettled,
    Error,
};

//

struct event::data::base_data {
    virtual ~base_data() = default;

    virtual void write(std::ostream &out) const = 0;
    [[nodiscard]] virtual base_data *clone() const = 0;

    friend std::ostream &operator<<(std::ostream &out, const base_data &data) {
        data.write(out);
        return out;
    }
};

template<typename derived_t>
struct event::data::cloneable_data : base_data {
    [[nodiscard]] base_data *clone() const override {
        return new derived_t(static_cast<derived_t const&>(*this));
    }
};

//

struct event::record {
    timew timestamp;
    direction direction;
    union {
        inbound in;
        outbound out;
    } type;
    std::unique_ptr<data::base_data> data;

    friend std::ostream &operator<<(std::ostream &out, const record &rec) {
        out << rec.timestamp << ' ';
        if (rec.direction == direction::Inbound) {
            out << static_cast<typename std::underlying_type<inbound>::type>(rec.type.in);
        } else {
            out << static_cast<typename std::underlying_type<outbound>::type>(rec.type.out);
        }
        if (rec.data) {
            out << ' ' << *rec.data;
        }
        return out;
    }
};

#endif // EVENT_H
