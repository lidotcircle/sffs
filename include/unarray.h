#pragma once
#include <cassert>
#include <cstddef>
#include <bitset>


namespace ldc {
template<typename T, size_t N, bool ASSERT_INVALID=false>
class unarray {
public:
    inline size_t size() const { return this->m_size; }
    inline void clear() {
        if constexpr (ASSERT_INVALID) {
            for (size_t i=0;i<N;i++) {
                if (m_ctlbits[i]) {
                    this->destroy(i);
                }
            }
        } else {
            for (size_t i=0;i<m_size;i++)
                this->destroy(i);
        }
    }

    template<typename ... Args>
    inline void construct(size_t idx, Args&& ... args) {
        if constexpr (ASSERT_INVALID) assert(idx < N && !m_ctlbits[idx]);
        m_array[idx].construct_with(std::forward<Args>(args)...);
        m_size++;
        if constexpr (ASSERT_INVALID) m_ctlbits[idx] = true;
    }

    inline void destroy(size_t idx) {
        if constexpr (ASSERT_INVALID) assert(idx < N && m_ctlbits[idx]);
        m_array[idx].destroy();
        m_size--;
        if constexpr (ASSERT_INVALID) m_ctlbits[idx] = false;
    }

    inline T&       operator[](size_t idx)       { return this->at(idx); }
    inline const T& operator[](size_t idx) const { return this->at(idx); }

    inline T&       at(size_t idx)       { return m_array[idx].as(); }
    inline const T& at(size_t idx) const { return m_array[idx].as(); }

    inline bool empty() const { return m_size == 0; }

    inline unarray(): m_size(0) {}
    inline ~unarray() { this->clear(); }

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
    struct ET {};

    size_t m_size;
    DT m_array[N];
    std::conditional_t<ASSERT_INVALID,std::bitset<N>,ET> m_ctlbits;
};
};
