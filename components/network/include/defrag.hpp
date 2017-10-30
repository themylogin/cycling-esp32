#include <cstring>
#include <cinttypes>
#include <map>
#include <utility>

#include "esp_log.h"

#include "optional.hpp"
#include "ring_buffer.hpp"

#include "packet_fragment.hpp"
#include "packet.hpp"

using std::experimental::optional;
using std::map;
using std::pair;

static const char* TAG = "defrag";

class defrag
{
public:
    defrag(ring_buffer<packet_fragment>& fragments, ring_buffer<packet>& packets) :
        fragments(fragments),
        packets(packets)
    {
    }

    optional<packet&> push_fragment(packet_fragment& fragment)
    {
        if (fragment.seq.fragment.fragment >= fragment.seq.fragment.nr_fragments)
        {
            ESP_LOGW(TAG, "Dropping packet with fragment=%" PRIu16 ", nr_fragments=%" PRIu16,
                     fragment.seq.fragment.fragment, fragment.seq.fragment.nr_fragments);

            this->fragments.notify_produce_failure(fragment);

            return {};
        }

        if (fragment.seq.fragment.nr_fragments == 1)
        {
            packet& result = this->packets.produce();

            result.network = fragment.network;
            result.src = fragment.src;
            result.seq = fragment.seq.seq | 0xffff;
            memcpy(result.data, fragment.data, fragment.size);
            result.size = fragment.size;

            this->fragments.notify_produce_failure(fragment);

            return result;
        }
        else
        {
            map<int, packet_fragment&> another_fragments;
            another_fragments.insert(pair<int, packet_fragment&>(
                fragment.seq.fragment.fragment, fragment));

            for (auto it = this->fragments.begin(); it != this->fragments.end(); it++)
            {
                packet_fragment& another_fragment = **it;

                if (another_fragment.network == fragment.network &&
                    another_fragment.src == fragment.src &&
                    another_fragment.seq.fragment.seq == fragment.seq.fragment.seq)
                {
                    another_fragments.insert(pair<int, packet_fragment&>(
                        another_fragment.seq.fragment.fragment, another_fragment));
                }
            }

            if (another_fragments.size() == fragment.seq.fragment.nr_fragments)
            {
                packet& result = this->packets.produce();

                result.network = fragment.network;
                result.src = fragment.src;
                result.seq = fragment.seq.seq | 0xffff;

                result.size = 0;
                for (auto it = another_fragments.begin(); it != another_fragments.end(); it++)
                {
                    packet_fragment& another_fragment = (*it).second;

                    memcpy(result.data + result.size, another_fragment.data, another_fragment.size);
                    result.size += another_fragment.size;
                }

                this->fragments.notify_produce_failure(fragment);

                return result;
            }
            else
            {
                this->fragments.notify_produce_success(fragment);

                return {};
            }
        }
    }

private:
    ring_buffer<packet_fragment>& fragments;
    ring_buffer<packet>& packets;
};
