#pragma once

#include <oneapi/tbb/concurrent_queue.h>
#include <cassert>
#include <functional>

template <typename T>
class Provider {
    using CommunicationChannel = oneapi::tbb::concurrent_queue<T>;
    using GeneratorFunction = std::function<T ()>;
    using SenderFunction = std::function<void (CommunicationChannel&, T)>;
public:
    template <typename G, typename S>
    Provider(std::weak_ptr<CommunicationChannel> channel, G&& generator, S&& sender)
        : channel(std::move(channel)),
          generator(generator),
          sender(sender) {}

    ~Provider() {
        stop();
    }

    void start() {
        assert(generator);
        assert(sender);

        if (!should_work) {
            should_work = true;
            worker = std::thread(
                [&] {
                    while (should_work) {
                        if (auto chan = channel.lock()) {
                            std::invoke(sender, *chan, generator());
                        } else {
                            should_work.exchange(false, std::memory_order::acquire);
                            break;
                        }
                    }
                });
        }
    }

    void stop() {
        should_work.exchange(false, std::memory_order::acquire);

        if (worker.joinable()) {
            worker.join();
        }
    }

    [[nodiscard]] bool isRunning() const {
        return should_work;
    }

private:
    std::weak_ptr<CommunicationChannel> channel;
    GeneratorFunction generator;
    SenderFunction sender;
    std::thread worker;
    std::atomic_bool should_work;
};
