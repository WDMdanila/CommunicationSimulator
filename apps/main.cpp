#include "CommunicationFactory.hpp"
#include "Utilities.hpp"
#include <optional>

int main() {
#ifdef NDEBUG
    spdlog::set_level(spdlog::level::info);
#else
    spdlog::set_level(spdlog::level::debug);
#endif

    CommunicationFactory<uint> factory {};

    spdlog::info("creating swarm");
    auto [swarm, requester] = factory.create_swarm(10, Generator{}, Sender{}, Receiver{});
    spdlog::info("created swarm, starting");

    swarm->emplace(requester->getChannel(), Generator{}, Sender{});

    swarm->start();
    requester->start();

    spdlog::info("started, waiting for second");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    spdlog::info("done waiting, stopping");
    swarm->stop();
    requester->stop();
    spdlog::info("total generated: {}", Generator<>::counter.load());
    spdlog::info("total sent: {}", Sender<>::counter.load());
    spdlog::info("total received: {}", Receiver<>::counter.load());

    return 0;
}
