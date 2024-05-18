#include "cclub.h"
#include "events.h"

#include <algorithm>
#include <vector>
#include <stdexcept>
#include <cmath>

computer_club::computer_club(unsigned int tables_count, unsigned int hourly_rate, timew opening, timew closing)
    : tables_count(tables_count)
    , hourly_rate(hourly_rate)
    , schedule({ .opening = opening, .closing = closing })
    , free_tables(tables_count)
{}

void computer_club::emit_error(const timew &timestamp, processing_error error) {
    emit({
        .timestamp = timestamp,
        .direction = event::direction::Outbound,
        .type = { .out = event::outbound::Error },
        .data = std::make_unique<event::data::error>(error)
    });
}

void computer_club::occupy_table(const computer_club::table_identifier_t &id, const client_identifier_t &by, const timew &at) {
    auto &table = tables[id];
    table.occupied_at = at;

    clients.at(by).table = id;

    --free_tables;
}

void computer_club::free_table(const computer_club::table_identifier_t &id, const timew &at) {
    auto &table = tables[id];

    auto diff = at - table.occupied_at.value();

    table.busy_time += diff;

    constexpr auto minutes_in_hour = 60;
    // table.revenue += std::ceil(diff.minutes / minutes_in_hour) * hourly_rate;
    // should be faster:
    table.revenue += ((diff.minutes + minutes_in_hour - 1) / minutes_in_hour) * hourly_rate;

    table.occupied_at.reset();

    ++free_tables;
}

void computer_club::add_client(client_identifier_t &&client, const timew &at) {
    clients[std::move(client)] = {
        .waiting = false,
        .table = std::nullopt
    };
}

void computer_club::pop_client(const computer_club::client_identifier_t &client, const timew &at) {
    const auto record = clients.find(client);
    auto& sec = record->second;
    if (record->second.table.has_value()) {
        free_table(record->second.table.value(), at);
    }
    clients.erase(record);
}

#define GET_EVENT_DATA_AS(event, type) *dynamic_cast<type*>(event.data.get()) // it won't take ownership

void computer_club::submit(event::record &&event) {
    if (last_event_timestamp > event.timestamp) {
        throw std::runtime_error("event is out of order");
    }
    last_event_timestamp = event.timestamp;

    emit(event);

    switch (event.type.in) {
        case event::inbound::ClientArrived: {
            auto &data = GET_EVENT_DATA_AS(event, event::data::client_arrival);
            if (event.timestamp < schedule.opening || event.timestamp >= schedule.closing) {
                emit_error(event.timestamp, processing_error::NotOpenYet);
                return;
            }
            if (is_client_present(data.client)) {
                emit_error(event.timestamp, processing_error::YouShallNotPass);
                return;
            }
            add_client(std::move(data.client), event.timestamp);
            break;
        }
        case event::inbound::ClientSettled: {
            auto &data = GET_EVENT_DATA_AS(event, event::data::client_setting);
            if (!(data.table >= 1 && data.table <= tables_count)) {
                throw std::runtime_error("table not found");
            }
            if (!is_client_present(data.client)) {
                emit_error(event.timestamp, processing_error::ClientUnknown);
                return;
            }
            if (is_table_occupied(data.table)) {
                emit_error(event.timestamp, processing_error::PlaceIsBusy);
                return;
            }
            occupy_table(data.table, data.client, event.timestamp);
            break;
        }
        case event::inbound::ClientWaiting: {
            auto &data = GET_EVENT_DATA_AS(event, event::data::client_waiting);
            if (free_tables > 0) {
                emit_error(event.timestamp, processing_error::ICanWaitNoLonger);
                return;
            }
            if (pending_clients.size() > tables_count) {
                pop_client(data.client, event.timestamp);
                emit({
                    .timestamp = event.timestamp,
                    .direction = event::direction::Outbound,
                    .type = { .out = event::outbound::ClientLeft },
                    .data = std::make_unique<event::data::client_leaving>(data.client)
                });
                return;
            }

            auto& client_info = clients[data.client];
            if (!client_info.waiting) {
                // "If a rehash happens, all iterators are invalidated,
                // but references and pointers to individual elements remain valid."
                //  -- https://cplusplus.com/reference/unordered_map/unordered_map/rehash/
                pending_clients.emplace_back(&clients.find(data.client)->first);
                client_info.waiting = true;
            }
            break;
        }
        case event::inbound::ClientLeft: {
            auto &data = GET_EVENT_DATA_AS(event, event::data::client_leaving);
            if (!is_client_present(data.client)) {
                emit_error(event.timestamp, processing_error::ClientUnknown);
                return;
            }

            const auto table = get_client_table(data.client);
            if (!table.has_value()) {
                const auto clients_it = clients.find(data.client);
                pending_clients.erase(
                        std::remove(pending_clients.begin(), pending_clients.end(), &clients_it->first),
                        pending_clients.end());
            }
            pop_client(data.client, event.timestamp);

            if (!pending_clients.empty() && table.has_value()) {
                const auto table_id = table.value();

                const auto successor = *pending_clients.front();
                pending_clients.pop_front();

                clients[successor].waiting = false;
                occupy_table(table_id, successor, event.timestamp);
                emit({
                    .timestamp = event.timestamp,
                    .direction = event::direction::Outbound,
                    .type = { .out = event::outbound::ClientSettled },
                    .data = std::make_unique<event::data::client_setting>(successor, table_id)
                });
            }
            break;
        }
        default: {
            throw std::runtime_error("unknown event");
        }
    }
}

inline bool computer_club::is_table_occupied(const computer_club::table_identifier_t &id) const noexcept {
    return tables.contains(id) && tables.at(id).occupied_at.has_value();
}

inline bool computer_club::is_client_present(const computer_club::client_identifier_t &client) const noexcept {
    return clients.contains(client);
}

const std::optional<computer_club::table_identifier_t> &
computer_club::get_client_table(const computer_club::client_identifier_t &client) const noexcept {
    return clients.at(client).table;
}

void computer_club::close() {
    const auto timestamp = schedule.closing;
    last_event_timestamp = timestamp;

    decltype(pending_clients)().swap(pending_clients);

    std::vector<const decltype(clients)::value_type*> sorted_clients;
    sorted_clients.reserve(clients.size());
    //
    std::transform(clients.begin(), clients.end(), std::back_inserter(sorted_clients), [](const auto &pair) {
        return &pair;
    });
    std::sort(sorted_clients.begin(), sorted_clients.end(), [](const auto &lhs, const auto &rhs) {
        return lhs->first < rhs->first;
    });
    //
    for (const auto &client_record: sorted_clients) {
        if (client_record->second.table.has_value()) {
            free_table(client_record->second.table.value(), timestamp);
        }
        emit({
            .timestamp = timestamp,
            .direction = event::direction::Outbound,
            .type = { .out = event::outbound::ClientLeft },
            .data = std::make_unique<event::data::client_leaving>(client_record->first)
        });
    }
    //
    decltype(clients)().swap(clients);
}

std::vector<const decltype(computer_club::tables)::value_type*> computer_club::report_tables() const {
    std::vector<const decltype(tables)::value_type*> sorted_tables(tables.size());

    std::transform(tables.begin(), tables.end(), sorted_tables.begin(), [](const auto &pair) {
        return &pair;
    });
    std::sort(sorted_tables.begin(), sorted_tables.end(), [](const auto &lhs, const auto &rhs) {
        return lhs->first < rhs->first;
    });

    return sorted_tables;
}
