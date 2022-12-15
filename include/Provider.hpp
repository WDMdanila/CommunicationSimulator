#pragma once

#include <oneapi/tbb/concurrent_queue.h>
#include <spdlog/spdlog.h>
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
            spdlog::debug("provider starting");
            should_work = true;
            worker = std::jthread(
                [&] {
                    while (should_work) {
                        if (auto chan = channel.lock()) {
                            std::invoke(sender, *chan, std::invoke(generator));
                        } else {
                            should_work.exchange(false, std::memory_order::acquire);
                            break;
                        }
                    }
                });
            spdlog::debug("provider started");
        }
    }

    void stop() {
        if (should_work) {
            spdlog::debug("provider stopping");
            should_work.exchange(false, std::memory_order::acquire);
            spdlog::debug("provider stopped");
        }
    }

    [[nodiscard]] bool isRunning() const {
        return should_work;
    }

private:
    std::weak_ptr<CommunicationChannel> channel;
    GeneratorFunction generator;
    SenderFunction sender;
    std::jthread worker;
    std::atomic_bool should_work;
};
