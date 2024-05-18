#include "eventstream.h"
#include "events.h"

#include <cassert>

#define CHK_SPACE(in)                       \
    if (in.peek() != ' ') {                 \
        in.setstate(std::ios::failbit);     \
        return in;                          \
    }                                       \


event_stream::event_stream(computer_club &club)
    : club(club)
    , handler(*this)
{
    club.subscribe(&handler);
}

event_stream::~event_stream() {
    club.unsubscribe(&handler);
}

void event_stream::enqueue(const event::record &event) {
    events.emplace(event::record {
        .timestamp = event.timestamp,
        .direction = event.direction,
        .type = event.type,
        .data = std::unique_ptr<decltype(event.data)::element_type>(event.data->clone())
    });
}

void event_stream::post(event::record &&event) {
    club.submit(std::move(event));
}

event::record event_stream::poll() {
    auto event = std::move(events.front());
    events.pop();
    return event;
}

bool event_stream::empty() const noexcept {
    return events.empty();
}

#define EVENT_CASE(event_type, data_type)                     \
    case event_type: {                                        \
            data_type data;                                   \
            if (!(in >> data)) return in;                     \
            record.data = std::make_unique<data_type>(data);  \
            break;                                            \
    }

std::istream &operator>>(std::istream &in, event_stream &stream) {
    event::record record;

    if (!(in >> record.timestamp)) return in;
    CHK_SPACE(in);

    record.direction = event::direction::Inbound;

    event::identifier_t id;
    if (!(in >> id)) return in;
    CHK_SPACE(in);
    record.type.in = static_cast<event::inbound>(id);

    switch (record.type.in) {
        EVENT_CASE(event::inbound::ClientArrived, event::data::client_arrival);
        EVENT_CASE(event::inbound::ClientSettled, event::data::client_setting);
        EVENT_CASE(event::inbound::ClientWaiting, event::data::client_waiting);
        EVENT_CASE(event::inbound::ClientLeft,    event::data::client_leaving);
        default: {
            // assert(!"Unhandled inbound event type");
            in.setstate(std::ios::failbit);
            return in;
        }
    }

    stream.post(std::move(record));

    return in;
}

// event_stream::event_handler //

event_stream::event_handler::event_handler(event_stream &stream)
    : stream(stream)
{}

void event_stream::event_handler::accept(const event::record &subject) {
    stream.enqueue(subject);
}