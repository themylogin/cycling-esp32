#include <deque>
#include <mutex>
#include <vector>

#include "optional.hpp"

using std::deque;
using std::mutex;
using std::vector;

using std::experimental::optional;

template<typename T>
class ring_buffer
{
public:
    ring_buffer(size_t capacity)
    {
        this->pool.resize(capacity);

        for (auto it = this->pool.begin(); it != this->pool.end(); it++)
        {
            this->empty.push_back(&*it);
        }
    }

    optional<T&> consume()
    {
        this->lock.lock();

        optional<T&> result;
        if (!this->filled.empty())
        {
            result = *this->filled.front();
            this->filled.pop_front();
        }

        this->lock.unlock();

        return result;
    }

    void notify_consume_success(T& v)
    {
        this->empty.push_back(&v);
    }

    void notify_consume_failure(T& v)
    {
        this->filled.push_front(&v);
    }

    T& produce()
    {
        this->lock.lock();

        if (this->empty.empty())
        {
            T& dropped = *this->filled.front();
            this->filled.pop_front();

            this->empty.push_back(&dropped);
        }

        T& result = *this->empty.front();
        this->empty.pop_front();

        this->lock.unlock();

        return result;
    }

    void notify_produce_success(T& v)
    {
        this->filled.push_back(&v);
    }

    void notify_produce_failure(T& v)
    {
        this->empty.push_front(&v);
    }

    typename deque<T*>::iterator begin()
    {
        return this->filled.begin();
    }

    typename deque<T*>::iterator end()
    {
        return this->filled.end();
    }

    size_t count() const
    {
        return this->filled.size();
    }

    void release(T& v)
    {
        for (auto it = this->filled.begin(); it != this->filled.end(); it++)
        {
            if (*it == &v)
            {
                this->filled.erase(it);
                break;
            }
        }

        this->empty.push_back(&v);
    }

private:
    vector<T> pool;

    deque<T*> filled;
    deque<T*> empty;

    mutex lock;
};
