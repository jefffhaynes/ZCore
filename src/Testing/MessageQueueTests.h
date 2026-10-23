#pragma once

#include <type_traits>
#include "MessageQueue.h"

namespace MessageQueueTests
{
    using Queue = MessageQueue<int, 4>;

    static_assert(!std::is_copy_constructible_v<Queue>, "MessageQueue copy constructible");
    static_assert(!std::is_copy_assignable_v<Queue>, "MessageQueue copy assignable");
    static_assert(!std::is_move_constructible_v<Queue>, "MessageQueue move constructible");
    static_assert(!std::is_move_assignable_v<Queue>, "MessageQueue move assignable");

    static_assert(requires(Queue& queue, int& item) { queue.Put(item); }, "Put needs a timeout");
    static_assert(requires(Queue& queue, int& item) { queue.Get(item); }, "Get needs a timeout");
}
