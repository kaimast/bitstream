#include <gtest/gtest.h>

#include "bitstream.h"

class BasicTest : public testing::Test
{
};

TEST(BasicTest, equality)
{
    bitstream bs1, bs2, bs3;
    bs1 << "foobar";
    bs2 << "foobaz";
    bs3 << "foobar";

    EXPECT_EQ(bs1, bs1);
    EXPECT_NE(bs1, bs2);
    EXPECT_EQ(bs1, bs3);
}


