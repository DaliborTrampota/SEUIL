#pragma once

#include <functional>
#include <memory>
#include <vector>


namespace ui {

    struct Connection {
        Connection(size_t id, std::weak_ptr<void> signal) : m_id(id), m_signal(signal) {}

        size_t id() const { return m_id; }
        bool connected() const { return m_signal.expired(); }

      private:
        size_t m_id;
        std::weak_ptr<void> m_signal;
    };

    template <typename... Args>
    class Signal {
        using FuncT = std::function<void(Args...)>;

      public:
        Signal() = default;

        Signal(const Signal& other) = delete;
        Signal(Signal&& other) noexcept = delete;

        Signal& operator=(const Signal& other) = delete;
        Signal& operator=(Signal&& other) noexcept = delete;

        void emit(Args... args) {
            for (auto& [_, callback] : m_callbacks) {
                callback(args...);
            }
        }

        [[nodiscard]] Connection connect(FuncT callback) {
            size_t id = m_next_id++;
            m_callbacks.emplace_back(id, std::move(callback));
            return Connection(id, m_tracker);
        }

        void disconnect(const Connection& connection) {
            auto removeBegin = std::remove_if(
                m_callbacks.begin(), m_callbacks.end(), [id = connection.id()](const auto& pair) {
                    return pair.first == id;
                }
            );
            m_callbacks.erase(removeBegin, m_callbacks.end());
        }

      private:
        size_t m_next_id = 0;
        std::vector<std::pair<size_t, FuncT>> m_callbacks;
        std::shared_ptr<void> m_tracker = std::make_shared<int>(0);
    };
}  // namespace ui