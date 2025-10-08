//
// Created by Dextron12 on 8/10/2025.
//

#ifndef DEXIUM_SIGNAL_HPP
#define DEXIUM_SIGNAL_HPP

#include <functional>
#include <typeindex>
#include <vector>

/* A signal is can be thought of as a radio station
* listeners *connect* to the station and the station chooses when adn what to broadcast
* listeners *stay connected* even when the broadcast has stopped, unless they explicitly disconnect (if the listener is destroyed)
*
*  Can eb used to send messages arounda  program without needing individual systems to be aware of each toher (just need access to the signal)
 */

// A base class for Signal to allow type-erasure
class ISignal {
public:
    virtual ~ISignal() = default;
    virtual std::type_index type() const = 0; // each signal reports its type
};

template<typename... Args> // Allows any arg/param to be provided to a signal
class Signal final : public ISignal {
public:
    using Slot = std::function<void(Args...)>;

    //Ovverride type_index to explicitly set the template type for the signal
    std::type_index type() const override {
        return typeid(Signal<Args...>);
    }

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


namespace Dexium {
    // Signal Manager

    // Mostly used for internal signal management in the engine. But could be sued by the end-user
    class SignalManager {
    public:

        static SignalManager& get() {
            static SignalManager instance;
            return instance;
        }

        template<typename... Args>
        std::shared_ptr<Signal<Args...>> getSignal(const std::string& ID) {
            // Check if signal already exists (by ID)
            auto it = signals.find(ID);
            if (it != signals.end()) {
                //run-time type check
                if (it->second->type() != typeid(Signal<Args...>)) {
                    TraceLog(Dexium::LOG_ERROR, "Signal type mismatch for {}", ID);
                    return nullptr;
                }
                return std::dynamic_pointer_cast<Signal<Args...>>(it->second);
            } else {
                TraceLog(Dexium::LOG_WARNING, "The requested signal {} does not exist", ID);
                return nullptr;
            }
        }

        template<typename... Args>
        std::shared_ptr<Signal<Args...>> createSignal(const std::string& ID) {
            // Check if it exists
            auto it = signals.find(ID);
            if (it != signals.end()) {
                TraceLog(LOG_ERROR, "Signal {} already exists, can not re-create it", ID);
                return nullptr;
            }
            // Otherwise, Create new signal
            auto sig = std::make_shared<Signal<Args...>>();
            signals[ID] = sig;
            return sig;
        }

        //Helper function to directly allow user to create listender from ID
        template<typename... Args, typename Func>
        auto connect(const std::string& id, Func&& func) {
            auto sig = getSignal<Args...>(id);
            if (!sig) {
                return typename Signal<Args...>::Connection{};
            }
            return sig->connect(std::forward<Func>(func));
        }

        //Helper function to directly allow suer to emit signal from ID
        template<typename... Args>
        void emit(const std::string& id, Args&&... args) {
            auto sig = getSignal<Args...>(id);
            if (sig) {
                sig->emit(std::forward<Args>(args)...);
            } else {
                TraceLog(LOG_WARNING, "Signal {} emitted, but no listeners are bound!");
            }
        }

        //Helper to disconnect listeners and then destroy signal
        template<typename... Args>
        void disconnect (const std::string& ID) {
            auto it = signals.find(ID);
            if (it != signals.end()) {
                // Singal found, remove it
                auto sig = getSignal<Args...>(ID);
                sig->clear();
                signals.erase(it);
                TraceLog(LOG_INFO, "Signal {} disconnected", ID);
            }
        }


    private:
        SignalManager() = default;
        ~SignalManager() = default;

        // Prevent copying
        SignalManager(SignalManager const&) = delete;
        SignalManager& operator=(SignalManager const&) = delete;

        std::unordered_map<std::string, std::shared_ptr<ISignal>> signals;
    };
}



#endif //DEXIUM_SIGNAL_HPP