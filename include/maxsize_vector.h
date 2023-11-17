#pragma once
#include <utility>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <vector>
#include "./ldc_utils.h"


namespace ldc {
template<typename T, size_t N, bool appendvector=false>
class maxsize_vector {
private:
    template<bool reverse>
    class const_iterator_impl {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;
        using const_reference   = const T&;

    protected:
        const maxsize_vector& m_vec;
        size_t m_idx;

    public:
        const_iterator_impl(const maxsize_vector& vec, size_t idx): m_vec(vec), m_idx(idx) {}

        const T& operator*() const {
            assert(m_vec.size() > m_idx);
            if constexpr (reverse) {
                return m_vec.at(m_vec.size() - m_idx - 1);
            } else {
                return m_vec.at(m_idx);
            }
        }

        const_iterator_impl& operator++() {
            assert(this->m_idx < m_vec.size());
            this->m_idx++;
            return *this;
        }

        const_iterator_impl& operator--() {
            assert(this->m_idx > 0);
            this->m_idx--;
            return *this;
        }

        const_iterator_impl operator++(int) { auto ans = *this; this->operator++(); return ans; }
        const_iterator_impl operator--(int) { auto ans = *this; this->operator--(); return ans; }

        const_iterator_impl& operator+(int n) { assert(m_idx + n <= m_vec.size()); m_idx += n; }
        const_iterator_impl& operator-(int n) { assert(m_idx >= n); m_idx -= n; }

        std::ptrdiff_t operator-(const const_iterator_impl& oth) { std::ptrdiff_t idx = m_idx; idx -= oth.m_idx; return idx; }

        bool operator< (const const_iterator_impl& oth) const { assert(&this->m_vec == &oth.m_vec); return this->m_idx <  oth.m_idx; }
        bool operator<=(const const_iterator_impl& oth) const { assert(&this->m_vec == &oth.m_vec); return this->m_idx <= oth.m_idx; }
        bool operator> (const const_iterator_impl& oth) const { assert(&this->m_vec == &oth.m_vec); return this->m_idx >  oth.m_idx; }
        bool operator>=(const const_iterator_impl& oth) const { assert(&this->m_vec == &oth.m_vec); return this->m_idx >= oth.m_idx; }
        bool operator==(const const_iterator_impl& oth) const { assert(&this->m_vec == &oth.m_vec); return this->m_idx == oth.m_idx; }
        bool operator!=(const const_iterator_impl& oth) const { return !this->operator==(oth); }
    };

    template<bool reverse>
    class iterator_impl: public const_iterator_impl<reverse> {
    public:
        using iterator_category = typename const_iterator_impl<reverse>::iterator_category;
        using difference_type   = typename const_iterator_impl<reverse>::difference_type;
        using value_type        = typename const_iterator_impl<reverse>::value_type;
        using pointer           = typename const_iterator_impl<reverse>::pointer;
        using reference         = typename const_iterator_impl<reverse>::reference;
        using const_reference   = typename const_iterator_impl<reverse>::const_reference;

        iterator_impl(maxsize_vector& vec, size_t idx): const_iterator_impl<reverse>(vec, idx) {}

        T& operator*() { return const_cast<T&>(static_cast<const const_iterator_impl<reverse>*>(this)->operator*()); }
    };

public:
    using iterator               = iterator_impl<false>;
    using reverse_iterator       = iterator_impl<true>;
    using const_iterator         = const_iterator_impl<false>;
    using const_reverse_iterator = const_iterator_impl<true>;

    inline size_t size() const { return this->m_size; }

    inline T&       back() {
        if constexpr (appendvector) {
            if (m_size <= N) {
                return m_array[m_size - 1].as();
            } else {
                return m_appvec.back();
            }
        } else {
            return m_array[m_size - 1].as();
        }
    }
    inline const T& back() const { return const_cast<maxsize_vector*>(this)->back(); }

