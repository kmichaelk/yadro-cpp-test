#ifndef CLUB_H
#define CLUB_H

#include <string>
#include <optional>
#include <unordered_map>
#include <vector>
#include <deque>
#include <set>

#include "timew.h"
#include "event.h"
#include "procerr.h"
#include "observable.h"

class computer_club : public observable<const event::record> {
public:
    using table_identifier_t = unsigned int;
    using client_identifier_t = std::string;
    using price_t = unsigned int;

    const table_identifier_t tables_count;
    const price_t hourly_rate;
    const struct {
        timew opening;
        timew closing;
    } schedule;

    struct table_report_row {
        price_t revenue;
        timew busy_time;
    };

private:
    struct table_info {
        price_t revenue = 0;
        timew busy_time = { 0 };
        std::optional<timew> occupied_at = std::nullopt;
    };
    struct client_info {
        bool waiting = false;
        std::optional<table_identifier_t> table = std::nullopt;
    };
    table_identifier_t free_tables;

    std::unordered_map<table_identifier_t, table_info> tables;
    std::unordered_map<client_identifier_t, client_info> clients;
    std::deque<const client_identifier_t*> pending_clients;

    timew last_event_timestamp = timew::MIN_VALUE;

    void emit_error(const timew &timestamp, processing_error error);

    void occupy_table(const table_identifier_t &id, const client_identifier_t &by, const timew &at);
    void free_table(const table_identifier_t &id, const timew &at);

    void add_client(client_identifier_t &&client, const timew &at);
    void pop_client(const client_identifier_t &client, const timew &at);

public:
    computer_club(table_identifier_t tables_count, price_t hourly_rate, timew opening, timew closing);

    ~computer_club() override = default;

    bool is_table_occupied(const table_identifier_t &id) const noexcept;
    bool is_client_present(const client_identifier_t &client) const noexcept;
    const std::optional<table_identifier_t> &get_client_table(const client_identifier_t &client) const noexcept;

    void submit(event::record &&event);

    std::vector<const decltype(tables)::value_type*> report_tables() const;

    void close();
};

#endif // CLUB_H
