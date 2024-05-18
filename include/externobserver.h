#ifndef EXTERNAL_OBSERVER_H
#define EXTERNAL_OBSERVER_H

#include "observable.h"
#include <functional>

template<typename Subject>
class external_observer : public observer<Subject> {
private:
    std::function<void(Subject &)> listener;
public:
    explicit external_observer(decltype(listener) listener)
            : listener(std::move(listener))
    {}

    void accept(Subject &subject) override { listener(subject); }
};

#endif // EXTERNAL_OBSERVER_H
