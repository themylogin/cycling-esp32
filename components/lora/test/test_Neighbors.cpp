#include <unity.h>

#include "lora/Neighbors.hpp"

using lora::Neighbors;

TEST_CASE("every_neighbor_can_hear: 1-2-3-4, 1 and 4 can't hear, we are 2", "[lora]")
{
    Neighbors n(2);

    n.update_neighbor(1, 0, 0);
    n.update_neighbor_neighbors(1, { 2, 3 });

    n.update_neighbor(2, 0, 0);
    n.update_neighbor_neighbors(3, { 1, 3, 4 });

    n.update_neighbor(4, 0, 0);
    n.update_neighbor_neighbors(4, { 2, 3 });

    TEST_ASSERT_FALSE(n.every_neighbor_can_hear(1, 0));
    TEST_ASSERT_FALSE(n.every_neighbor_can_hear(4, 0));
}

TEST_CASE("every_neighbor_can_hear: 1-2-3-4, 1 and 4 can't hear, we are 3", "[lora]")
{
    Neighbors n(3);

    n.update_neighbor(1, 0, 0);
    n.update_neighbor_neighbors(1, { 2, 3 });

    n.update_neighbor(2, 0, 0);
    n.update_neighbor_neighbors(2, { 1, 3, 4 });

    n.update_neighbor(4, 0, 0);
    n.update_neighbor_neighbors(4, { 2, 3 });

    TEST_ASSERT_TRUE(n.every_neighbor_can_hear(1, 0));
    TEST_ASSERT_TRUE(n.every_neighbor_can_hear(2, 0));
}

/*
 * 1     6 9
 * 2 4 5 7
 * 3     8
 */

TEST_CASE("every_neighbor_can_hear: 1-2-3-4, complicated case 1, we are 4", "[lora]")
{
    Neighbors n(4);

    n.update_neighbor(1, 0, 0);
    n.update_neighbor_neighbors(1, { 2, 3, 4 });

    n.update_neighbor(2, 0, 0);
    n.update_neighbor_neighbors(2, { 1, 3, 4 });

    n.update_neighbor(3, 0, 0);
    n.update_neighbor_neighbors(4, { 1, 2, 4 });

    n.update_neighbor(5, 0, 0);
    n.update_neighbor_neighbors(4, { 4, 6, 7, 8 });

    TEST_ASSERT_FALSE(n.every_neighbor_can_hear(1, 0));
    TEST_ASSERT_FALSE(n.every_neighbor_can_hear(2, 0));
    TEST_ASSERT_FALSE(n.every_neighbor_can_hear(3, 0));
    TEST_ASSERT_FALSE(n.every_neighbor_can_hear(5, 0));
    TEST_ASSERT_FALSE(n.every_neighbor_can_hear(6, 0));
    TEST_ASSERT_FALSE(n.every_neighbor_can_hear(7, 0));
    TEST_ASSERT_FALSE(n.every_neighbor_can_hear(8, 0));
    TEST_ASSERT_FALSE(n.every_neighbor_can_hear(9, 0));
}

TEST_CASE("every_neighbor_can_hear: 1-2-3-4, complicated case 1, we are 9", "[lora]")
{
    Neighbors n(9);

    n.update_neighbor(6, 0, 0);
    n.update_neighbor_neighbors(6, { 5, 7, 8, 9 });

    n.update_neighbor(7, 0, 0);
    n.update_neighbor_neighbors(7, { 5, 6, 8, 9 });

    n.update_neighbor(8, 0, 0);
    n.update_neighbor_neighbors(8, { 5, 6, 7 });

    TEST_ASSERT_FALSE(n.every_neighbor_can_hear(1, 0));
    TEST_ASSERT_FALSE(n.every_neighbor_can_hear(2, 0));
    TEST_ASSERT_FALSE(n.every_neighbor_can_hear(3, 0));
    TEST_ASSERT_FALSE(n.every_neighbor_can_hear(4, 0));
    TEST_ASSERT_TRUE(n.every_neighbor_can_hear(5, 0));
    TEST_ASSERT_TRUE(n.every_neighbor_can_hear(6, 0));
    TEST_ASSERT_TRUE(n.every_neighbor_can_hear(7, 0));
    TEST_ASSERT_TRUE(n.every_neighbor_can_hear(8, 0));
}
