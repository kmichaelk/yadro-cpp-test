#include <gtest/gtest.h>

#include "events.h"
#include <sstream>

TEST(validation, client_name) {
    EXPECT_EQ(true, validate_client_name("test"));

    EXPECT_EQ(true, validate_client_name("_test_"));
    EXPECT_EQ(true, validate_client_name("_te-st_"));
    EXPECT_EQ(true, validate_client_name("te-st"));

    EXPECT_EQ(true, validate_client_name("T3ST"));
    EXPECT_EQ(true, validate_client_name("test0"));
    EXPECT_EQ(true, validate_client_name("Test0"));



    EXPECT_EQ(false, validate_client_name("test "));
    EXPECT_EQ(false, validate_client_name("te st"));
    EXPECT_EQ(false, validate_client_name("te&st"));
}