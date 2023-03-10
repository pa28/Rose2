/**
* @file Signals.h
* @author Richard Buckley <richard.buckley@ieee.org>
* @version 1.1
* @date 2021-03-07, 2023-01-26
* @brief Establish an intra-application signaling protocol.
*/

#ifndef ROSE2_SIGNALS_H
#define ROSE2_SIGNALS_H

#include <memory>
#include <functional>
#include <utility>
#include <vector>

namespace rose {

    /**
     * @struct Slot
     * @brief The receiver portion of a Signal-Slot transmitter receiver pair.
     * @tparam Args The signature of the Slot.
     * @details The signature of the Slot also sets the signature of the receiver function that is called when a
     * Signal is received. Signals and Slots must share the same signature to be connected.
     */
    template<typename ... Args>
    struct Slot : public std::enable_shared_from_this<Slot<Args...>> {
        using signal_function = std::function<void(Args...)>;

        signal_function receiver{};
        bool connected{false};
    };

    /**
     * @class Signal
     * @brief The transmitter portion of a Signal-Slot transmitter receiver pair.
     * @tparam Args The signature of the Signal.
     * @details The signature of the Signal also sets the signature of the transmit() function. Signals and Slots must
     * share the same signature to be connected. The Signal maintains a list of std::weak_ptr to the Slots with which
     * is is connected. This list is groomed to remove expired Slots on each call to transmit, connect and disconnect.
     */
    template<typename ... Args>
    class Signal {
    protected:
        std::vector<std::weak_ptr<Slot<Args...>>> callList{};

        std::function<void()> connecting{};     ///< Called when new slots are connected. See transmitLastConnected()

    public:
        Signal() = default;

        /**
         * @brief Constructor that sets the connecting() callback.
         * @param connecting The function to call when a new slot is connecting.
         */
        explicit Signal(std::function<void()> connecting) : connecting(std::move(connecting)) {}

        Signal(const Signal&) = delete;
        Signal(Signal&&)  noexcept = default;
        Signal& operator = (const Signal&) = delete;
        Signal& operator = (Signal&&)  noexcept = default;

        ~Signal() = default;

        using signal_function = std::function<void(Args...)>;

        explicit operator bool () {
            auto r = !callList.empty();
            return r;
        }

        /**
         * @brief Groom the Slot list.
         */
        void clean() {
            callList.erase(std::remove_if(callList.begin(), callList.end(), [](auto weak) {
                return weak.expired();
            }), callList.end());
        }

        /**
         * @brief Connect a Slot to the Signal only if it has not already been connected.
         * @param slot
         */
        void connect(std::shared_ptr<Slot<Args...>> &slot) {
            if (slot) {
                clean();
                for (auto &weak : callList) {
                    if (auto locked = weak.lock(); locked) {
                        if (locked == slot) {
                            slot->connected = true;
                            return;
                        }
                    }
                }

                callList.push_back(slot);
                slot->connected = true;
                if (connecting) connecting();
            }
        }

        /**
         * @brief Disconnect a Slot from the Signal, if it is connected.
         * @param slot The Slot to disconnect.
         */
        void disconnect(std::shared_ptr<Slot<Args...>> &slot) {
            callList.erase(std::remove_if(callList.begin(), callList.end(), [&](auto weak) {
                if (weak.expired()) {
                    return true;
                } else if (auto strong = weak.lock(); strong) {
                    slot->connected = false;
                    return strong == slot;
                }
                return false;
            }));
        }

        /**
         * @brief Transmit the Signal data to all connected and non-expired Slots.
         * @param args The signature of the Signal data.
         */
        void transmit(Args ... args) {
            bool expired = false;
            for (auto &weak : callList) {
                if (auto slot = weak.lock(); slot) {
                    if (slot->receiver)
                        slot->receiver(args...);
                } else {
                    expired = true;
                }
            }

            if (expired)
                clean();
        }

        /**
         * @brief Transmit a signal only to the last connection.
         * @details This can be used with the connecting callback to 'catchup' late connectors.
         * @param args the arguments to the signal.
         */
        void transmitLastConnected(Args ... args) {
            auto weak = callList.back();
            if (!weak.expired()) {
                if (auto slot = weak.lock(); slot) {
                    if (slot->receiver)
                        slot->receiver(args...);
                }
            } else {
                clean();
            }
        }
    };

    /**
     * @struct Protocol
     * @brief A convenience structure that composes Signal and Slot types from a protocol signature, and provides
     * a Slot factory.
     * @details All Slots should be created by the appropriated Protocol Slot factory. This ensures that the Slots
     * will be properly composed std::shared_ptr or std::weak_ptr. Also using a Protocol structure to create Signals
     * and Slots will ensure that those intended to be connected have compatible signatures.
     * @tparam Args The signature of the Signal and Slots in the Protocol.
     */
    template<typename ... Args>
    struct Protocol {
        typedef std::shared_ptr<Slot<Args...>> slot_type;   ///< Composed Slot type.
        typedef Signal<Args...> signal_type;                ///< Composed Signal type.

        /**
         * @brief The Protocol Slot factory.
         * @return
         */
        static slot_type createSlot() {
            return std::make_shared<Slot<Args...>>();
        }
    };
}

#endif /* ROSE2_SIGNALS_H */
