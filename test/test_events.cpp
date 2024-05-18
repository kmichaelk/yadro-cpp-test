#include <gtest/gtest.h>

#include "cclub.h"
#include "events.h"
#include "externobserver.h"

TEST(cclub_events, rejects_arrival_outside_working_hours) {
    computer_club club(10, 10, { 9 * 60 }, { 21 * 60 });

    size_t handled_events = 0, outbound_events = 0;
    external_observer<computer_club::subject_type> observer([&handled_events, &outbound_events](auto &event) {
        ++handled_events;
        if (event.direction == event::direction::Inbound) return;

        ++outbound_events;
        EXPECT_EQ(event::outbound::Error, event.type.out);
    });

    club.subscribe(&observer);

    club.submit({
        .timestamp = { 8 * 60 },
        .direction = event::direction::Inbound,
        .type = { .in = event::inbound::ClientArrived },
        .data = std::make_unique<event::data::client_arrival>("client1")
    });
    club.submit({
        .timestamp = { 10 * 60 },
        .direction = event::direction::Inbound,
        .type = { .in = event::inbound::ClientArrived },
        .data = std::make_unique<event::data::client_arrival>("client2")
    });
    club.submit({
        .timestamp = { 22 * 60 },
        .direction = event::direction::Inbound,
        .type = { .in = event::inbound::ClientArrived },
        .data = std::make_unique<event::data::client_arrival>("client3")
    });

    EXPECT_EQ(5, handled_events);
    EXPECT_EQ(2, outbound_events);
}