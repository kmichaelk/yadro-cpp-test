#include <gtest/gtest.h>

#include "cclub.h"
#include "events.h"

TEST(cclub, rejects_out_of_order_event) {
    const auto client = "client1";

    computer_club club(10, 10, { 9 * 60 }, { 21 * 60 });

    ASSERT_NO_THROW(club.submit({
        .timestamp = { 12 * 60 },
        .direction = event::direction::Inbound,
        .type = { .in = event::inbound::ClientArrived },
        .data = std::make_unique<event::data::client_arrival>(client)
    }));
    EXPECT_ANY_THROW(club.submit({
        .timestamp = { 10 * 60 },
        .direction = event::direction::Inbound,
        .type = { .in = event::inbound::ClientArrived },
        .data = std::make_unique<event::data::client_arrival>(client)
    }));
}

TEST(cclub, can_check_client_presence) {
    const auto client1 = "client1";
    const auto client2 = "client2";

    computer_club club(10, 10, { 9 * 60 }, { 21 * 60 });

    EXPECT_EQ(false, club.is_client_present(client1));
    EXPECT_EQ(false, club.is_client_present(client2));

    ASSERT_NO_THROW(club.submit({
        .timestamp = { 12 * 60 },
        .direction = event::direction::Inbound,
        .type = { .in = event::inbound::ClientArrived },
        .data = std::make_unique<event::data::client_arrival>(client1)
    }));

    EXPECT_EQ(true, club.is_client_present(client1));
    EXPECT_EQ(false, club.is_client_present(client2));

    ASSERT_NO_THROW(club.submit({
        .timestamp = { 13 * 60 },
        .direction = event::direction::Inbound,
        .type = { .in = event::inbound::ClientLeft },
        .data = std::make_unique<event::data::client_leaving>(client1)
    }));

    EXPECT_EQ(false, club.is_client_present(client1));
    EXPECT_EQ(false, club.is_client_present(client2));
}

TEST(cclub, can_check_if_table_is_occupied) {
    const auto client = "client1";
    const auto table = 1;

    computer_club club(10, 10, { 9 * 60 }, { 21 * 60 });

    EXPECT_EQ(false, club.is_table_occupied(table));

    ASSERT_NO_THROW(club.submit({
        .timestamp = { 12 * 60 },
        .direction = event::direction::Inbound,
        .type = { .in = event::inbound::ClientArrived },
        .data = std::make_unique<event::data::client_arrival>(client)
    }));
    ASSERT_EQ(true, club.is_client_present(client));

    ASSERT_NO_THROW(club.submit({
        .timestamp = { 13 * 60 },
        .direction = event::direction::Inbound,
        .type = { .in = event::inbound::ClientSettled },
        .data = std::make_unique<event::data::client_setting>(client, table)
    }));
    EXPECT_EQ(true, club.is_table_occupied(table));


    ASSERT_NO_THROW(club.submit({
        .timestamp = { 14 * 60 },
        .direction = event::direction::Inbound,
        .type = { .in = event::inbound::ClientLeft },
        .data = std::make_unique<event::data::client_leaving>(client)
    }));
    ASSERT_EQ(false, club.is_client_present(client));

    EXPECT_EQ(false, club.is_table_occupied(table));
}

TEST(cclub, client_leaving_frees_table) {
    const auto client1 = "client1";
    const auto table = 1;

    computer_club club(1, 10, { 9 * 60 }, { 21 * 60 });

    ASSERT_EQ(false, club.is_client_present(client1));
    ASSERT_EQ(false, club.is_table_occupied(table));

    ASSERT_NO_THROW(club.submit({
        .timestamp = { 12 * 60 },
        .direction = event::direction::Inbound,
        .type = { .in = event::inbound::ClientArrived },
        .data = std::make_unique<event::data::client_arrival>(client1)
    }));
    ASSERT_EQ(true, club.is_client_present(client1));
    ASSERT_EQ(false, club.is_table_occupied(table));

    ASSERT_NO_THROW(club.submit({
        .timestamp = { 13 * 60 },
        .direction = event::direction::Inbound,
        .type = { .in = event::inbound::ClientSettled },
        .data = std::make_unique<event::data::client_setting>(client1, table)
    }));
    ASSERT_EQ(true, club.is_client_present(client1));
    ASSERT_EQ(true, club.is_table_occupied(table));

    ASSERT_NO_THROW(club.submit({
        .timestamp = { 15 * 60 },
        .direction = event::direction::Inbound,
        .type = { .in = event::inbound::ClientLeft },
        .data = std::make_unique<event::data::client_leaving>(client1)
    }));
    ASSERT_EQ(false, club.is_client_present(client1));
    EXPECT_EQ(false, club.is_table_occupied(table));
}

TEST(cclub, clients_queue_moving_on_leave) {
    const auto client1 = "client1";
    const auto client2 = "client2";
    const auto table = 1;

    computer_club club(1, 10, { 9 * 60 }, { 21 * 60 });

    ASSERT_EQ(false, club.is_client_present(client1));
    ASSERT_EQ(false, club.is_client_present(client2));
    ASSERT_EQ(false, club.is_table_occupied(table));

    ASSERT_NO_THROW(club.submit({
        .timestamp = { 12 * 60 },
        .direction = event::direction::Inbound,
        .type = { .in = event::inbound::ClientArrived },
        .data = std::make_unique<event::data::client_arrival>(client1)
    }));
    ASSERT_EQ(true, club.is_client_present(client1));
    ASSERT_EQ(false, club.is_client_present(client2));
    ASSERT_EQ(false, club.is_table_occupied(table));

    ASSERT_NO_THROW(club.submit({
        .timestamp = { 13 * 60 },
        .direction = event::direction::Inbound,
        .type = { .in = event::inbound::ClientSettled },
        .data = std::make_unique<event::data::client_setting>(client1, table)
    }));
    ASSERT_EQ(true, club.is_client_present(client1));
    ASSERT_EQ(false, club.is_client_present(client2));
    ASSERT_EQ(true, club.is_table_occupied(table));

    ASSERT_NO_THROW(club.submit({
        .timestamp = { 14 * 60 },
        .direction = event::direction::Inbound,
        .type = { .in = event::inbound::ClientArrived },
        .data = std::make_unique<event::data::client_arrival>(client2)
    }));
    ASSERT_EQ(true, club.is_client_present(client1));
    ASSERT_EQ(true, club.is_client_present(client2));
    ASSERT_EQ(true, club.is_table_occupied(table));

    ASSERT_NO_THROW(club.submit({
        .timestamp = { 15 * 60 },
        .direction = event::direction::Inbound,
        .type = { .in = event::inbound::ClientWaiting },
        .data = std::make_unique<event::data::client_waiting>(client2)
    }));
    ASSERT_EQ(true, club.is_client_present(client1));
    ASSERT_EQ(true, club.is_client_present(client2));
    ASSERT_EQ(true, club.is_table_occupied(table));

    ASSERT_NO_THROW(club.submit({
        .timestamp = { 15 * 60 },
        .direction = event::direction::Inbound,
        .type = { .in = event::inbound::ClientLeft },
        .data = std::make_unique<event::data::client_leaving>(client1)
    }));
    ASSERT_EQ(false, club.is_client_present(client1));
    ASSERT_EQ(true, club.is_client_present(client2));
    EXPECT_EQ(true, club.is_table_occupied(table));
}