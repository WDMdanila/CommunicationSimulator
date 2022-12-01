#pragma once

#include <oneapi/tbb/concurrent_queue.h>
#include <functional>
#include <list>

template <typename T>
class Requester {
    using CommunicationChannel = oneapi::tbb::concurrent_queue<T>;
    using ReceiverFunction = std::function<void (CommunicationChannel&)>;
public:
    template <typename R>
    Requester(std::vector<std::shared_ptr<CommunicationChannel>> channels, R&& receiver)
        : channels(std::move(channels)),
          receiver(receiver) {}

    ~Requester() {
        stop();
    }

    void start() {
        assert(receiver);

        if (!should_work) {
            should_work = true;

            for (auto& channel: channels) {
                assert(channel);
                workers.push_back(
                    std::thread(
                        [&] {
                            while (should_work) {
                                std::invoke(receiver, *channel);
                            }
                        }));
            }
        }
    }

    void stop() {
        should_work.exchange(false, std::memory_order::acquire);

        for (auto& worker: workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

    [[nodiscard]] bool isRunning() const {
        return should_work;
    }

private:
    std::vector<std::shared_ptr<CommunicationChannel>> channels;
    ReceiverFunction receiver;
    std::vector<std::thread> workers;
    std::atomic_bool should_work;
};
