#include <unity.h>

#include "defrag.hpp"

TEST_CASE("Defrag drops packets with fragment >= nr_fragments", "[network]")
{
    ring_buffer<packet_fragment> fragments(256);
    ring_buffer<packet> packets(16);

    defrag d(fragments, packets);

    auto fragment = fragments.produce();
    fragment.seq.fragment.seq = 1;
    fragment.seq.fragment.nr_fragments = 1;
    fragment.seq.fragment.fragment = 2;

    auto packet = d.push_fragment(fragment);

    TEST_ASSERT_FALSE(packet);

    TEST_ASSERT_EQUAL(fragments.count(), 0);
}

TEST_CASE("Defrag handles packets with nr_fragments = 1", "[network]")
{
    ring_buffer<packet_fragment> fragments(256);
    ring_buffer<packet> packets(16);

    defrag d(fragments, packets);

    auto fragment = fragments.produce();
    fragment.network                    = 0x1234;
    fragment.src                        = 0x5678,
    fragment.seq.fragment.seq           = 1;
    fragment.seq.fragment.nr_fragments  = 1;
    fragment.seq.fragment.fragment      = 0;
    memcpy(fragment.data,               "ABCD", 4);
    fragment.size                       = 4;

    auto packet = d.push_fragment(fragment);

    TEST_ASSERT_TRUE(packet);

    TEST_ASSERT_EQUAL((*packet).seq, 0x10001ffff);
    TEST_ASSERT_EQUAL(memcmp((*packet).data, "ABCD", 4), 0);
    TEST_ASSERT_EQUAL((*packet).size, 4);

    TEST_ASSERT_EQUAL(fragments.count(), 0);
    TEST_ASSERT_EQUAL(packets.count(), 0);
}

TEST_CASE("Defrag handles packets with nr_fragments = 2", "[network]")
{
    ring_buffer<packet_fragment> fragments(256);
    ring_buffer<packet> packets(16);

    defrag d(fragments, packets);

    auto fragment_1 = fragments.produce();
    fragment_1.network                      = 0x1234;
    fragment_1.src                          = 0x5678,
    fragment_1.seq.fragment.seq             = 1;
    fragment_1.seq.fragment.nr_fragments    = 2;
    fragment_1.seq.fragment.fragment        = 0;
    memcpy(fragment_1.data,                 "ABCD", 4);
    fragment_1.size                         = 4;

    auto packet = d.push_fragment(fragment_1);

    TEST_ASSERT_FALSE(packet);

    TEST_ASSERT_EQUAL(fragments.count(), 1);
    TEST_ASSERT_EQUAL(packets.count(), 0);

    auto fragment_2 = fragments.produce();
    fragment_2.network                      = 0x1234;
    fragment_2.src                          = 0x5678,
    fragment_2.seq.fragment.seq             = 1;
    fragment_2.seq.fragment.nr_fragments    = 2;
    fragment_2.seq.fragment.fragment        = 1;
    memcpy(fragment_2.data,                 "EFGH", 4);
    fragment_2.size                         = 4;

    packet = d.push_fragment(fragment_2);

    TEST_ASSERT_TRUE(packet);

    TEST_ASSERT_EQUAL((*packet).network, 0x1234);
    TEST_ASSERT_EQUAL((*packet).src, 0x5678);
    TEST_ASSERT_EQUAL((*packet).seq, 0x10002ffff);
    TEST_ASSERT_EQUAL(memcmp((*packet).data, "ABCDEFGH", 4), 0);
    TEST_ASSERT_EQUAL((*packet).size, 8);

    TEST_ASSERT_EQUAL(fragments.count(), 1);
    TEST_ASSERT_EQUAL(packets.count(), 0);
}
