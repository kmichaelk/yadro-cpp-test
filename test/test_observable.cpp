#include <gtest/gtest.h>

#include "observable.h"
#include "externobserver.h"

using Subject = const int;

template<typename Subject>
class exposing_observable : public observable<Subject> {
public:
    void _emit(Subject &subject) { this->emit(subject); }
};

TEST(observable, can_emit) {
    exposing_observable<Subject> observable;
    EXPECT_NO_THROW(observable._emit(1));
}

TEST(observable, can_subscribe) {
    exposing_observable<Subject> observable;
    external_observer<Subject> observer([](const auto& subject) {});
    EXPECT_NO_THROW(observable.subscribe(&observer));
}

TEST(observable, observer_can_accept) {
    constexpr Subject val = 42;
    bool called = false;

    exposing_observable<Subject> observable;
    external_observer<Subject> observer([val, &called](const auto& subject) {
        EXPECT_EQ(val, subject);
        called = true;
    });
    ASSERT_NO_THROW(observable.subscribe(&observer));
    ASSERT_NO_THROW(observable._emit(val));
    EXPECT_TRUE(called);
}