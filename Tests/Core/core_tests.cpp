#include <gtest/gtest.h>

#include "tech_demo.h"

TEST(CoreTest, BasicTest) {

    bool success = false;

    try
    {
        TechDemo::run_all();
        success = true;
    } catch (std::exception e)
    {
        std::cout << e.what() << '\n';
        success = false;
    }

    EXPECT_EQ(success, true);
}

