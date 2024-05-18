#include <gtest/gtest.h>

#include "timew.h"
#include <sstream>

TEST(timew, can_parse_from_istream) {
    timew timestamp{};
    std::stringstream ss;

    std::stringstream().swap(ss);
    ss << "09:30";
    ss >> timestamp;
    EXPECT_FALSE(ss.fail());
    EXPECT_EQ((9 * 60) + 30, timestamp.minutes);

    std::stringstream().swap(ss);
    ss << "00:00";
    ss >> timestamp;
    EXPECT_FALSE(ss.fail());
    EXPECT_EQ(0, timestamp.minutes);

    std::stringstream().swap(ss);
    ss << "23:23";
    ss >> timestamp;
    EXPECT_FALSE(ss.fail());
    EXPECT_EQ((23 * 60) + 23, timestamp.minutes);
}

TEST(timew, parsing_fails_on_malformed_input) {
    timew timestamp{};
    std::stringstream ss;

    std::stringstream().swap(ss);
    ss << "";
    ss >> timestamp;
    EXPECT_TRUE(ss.fail());

    std::stringstream().swap(ss);
    ss << "1";
    ss >> timestamp;
    EXPECT_TRUE(ss.fail());

    std::stringstream().swap(ss);
    ss << "1:";
    ss >> timestamp;
    EXPECT_TRUE(ss.fail());

    std::stringstream().swap(ss);
    ss << ":1";
    ss >> timestamp;
    EXPECT_TRUE(ss.fail());

    std::stringstream().swap(ss);
    ss << "1:1";
    ss >> timestamp;
    EXPECT_TRUE(ss.fail());

    std::stringstream().swap(ss);
    ss << "01:1";
    ss >> timestamp;
    EXPECT_TRUE(ss.fail());

    std::stringstream().swap(ss);
    ss << "1:01";
    ss >> timestamp;
    EXPECT_TRUE(ss.fail());

    std::stringstream().swap(ss);
    ss << "24:01";
    ss >> timestamp;
    EXPECT_TRUE(ss.fail());

    std::stringstream().swap(ss);
    ss << "05:61";
    ss >> timestamp;
    EXPECT_TRUE(ss.fail());

    std::stringstream().swap(ss);
    ss << "-01:01";
    ss >> timestamp;
    EXPECT_TRUE(ss.fail());
}