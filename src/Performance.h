#pragma once

#include "ErrorConverter.h"
#include <zephyr/kernel.h>


class Performance
{
public:
    static ReturnCode GetUtilization(float& utilization)
    {
#ifndef CONFIG_SCHED_THREAD_USAGE_ALL
        return ReturnCode::NotSupported;
#else
        k_thread_runtime_stats_t stats;
        auto err = k_thread_runtime_stats_all_get(&stats);
        auto rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        utilization = stats.total_cycles / (float) stats.execution_cycles;

        return ReturnCode::Success;
#endif // CONFIG_SCHED_THREAD_USAGE_ALL
    }
};
