#include <unity.h>

#include "lora/Deduplicator.hpp"

using lora::Deduplicator;
using lora::Packet;

TEST_CASE("Deduplicator works", "[lora]")
{
    Deduplicator<2> deduplicator;

    Packet p1;
    p1.src = 1;
    p1.seq = 2;
    TEST_ASSERT_TRUE(deduplicator.receive(&p1));

    Packet p2;
    p2.src = 1;
    p2.seq = 3;
    TEST_ASSERT_TRUE(deduplicator.receive(&p2));

    Packet p3;
    p3.src = 1;
    p3.seq = 2;
    TEST_ASSERT_FALSE(deduplicator.receive(&p3));
}

TEST_CASE("Deduplicator rotates", "[lora]")
{
    Deduplicator<2> deduplicator;

    Packet p1;
    p1.src = 1;
    p1.seq = 2;
    TEST_ASSERT_TRUE(deduplicator.receive(&p1));

    Packet p2;
    p2.src = 1;
    p2.seq = 3;
    TEST_ASSERT_TRUE(deduplicator.receive(&p2));

    Packet p3;
    p3.src = 1;
    p3.seq = 4;
    TEST_ASSERT_TRUE(deduplicator.receive(&p3));

    Packet p4;
    p4.src = 1;
    p4.seq = 3;
    TEST_ASSERT_FALSE(deduplicator.receive(&p4));

    Packet p5;
    p5.src = 1;
    p5.seq = 4;
    TEST_ASSERT_FALSE(deduplicator.receive(&p5));
}
