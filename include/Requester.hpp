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
          receiver(receiver) {
        assert(std::all_of(this->channels.begin(), this->channels.end(), [] (auto& obj) {return static_cast<bool>(obj);}));
    }

    ~Requester() {
        stop();
    }

    void start() {
        assert(receiver);
        spdlog::debug("requester starting");

        if (!is_running) {
            is_running = true;
            createWorkers();
        }
        spdlog::debug("requester started");
    }

    void stop() {
        if (is_running) {
            spdlog::debug("requester stopping");
            is_running.exchange(false, std::memory_order::acquire);
            spdlog::debug("requester stopped");
        }
    }

    [[nodiscard]] bool isRunning() const noexcept {
        return is_running;
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
                std::jthread(
                    [&] {
                        while (is_running) {
                            std::invoke(receiver, *channel);
                        }
                    }));
        }
        spdlog::debug("requester created workers");
    }

    std::vector<std::shared_ptr<CommunicationChannel>> channels;
    ReceiverFunction receiver;
    std::vector<std::jthread> workers;
    std::atomic_bool is_running;
};
