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

TEST(BasicTest, duplicate)
{
    bitstream bs1;
    size_t v1 = 42;
    size_t v2 = 23;

    bs1 << v1;
    auto pos = bs1.pos();
    bs1 << v2;
    bs1.move_to(pos);

    auto bs2 = bs1.duplicate(true);
    auto bs3 = bs1.duplicate();

    size_t res1, res2;
    bs2 >> res1;
    bs3 >> res2;

    EXPECT_EQ(res1, v2);
    EXPECT_EQ(res2, v2);
}

TEST(BasicTest, view)
{
    bitstream bs1;
    size_t v1 = 42;
    size_t v2 = 23;

    bs1 << v1;
    bs1 << v2;

    auto bs2 = bs1.make_view();

    size_t res1, res2;
    bs2 >> res1 >> res2;

    EXPECT_EQ(res1, v1);
    EXPECT_EQ(res2, v2);
}

TEST(BasicTest, read_only)
{
    bitstream bs1, bs2;

    size_t val = 42;
    bs1 << val;

    bs2.assign(bs1.data(), bs1.size(), true);

    EXPECT_FALSE(bs1.is_read_only());
    EXPECT_TRUE(bs2.is_read_only());
}
