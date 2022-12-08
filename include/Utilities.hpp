#pragma once

#include <oneapi/tbb/concurrent_queue.h>
#include <atomic>
#include <mutex>
#include <optional>

struct NoWaitPolicy {
    static void wait() {}
};

template <typename Duration = std::chrono::seconds, int num = 1>
struct SleepPolicy {
    static void wait() {std::this_thread::sleep_for(Duration(num));}
};

struct NoLockPolicy {
    struct NoMutex {};

    struct NoLock {
        explicit NoLock(NoMutex&) {}
    };

    using MutexType = NoMutex;
    using LockType = NoLock;
};

struct UniqueLockPolicy {
    using MutexType = std::mutex;
    using LockType = std::unique_lock<MutexType>;
};

template <typename WaitPolicy = NoWaitPolicy, typename LockPolicy = NoLockPolicy>
struct Generator {
    uint operator ()() {
        typename LockPolicy::LockType lock(mutex);
        WaitPolicy::wait();
        return counter++;
    }

    static inline std::atomic_uint counter {0};
    static inline LockPolicy::MutexType mutex;
};

template <typename WaitPolicy = NoWaitPolicy, typename LockPolicy = NoLockPolicy>
struct Sender {
    void operator ()(oneapi::tbb::concurrent_queue<uint>& queue, uint&& val) {
        typename LockPolicy::LockType lock(mutex);
        WaitPolicy::wait();
        queue.push(val);
        counter++;
    }

    static inline std::atomic_uint counter {0};
    static inline LockPolicy::MutexType mutex;
};

template <typename WaitPolicy = NoWaitPolicy, typename LockPolicy = NoLockPolicy>
struct Receiver {
    void operator ()(oneapi::tbb::concurrent_queue<uint>& queue) {
        typename LockPolicy::LockType lock(mutex);
        WaitPolicy::wait();
        std::optional<uint> val;

        if (queue.try_pop(*val)) {
            counter++;
        }
    }

    static inline std::atomic_uint counter {0};
    static inline LockPolicy::MutexType mutex;
};

#define REGISTER_FUNC_OBJECT(class_name) \
    using Default ## class_name = class_name<>; \
    template <typename Duration = std::chrono::seconds, int num = 1> \
    using Sleeping ## class_name = class_name<SleepPolicy<Duration, num>>; \
    using Blocking ## class_name = class_name<NoWaitPolicy, UniqueLockPolicy>; \
    template <typename Duration = std::chrono::seconds, int num = 1> \
    using SleepingBlocking ## class_name = class_name<SleepPolicy<Duration, num>, UniqueLockPolicy>;

REGISTER_FUNC_OBJECT(Generator)
REGISTER_FUNC_OBJECT(Sender)
REGISTER_FUNC_OBJECT(Receiver)
