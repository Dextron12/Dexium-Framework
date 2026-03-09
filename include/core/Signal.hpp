//
// Created by Dextron12 on 7/3/26.
//

#ifndef DEXIUM_SIGNAL_HPP
#define DEXIUM_SIGNAL_HPP


// A RAII style Signals implementation that I hope I can sue to replace EnTT Signal so the lib cna be removed from the project

#include <vector>
#include <algorithm> // Used for std::remove_if
#include <cstdint>

namespace Dexium::Core {

    template<typename... Args>
    class Signal {
        // NOn std::func slot (uses ptr to fn instead)
        struct Slot {
            uint32_t id;
            void* instance;
            void (*call)(void*, Args...);
            bool active;
        };
    public:
        class Connection {
        public:
            Connection() = default;

            Connection(Signal* sig, uint32_t id)
                : m_signal(sig), m_id(id) {}

            ~Connection() {
                disconnect();
            }

            void disconnect() {
                if (m_signal) {
                    m_signal->disconnect(m_id);
                    m_signal = nullptr;
                }
            }

            bool valid() const {
                return m_signal != nullptr;
            }

        private:
            Signal* m_signal = nullptr;
            uint32_t m_id = 0;
        };

        template<class T, void(T::*Method)(Args...)>
        Connection connect(T* instance) {
            Slot slot;

            slot.id = m_nextID++;
            slot.instance = instance;
            slot.active = true;

            slot.call = [](void* obj, Args... args) {
                T* self = static_cast<T*>(obj);
                (self->*Method)(args...);
            };

            m_slots.push_back(slot);

            return Connection(this, slot.id);
        }

        void emit(Args... args) {
            for (auto& s : m_slots) {
                if (s.active) {
                    s.call(s.instance, args...);
                }
            }
        }

        // Removes any inactive slots from the signal
        void cleanup() {
            m_slots.erase(
                std::remove_if(
                    m_slots.begin(), m_slots.end(),
                    [](const Slot& s) {
                        return !s.active;
                    }), m_slots.end());
        }

    private:
        void disconnect(uint32_t id) {
            for (auto& s : m_slots) {
                if (s.id == id) {
                    s.active = false;
                    break;
                }
            }
        }

        std::vector<Slot> m_slots;
        uint32_t m_nextID = 1;
    };

}


/* |--- EXAMPLE USAGE OF THIS SIGNALS CLASS ---|
 * - When a connection goes out of scope, the dtor diconnects itself from the Signal
 * - Can emit data or none at all
 * Here's an example of creating a Listener:
 */
/*
class Enemy {
public:
    void onDamage(int dmg) {
        std::cout << "Enemy too " << dmg << "\n";
    }
};

// Now connectiong
Dexium::Core::Signal<int> damageSignal;
Enemy enemy;
auto conn = damageSignal.connect<Enemy, &Enemy::onDamage>(&enemy);
damageSignal.emit(10);
*/

#endif //DEXIUM_SIGNAL_HPP