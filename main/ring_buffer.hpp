#include <deque>
#include <mutex>

using std::deque;
using std::mutex;

template<typename T>
class RingBuffer
{
public:
    void add(T& v)
    {
        this->empty.push_back(v);
    }

    bool canStartConsuming()
    {
        return !this->filled.empty();
    }

    T& startConsuming()
    {
        this->lock.lock();

        T& result = this->filled.front();
        this->filled.pop_front();
        this->lock.unlock();
        return result;
    }

    void endConsuming(const T& v)
    {
        this->empty.push_back(v);
    }

    T& startProducing()
    {
        this->lock.lock();

        if (this->empty.empty())
        {
            T& dropped = this->filled.front();
            this->filled.pop_front();

            this->empty.push_back(dropped);
        }

        T& result = this->empty.front();
        this->empty.pop_front();
        this->lock.unlock();
        return result;
    }

    void endProducing(const T& v)
    {
        this->filled.push_back(v);
    }

private:
    deque<T> filled;
    deque<T> empty;

    mutex lock;
};