    inline void clear() {
        if constexpr (appendvector) {
            if (m_size > N) {
                this->m_appvec.clear();
                m_size = N;
            }
        }

        if constexpr (!std::is_trivially_destructible_v<T>) {
            while (!this->empty()) this->pop_back();
        } else {
            m_size = 0;
        }
    }
    template<typename K>
    inline void push_back(K&& val) {
        if constexpr (appendvector) {
            if (m_size < N) {
                m_array[m_size++].construct_with(std::forward<K>(val));
            } else {
                m_appvec.push_back(std::forward<K>(val));
                m_size++;
            }
        } else {
            assert(m_size < N);
            m_array[m_size++].construct_with(std::forward<K>(val));
        }
    }
    template<typename ... Args>
    inline void emplace_back(Args&& ... args) {
        if constexpr (appendvector) {
            if (m_size < N) {
                m_array[m_size++].construct_with(std::forward<Args>(args)...);
            } else {
                m_appvec.emplace_back(std::forward<Args>(args)...);
                m_size++;
            }
        } else {
            assert(m_size < N);
            m_array[m_size++].construct_with(std::forward<Args>(args)...);
        }
    }
    inline void pop_back() {
        assert(m_size > 0);
        m_size--;
        if constexpr (appendvector) {
            if (m_size < N) {
                m_array[m_size].destroy();
            } else {
                assert(m_appvec.size() > 0);
                m_appvec.pop_back();
            }
        } else {
            m_array[m_size].destroy();
        }
    }

    inline T&       operator[](size_t idx) { return this->at(idx); }
    inline const T& operator[](size_t idx) const { return this->at(idx); }

    inline T&       at(size_t idx) {
        assert(idx < m_size);
        if constexpr (appendvector) {
            if (idx < N) {
                return m_array[idx].as();
            } else {
                return m_appvec[idx - N];
            }
        } else {
            return m_array[idx].as();
        }
    }
    inline const T& at(size_t idx) const { return const_cast<maxsize_vector*>(this)->at(idx); }

    inline bool empty() const { return m_size == 0; }

    inline maxsize_vector(std::initializer_list<T> vec): m_size(0) {
        for (auto v: vec)
            this->push_back(v);
    }

    maxsize_vector(maxsize_vector&& oth): m_size(0)
    {
        for (size_t i=0;i<N && i < oth.size();i++)
            this->emplace_back(std::move(oth.at(i)));
        m_appvec = std::move(oth.m_appvec);
        this->m_size = oth.m_size;
        oth.m_size = 0;
    }

    maxsize_vector(const maxsize_vector& oth): m_size(0)
    {
        for (auto& v: oth) this->emplace_back(v);
    }

    maxsize_vector& operator=(maxsize_vector&& oth) {
        this->clear();
        for (size_t i=0;i<N && i < oth.size();i++)
            this->emplace_back(std::move(oth.at(i)));
        m_appvec = std::move(oth.m_appvec);
        this->m_size = oth.m_size;
        return *this;
    }

    maxsize_vector& operator=(const maxsize_vector& oth) {
        this->clear();
        for (auto& v: oth) this->emplace_back(v);
        return *this;
    }

    bool operator==(const maxsize_vector& oth) const {
        if (this->size() != oth.size()) return false;
        for (size_t i=0;i<this->size();i++) {
            if (this->at(i) != oth.at(i)) return false;
        }
        return true;
    }
    bool operator!=(const maxsize_vector& oth) const { return !this->operator==(oth); }

    iterator       begin()        { return iterator(*this, 0); }
    const_iterator begin()  const { return const_iterator(*this, 0); }
    const_iterator cbegin() const { return const_iterator(*this, 0); }
    iterator       end()        { return iterator(*this, m_size); }
    const_iterator end()  const { return const_iterator(*this, m_size); }
    const_iterator cend() const { return const_iterator(*this, m_size); }

    reverse_iterator       rbegin()        { return reverse_iterator(*this, 0); }
    const_reverse_iterator rbegin()  const { return const_reverse_iterator(*this, 0); }
    const_reverse_iterator crbegin() const { return const_reverse_iterator(*this, 0); }
    reverse_iterator       rend()        { return reverse_iterator(*this, m_size); }
    const_reverse_iterator rend()  const { return const_reverse_iterator(*this, m_size); }
    const_reverse_iterator crend() const { return const_reverse_iterator(*this, m_size); }

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
    std::conditional_t<appendvector,std::vector<T>,dummy_struct> m_appvec;
};
};
