#pragma once
#include <assert.h>

#include <algorithm>

#define LDC_CLASS_MEMBER_TEST(QUALIFIERS, MEMBER, PROTO)                    \
    struct LDC_CLASS_MEMBER_TEST_##MEMBER {                                 \
        template <typename LDC_PROTO>                                       \
        struct CHECK {                                                      \
            template <typename LDC_U>                                       \
            static uint8_t test(...);                                       \
                                                                            \
            template <                                                      \
                typename LDC_U,                                             \
                std::enable_if_t<                                           \
                    std::is_same_v<                                         \
                        LDC_PROTO,                                          \
                        decltype(std::declval<LDC_U QUALIFIERS>().MEMBER)>, \
                    bool> = true>                                           \
            static uint16_t test(int);                                      \
        };                                                                  \
                                                                            \
        template <typename LDC_Ret, typename... LDC_Args>                   \
        struct CHECK<LDC_Ret(LDC_Args...)> {                                \
            template <typename LDC_U>                                       \
            static uint8_t test(...);                                       \
                                                                            \
            template <                                                      \
                typename LDC_U,                                             \
                std::enable_if_t<                                           \
                    std::is_same_v<                                         \
                        LDC_Ret,                                            \
                        decltype(std::declval<LDC_U QUALIFIERS>().MEMBER(   \
                            std::declval<LDC_Args>()...))> ||               \
                        std::is_same_v<LDC_Ret, ldc::AnyType>,              \
                    bool> = true>                                           \
            static uint16_t test(int);                                      \
        };                                                                  \
    };                                                                      \
    template <typename LDC_T>                                               \
    static constexpr bool LDC_CLASS_MEMBER_TEST_VALUE_##MEMBER =            \
        sizeof(LDC_CLASS_MEMBER_TEST_##MEMBER::template CHECK<              \
               PROTO>::template test<LDC_T>(std::declval<int>())) ==        \
        sizeof(uint16_t)

#define LDC_CLASS_MEMBER_TEST_VALUE(TYPENAME, MEMBER) \
    LDC_CLASS_MEMBER_TEST_VALUE_##MEMBER<TYPENAME>
#define LDC_CLASS_MEMBER_TEST_VALUE_AUTONAME(TYPENAME, QUALIFIERS, MEMBER, \
                                             PROTO)                        \
    LDC_CLASS_MEMBER_TEST(QUALIFIERS, MEMBER, PROTO);                      \
    constexpr static bool has_##MEMBER =                                   \
        LDC_CLASS_MEMBER_TEST_VALUE(TYPENAME, MEMBER)

#define LDC_CLASS_STATIC_MEMBER_TEST(MEMBER, PROTO)                            \
    struct LDC_CLASS_STATIC_MEMBER_TEST_##MEMBER {                             \
        template <typename LDC_PROTO>                                          \
        struct CHECK {                                                         \
            template <typename LDC_U>                                          \
            static uint8_t test(...);                                          \
                                                                               \
            template <typename LDC_U,                                          \
                      std::enable_if_t<                                        \
                          std::is_same_v<LDC_PROTO, decltype(LDC_U::MEMBER)>,  \
                          bool> = true>                                        \
            static uint16_t test(int);                                         \
        };                                                                     \
                                                                               \
        template <typename LDC_Ret, typename... LDC_Args>                      \
        struct CHECK<LDC_Ret(LDC_Args...)> {                                   \
            template <typename LDC_U>                                          \
            static uint8_t test(...);                                          \
                                                                               \
            template <typename LDC_U,                                          \
                      std::enable_if_t<                                        \
                          std::is_same_v<LDC_Ret,                              \
                                         decltype(LDC_U::MEMBER(               \
                                             std::declval<LDC_Args>()...))> || \
                              std::is_same_v<LDC_Ret, ldc::AnyType>,           \
                          bool> = true>                                        \
            static uint16_t test(int);                                         \
        };                                                                     \
    };                                                                         \
    template <typename LDC_T>                                                  \
    static constexpr bool LDC_CLASS_STATIC_MEMBER_TEST_VALUE_##MEMBER =        \
        sizeof(LDC_CLASS_STATIC_MEMBER_TEST_##MEMBER::template CHECK<          \
               PROTO>::template test<LDC_T>(std::declval<int>())) ==           \
        sizeof(uint16_t)

#define LDC_CLASS_STATIC_MEMBER_TEST_VALUE(TYPENAME, MEMBER) \
    LDC_CLASS_STATIC_MEMBER_TEST_VALUE_##MEMBER<TYPENAME>
#define LDC_CLASS_STATIC_MEMBER_TEST_VALUE_AUTONAME(TYPENAME, MEMBER, PROTO) \
    LDC_CLASS_STATIC_MEMBER_TEST(MEMBER, PROTO);                             \
    constexpr static bool has_##MEMBER =                                     \
        LDC_CLASS_STATIC_MEMBER_TEST_VALUE(TYPENAME, MEMBER)

namespace ldc {
struct dummy_struct {};

struct NonExistsType {};
struct AnyType {};

inline void unreachable() { assert(false && "unreachable"); }

template <typename FUNC>
struct deferred_call {
    deferred_call(const deferred_call& that) = delete;
    deferred_call& operator=(const deferred_call& that) = delete;

    explicit deferred_call(FUNC&& f)
        : m_func(std::forward<FUNC>(f)), m_bOwner(true) {}

    deferred_call(deferred_call&& that)
        : m_func(std::move(that.m_func)), m_bOwner(that.m_bOwner) {
        that.m_bOwner = false;
    }

    ~deferred_call() { execute(); }

    bool cancel() {
        bool bWasOwner = m_bOwner;
        m_bOwner = false;
        return bWasOwner;
    }

    bool execute() {
        const auto bWasOwner = m_bOwner;

        if (m_bOwner) {
            m_bOwner = false;
            m_func();
        }

        return bWasOwner;
    }

private:
    FUNC m_func;
    bool m_bOwner;
};

/**
 * !!! be cautious about lifetime of return value
 */
template <typename F>
[[nodiscard]] deferred_call<F> defer(F&& f) {
    return deferred_call<F>(std::forward<F>(f));
}

};  // namespace ldc
