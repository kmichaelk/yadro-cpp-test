#ifndef EVENTS_H
#define EVENTS_H

#include "event.h"
#include "cclub.h"
#include "procerr.h"

bool validate_client_name(const std::string& name);

#define VALIDATE_CLIENT_NAME(in, name)   \
    if (!validate_client_name(name)) {   \
        in.setstate(std::ios::failbit);  \
    }                                    \


namespace event::data {
    struct error : cloneable_data<error> {
        const processing_error type;

        explicit error(processing_error type)
            : type(type)
        {}

        void write(std::ostream &out) const override {
            switch (type) {
#define EXPAND_PROCERR(val)                      \
                case processing_error::val:  {   \
                    out << #val;                 \
                    break;                       \
                }
#include "xprocerr.def"
#undef EXPAND_PROCERR
            }

            // can't use exclamation mark in names //
            if (type == processing_error::ICanWaitNoLonger) {
                out << '!';
            }
            //                 :(                 //
        }
    };

    struct client_arrival : cloneable_data<client_arrival> {
        computer_club::client_identifier_t client;

        client_arrival() = default;
        explicit client_arrival(computer_club::client_identifier_t client)
            : client(std::move(client))
        {}

        void write(std::ostream &out) const override {
            out << client;
        }
        friend std::istream &operator>>(std::istream &in, client_arrival &data) {
            in >> data.client;
            VALIDATE_CLIENT_NAME(in, data.client);
            return in;
        }
    };
    struct client_setting : cloneable_data<client_setting> {
        computer_club::client_identifier_t client;
        computer_club::table_identifier_t table;

        client_setting() = default;
        client_setting(computer_club::client_identifier_t client, computer_club::table_identifier_t table)
            : client(std::move(client))
            , table(table)
        {}

        void write(std::ostream &out) const override {
            out << client << ' ' << table;
        }
        friend std::istream &operator>>(std::istream &in, client_setting &data) {
            in >> data.client >> data.table;
            VALIDATE_CLIENT_NAME(in, data.client);
            return in;
        }
    };
    struct client_waiting : cloneable_data<client_waiting> {
        computer_club::client_identifier_t client;

        client_waiting() = default;
        explicit client_waiting(computer_club::client_identifier_t client)
            : client(std::move(client))
        {}

        void write(std::ostream &out) const override {
            out << client;
        }
        friend std::istream &operator>>(std::istream &in, client_waiting &data) {
            in >> data.client;
            VALIDATE_CLIENT_NAME(in, data.client);
            return in;
        }
    };
    struct client_leaving : cloneable_data<client_leaving> {
        computer_club::client_identifier_t client;

        client_leaving() = default;
        explicit client_leaving(computer_club::client_identifier_t client)
            : client(std::move(client))
        {}

        void write(std::ostream &out) const override {
            out << client;
        }
        friend std::istream &operator>>(std::istream &in, client_leaving &data) {
            in >> data.client;
            VALIDATE_CLIENT_NAME(in, data.client);
            return in;
        }
    };
}

#endif // EVENTS_H
