#ifndef EVENT_STREAM_H
#define EVENT_STREAM_H

#include "cclub.h"

#include <iostream>
#include <queue>

class event_stream {
private:
    computer_club &club;

    struct event_handler : observer<const event::record> {
        event_stream &stream;

        explicit event_handler(event_stream &stream);

        void accept(const event::record &subject) override;
    } handler;
    friend event_handler;

    std::queue<event::record> events;

    void enqueue(const event::record &event);

public:
    explicit event_stream(computer_club &club);

    ~event_stream();

    event_stream(const event_stream &other) = delete;
    event_stream &operator=(const event_stream &other) = delete;

    void post(event::record &&event);
    event::record poll();

    [[nodiscard]] bool empty() const noexcept;
};

std::istream &operator>>(std::istream &in, event_stream &stream);

#endif // EVENT_STREAM_H
