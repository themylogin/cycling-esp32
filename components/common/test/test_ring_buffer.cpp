#include <unity.h>

#include "ring_buffer.hpp"

TEST_CASE("Empty ring buffer read", "[common]")
{
    ring_buffer<2, 4> rb;

    size_t length = 4;

    uint8_t dst1[4];
    TEST_ASSERT_FALSE(rb.read(dst1, &length));
}

TEST_CASE("Ring buffer single write multiple times", "[common]")
{
    ring_buffer<2, 4> rb;

    size_t length = 4;

    for (int i = 0; i < 3; i++)
    {
        uint8_t src1[] = {1, 2, 3, 4};
        rb.write(src1, length);

        uint8_t dst1[4];
        TEST_ASSERT_TRUE(rb.read(dst1, &length));
        TEST_ASSERT_EQUAL(dst1[0], 1);
        TEST_ASSERT_EQUAL(dst1[1], 2);
        TEST_ASSERT_EQUAL(dst1[2], 3);
        TEST_ASSERT_EQUAL(dst1[3], 4);

        uint8_t dst2[4];
        TEST_ASSERT_FALSE(rb.read(dst2, &length));
    }
}

TEST_CASE("Ring buffer multiple writes", "[common]")
{
    ring_buffer<2, 4> rb;

    size_t length = 4;

    uint8_t src1[] = {1, 2, 3, 4};
    rb.write(src1, length);

    uint8_t src2[] = {5, 6, 7, 8};
    rb.write(src2, length);

    uint8_t dst1[4];
    TEST_ASSERT_TRUE(rb.read(dst1, &length));
    TEST_ASSERT_EQUAL(dst1[0], 1);
    TEST_ASSERT_EQUAL(dst1[1], 2);
    TEST_ASSERT_EQUAL(dst1[2], 3);
    TEST_ASSERT_EQUAL(dst1[3], 4);

    uint8_t dst2[4];
    TEST_ASSERT_TRUE(rb.read(dst2, &length));
    TEST_ASSERT_EQUAL(dst2[0], 5);
    TEST_ASSERT_EQUAL(dst2[1], 6);
    TEST_ASSERT_EQUAL(dst2[2], 7);
    TEST_ASSERT_EQUAL(dst2[3], 8);
}

TEST_CASE("Ring buffer overflow", "[common]")
{
    ring_buffer<2, 4> rb;

    size_t length = 4;

    uint8_t src1[] = {1, 2, 3, 4};
    rb.write(src1, length);

    uint8_t src2[] = {5, 6, 7, 8};
    rb.write(src2, length);

    uint8_t src3[] = {9, 10, 11, 12};
    rb.write(src3, length);

    uint8_t dst1[4];
    TEST_ASSERT_TRUE(rb.read(dst1, &length));
    TEST_ASSERT_EQUAL(dst1[0], 5);
    TEST_ASSERT_EQUAL(dst1[1], 6);
    TEST_ASSERT_EQUAL(dst1[2], 7);
    TEST_ASSERT_EQUAL(dst1[3], 8);

    uint8_t src4[] = {13, 14, 15, 16};
    rb.write(src4, length);

    uint8_t dst2[4];
    TEST_ASSERT_TRUE(rb.read(dst2, &length));
    TEST_ASSERT_EQUAL(dst2[0], 9);
    TEST_ASSERT_EQUAL(dst2[1], 10);
    TEST_ASSERT_EQUAL(dst2[2], 11);
    TEST_ASSERT_EQUAL(dst2[3], 12);

    uint8_t dst3[4];
    TEST_ASSERT_TRUE(rb.read(dst3, &length));
    TEST_ASSERT_EQUAL(dst3[0], 13);
    TEST_ASSERT_EQUAL(dst3[1], 14);
    TEST_ASSERT_EQUAL(dst3[2], 15);
    TEST_ASSERT_EQUAL(dst3[3], 16);
}
