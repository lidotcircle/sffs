#pragma once
#include <utility>
#include <cassert>
#include <cstddef>


template<typename T, size_t N>
class maxsize_vector {
public:
    inline size_t size() const { return this->m_size; }

    inline T&       back() { return m_array[m_size - 1].as(); }
    inline const T& back() const { return m_array[m_size - 1].as(); }

    inline void clear() { while (!this->empty()) this->pop_back(); }
    template<typename K>
    inline void push_back(K&& val) { assert(m_size < N); m_array[m_size++].construct_with(std::forward<K>(val));  }
    template<typename ... Args>
    inline void emplace_back(Args&& ... args) { assert(m_size < N); m_array[m_size++].construct_with(std::forward<Args>(args)...);  }
    inline void pop_back()       { assert(m_size > 0); m_array[--m_size].destroy();  }

    inline T&       operator[](size_t idx) { assert(idx < m_size); return m_array[idx].as(); }
    inline const T& operator[](size_t idx) const { assert(idx < m_size); return m_array[idx].as(); }

    inline bool empty() const { return m_size == 0; }

    inline maxsize_vector(std::initializer_list<T> vec): m_size(0) {
        for (auto v: vec)
            this->push_back(v);
    }

    inline maxsize_vector(): m_size(0) {}
    inline ~maxsize_vector() { this->clear(); }

private:
    struct DT {
        template<typename ... Args>
        void construct_with(Args&& ... args) { new (m_buf) T(std::forward<Args>(args)...); }

        void destroy() { this->as().~T(); }

        const T& as() const { return const_cast<DT*>(this)->as(); }
        T& as() { return *reinterpret_cast<T*>(static_cast<void*>(m_buf)); }

    private:
        alignas(T) std::byte m_buf[sizeof(T)];
    };
    size_t m_size;
    DT m_array[N];
};
