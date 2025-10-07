//
// Created by Dextron12 on 8/10/2025.
//

#ifndef DEXIUM_SIGNAL_HPP
#define DEXIUM_SIGNAL_HPP

#include <functional>
#include <vector>

/* A signal is can be thought of as a radio station
* listeners *connect* to the station and the station chooses when adn what to broadcast
* listeners *stay connected* even when the broadcast has stopped, unless they explicitly disconnect (if the listener is destroyed)
*
*  Can eb used to send messages arounda  program without needing individual systems to be aware of each toher (just need access to the signal)
 */

template<typename... Args> // Allows any arg/param to be provided to a signal
class Signal {
public:
    using Slot = std::function<void(Args...)>;

    class Connection { // essentially the listener
    public:
        Connection() = default;
        Connection(Signal* sig, size_t id) : signal(sig), id(id) {}

        void disconnect() { // Manually cal to diconnect listener. Should be done before no longert lsitening so theres no dangling functions in the signal stack
            if (signal) {
                signal->disconnect(id);
                signal = nullptr;
            }
        }

    private:
        Signal* signal;
        size_t id = 0;
    };

    // The listener. Signal can have multiple listeners(all executed in order)
    Connection connect(Slot slot) {
        slots.emplace_back(nextID++, std::move(slot));
        return Connection(this, nextID-1);
    }

    // When lsiteners need to recieve something, the signal emits (args -> data to send to lsiteners. Listeners must be able to accept them!)
    void emit(Args... args) {
        for (auto& [id, slot] : slots) {
            slot(args...);
        }
    }

    // Clears all lsitners from stack
    void clear() {
        slots.clear();
    }

private:
    void disconnect(size_t id) {
        slots.erase(
            std::remove_if(slots.begin(), slots.end(),
                [id](auto& pair){return pair.first == id; }),
                slots.end());
    }

    std::vector<std::pair<size_t, Slot>> slots;
    size_t nextID = 0;
};



#endif //DEXIUM_SIGNAL_HPP