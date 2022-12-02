#pragma once

#include <oneapi/tbb/concurrent_queue.h>
#include <spdlog/spdlog.h>
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
        spdlog::debug("requester starting");

        if (!should_work) {
            should_work = true;
            createWorkers();
        }
        spdlog::debug("requester started");
    }

    void stop() {
        spdlog::debug("requester stopping");
        should_work.exchange(false, std::memory_order::acquire);

        for (auto& worker: workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
        spdlog::debug("requester stopped");
    }

    [[nodiscard]] bool isRunning() const {
        return should_work;
    }

    std::weak_ptr<CommunicationChannel> getChannel() {
        auto channel = std::make_shared<CommunicationChannel>();
        channels.push_back(channel);
        return channel;
    }

private:
    void createWorkers() {
        spdlog::debug("requester creating workers");

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
        spdlog::debug("requester created workers");
    }

    std::vector<std::shared_ptr<CommunicationChannel>> channels;
    ReceiverFunction receiver;
    std::vector<std::thread> workers;
    std::atomic_bool should_work;
};
