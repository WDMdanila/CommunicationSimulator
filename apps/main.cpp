#include "CommunicationFactory.hpp"
#include "Utilities.hpp"
#include <optional>

int main() {
    CommunicationFactory<uint> factory {};

    printf("creating swarm...\n");
    auto [swarm, requester] = factory.create_swarm(10, Generator{}, Sender{}, Receiver{});
    printf("created swarm, starting...\n");

    swarm->start();
    requester->start();

    printf("started, waiting for second...\n");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    printf("done waiting, stopping...\n");
    swarm->stop();
    requester->stop();
    printf("generated: %d\n", Generator<>::counter.load());
    printf("sent: %d\n", Sender<>::counter.load());
    printf("received: %d\n", Receiver<>::counter.load());

    return 0;
}
