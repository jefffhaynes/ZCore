#include <zephyr/kernel.h>
#include <zephyr/ztest.h>
#include "MessageQueue.h"
#include "Testing/MessageQueueTests.h"

namespace
{
    constexpr uint32_t Capacity = 4;
    constexpr TimeSpan Delay = TimeSpan::FromMilliseconds(50);
    constexpr TimeSpan Patience = TimeSpan::FromSeconds(1);
    constexpr int64_t DelayMilliseconds = static_cast<int64_t>(Delay.ToMilliseconds());

    struct Message
    {
        uint32_t Sequence = 0;
        int32_t Value = 0;
    };

    using Queue = MessageQueue<Message, Capacity>;

    ReturnCode interruptResult;

    Message Numbered(uint32_t sequence)
    {
        return Message{ sequence, -static_cast<int32_t>(sequence) };
    }

    void Fill(Queue& queue)
    {
        for(uint32_t i = 0; i < Capacity; i++)
        {
            zassert_equal(queue.Put(Numbered(i)), ReturnCode::Success);
        }
    }

    void PutFromInterrupt(struct k_timer* timer)
    {
        auto queue = static_cast<Queue*>(k_timer_user_data_get(timer));
        interruptResult = queue->Put(Numbered(7));
    }
}

ZTEST_SUITE(message_queue, nullptr, nullptr, nullptr, nullptr, nullptr);

ZTEST(message_queue, test_without_a_timeout_put_and_get_dont_wait)
{
    Queue queue;
    auto start = k_uptime_get();

    Fill(queue);
    zassert_equal(queue.Put(Numbered(Capacity)), ReturnCode::Timeout, "put into a full queue");

    Message message;

    for(uint32_t i = 0; i < Capacity; i++)
    {
        zassert_equal(queue.Get(message), ReturnCode::Success);
        zassert_equal(message.Sequence, i, "out of order");
        zassert_equal(message.Value, -static_cast<int32_t>(i));
    }

    zassert_equal(queue.Get(message), ReturnCode::Timeout, "got from an empty queue");

    zassert_equal(k_uptime_get(), start, "waited");
}

ZTEST(message_queue, test_put_and_get_wait_out_their_timeouts)
{
    Queue queue;
    Message message;

    auto start = k_uptime_get();
    zassert_equal(queue.Get(message, Delay), ReturnCode::Timeout);
    zassert_true(k_uptime_get() - start >= DelayMilliseconds, "get didn't wait");

    Fill(queue);

    start = k_uptime_get();
    zassert_equal(queue.Put(Numbered(Capacity), Delay), ReturnCode::Timeout);
    zassert_true(k_uptime_get() - start >= DelayMilliseconds, "put didn't wait");
}

ZTEST(message_queue, test_get_waits_for_a_put_from_an_interrupt)
{
    Queue queue;
    struct k_timer timer;
    k_timer_init(&timer, PutFromInterrupt, nullptr);
    k_timer_user_data_set(&timer, &queue);
    interruptResult = ReturnCode::InvalidState;

    auto start = k_uptime_get();
    k_timer_start(&timer, TimeHelper::ToTimeout(Delay), K_NO_WAIT);

    Message message;
    zassert_equal(queue.Get(message, Patience), ReturnCode::Success);
    zassert_true(k_uptime_get() - start >= DelayMilliseconds, "didn't wait for the put");
    zassert_equal(interruptResult, ReturnCode::Success);
    zassert_equal(message.Sequence, 7);

    k_timer_stop(&timer);
}

ZTEST(message_queue, test_peek_leaves_the_next_item)
{
    Queue queue;
    Message message;

    zassert_equal(queue.Peek(message), ReturnCode::NotFound, "peeked into an empty queue");

    zassert_equal(queue.Put(Numbered(1)), ReturnCode::Success);
    zassert_equal(queue.Put(Numbered(2)), ReturnCode::Success);

    for(uint32_t i = 0; i < 2; i++)
    {
        zassert_equal(queue.Peek(message), ReturnCode::Success);
        zassert_equal(message.Sequence, 1);
    }

    zassert_equal(queue.Get(message), ReturnCode::Success);
    zassert_equal(message.Sequence, 1);

    zassert_equal(queue.Peek(message), ReturnCode::Success);
    zassert_equal(message.Sequence, 2);
}
