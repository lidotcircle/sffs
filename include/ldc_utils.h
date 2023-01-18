#pragma once
#include <assert.h>
#include <cstdint>
#include <type_traits>


#define LDC_MEMBER_FUNCTION_CALLABLE(QUALIFIERS, FUNCN, RET, ...) \
    template<typename ... LDC_Args> \
    struct LDC_MEMBER_FUNCTION_CALLABLE_TEST_##FUNCN { \
        template<typename LDC_U> \
        static uint8_t  test(...); \
        template<typename LDC_U,std::enable_if_t<std::is_same_v<RET,decltype(std::declval<LDC_U QUALIFIERS>().FUNCN(std::declval<LDC_Args>()...))>,bool> = true> \
        static uint16_t test(int); \
    }; \
    template<typename LDC_T> \
    static constexpr bool LDC_MEMBER_FUNCTION_CALLABLE_TEST_VALUE_##FUNCN = sizeof(LDC_MEMBER_FUNCTION_CALLABLE_TEST_##FUNCN<__VA_ARGS__>::template test<LDC_T>(1)) == sizeof(uint16_t)

#define LDC_MEMBER_FUNCTION_CALLABLE_VALUE(TYPEN, FUNCN) LDC_MEMBER_FUNCTION_CALLABLE_TEST_VALUE_##FUNCN<TYPEN>
#define LDC_MEMBER_FUNCTION_CALLABLE_DEFINE_STATIC_CONSTEXPR(var, TYPEN, QUALIFIERS, FUNCN, RET, ...) \
    LDC_MEMBER_FUNCTION_CALLABLE(QUALIFIERS, FUNCN, RET, __VA_ARGS__); \
    constexpr static bool var = LDC_MEMBER_FUNCTION_CALLABLE_VALUE(TYPEN, FUNCN)
#define LDC_MEMBER_FUNCTION_CALLABLE_DEFINE_STATIC_CONSTEXPR_AUTONAME(TYPEN, QUALIFIERS, FUNCN, RET, ...) \
    LDC_MEMBER_FUNCTION_CALLABLE_DEFINE_STATIC_CONSTEXPR(has_##FUNCN, TYPEN, QUALIFIERS, FUNCN, RET, __VA_ARGS__)


#define LDC_STATIC_FUNCTION_CALLABLE(FUNCN, RET, ...) \
    template<typename ... LDC_Args> \
    struct LDC_STATIC_FUNCTION_CALLABLE_TEST_##FUNCN { \
        template<typename LDC_U> \
        static uint8_t  test(...); \
        template<typename LDC_U,std::enable_if_t<std::is_same_v<RET,decltype(LDC_U::FUNCN(std::declval<LDC_Args>()...))>,bool> = true> \
        static uint16_t test(int); \
    }; \
    template<typename LDC_T> \
    static constexpr bool LDC_STATIC_FUNCTION_CALLABLE_TEST_VALUE_##FUNCN = sizeof(LDC_STATIC_FUNCTION_CALLABLE_TEST_##FUNCN<__VA_ARGS__>::template test<LDC_T>(1)) == sizeof(uint16_t)

#define LDC_STATIC_FUNCTION_CALLABLE_VALUE(TYPEN, FUNCN) LDC_STATIC_FUNCTION_CALLABLE_TEST_VALUE_##FUNCN<TYPEN>
#define LDC_STATIC_FUNCTION_CALLABLE_DEFINE_STATIC_CONSTEXPR(var, TYPEN, FUNCN, RET, ...) \
    LDC_STATIC_FUNCTION_CALLABLE(FUNCN, RET, __VA_ARGS__); \
    constexpr static bool var = LDC_STATIC_FUNCTION_CALLABLE_VALUE(TYPEN, FUNCN)
#define LDC_STATIC_FUNCTION_CALLABLE_DEFINE_STATIC_CONSTEXPR_AUTONAME(TYPEN, FUNCN, RET, ...) \
    LDC_STATIC_FUNCTION_CALLABLE_DEFINE_STATIC_CONSTEXPR(has_##FUNCN, TYPEN, FUNCN, RET, __VA_ARGS__)

#define LDC_OBJECT_CALLABLE(UNIQUENAME, QUALIFIERS, RET, ...) \
    template<typename ... LDC_Args> \
    struct LDC_OBJECT_CALLABLE_TEST_##UNIQUENAME { \
        template<typename LDC_U> \
        static uint8_t  test(...); \
        template<typename LDC_U,std::enable_if_t<std::is_same_v<RET,decltype(std::declval<LDC_U QUALIFIERS>()(std::declval<LDC_Args>()...))>,bool> = true> \
        static uint16_t test(int); \
    }; \
    template<typename LDC_T> \
    static constexpr bool LDC_OBJECT_CALLABLE_TEST_VALUE_##UNIQUENAME = sizeof(LDC_OBJECT_CALLABLE_TEST_##UNIQUENAME<__VA_ARGS__>::template test<LDC_T>(1)) == sizeof(uint16_t)

#define LDC_OBJECT_CALLABLE_VALUE(UNIQUENAME, TYPEN) LDC_OBJECT_CALLABLE_TEST_VALUE_##UNIQUENAME<TYPEN>


// TODO confused about member and static
#define LDC_MEMBER_VARIABLE(VARN, VARTYPE) \
    struct LDC_MEMBER_VARIABLE_TEST_##VARN { \
        template<typename LDC_U> \
        static uint8_t  test(...); \
        template<typename LDC_U,std::enable_if_t<std::is_same_v<VARTYPE,decltype(std::declval<LDC_U&>().VARN)>,bool> = true> \
        static uint16_t test(int); \
    }; \
    template<typename LDC_T> \
    static constexpr bool LDC_MEMBER_VARIABLE_TEST_VALUE_##VARN = sizeof(LDC_MEMBER_VARIABLE_TEST_##VARN::template test<LDC_T>(1)) == sizeof(uint16_t)

#define LDC_MEMBER_VARIABLE_VALUE(TYPEN, VARN) LDC_MEMBER_VARIABLE_TEST_VALUE_##VARN<TYPEN>


#define LDC_STATIC_VARIABLE(VARN, VARTYPE) \
    struct LDC_STATIC_VARIABLE_TEST_##VARN { \
        template<typename LDC_U> \
        static uint8_t  test(...); \
        template<typename LDC_U,std::enable_if_t<std::is_same_v<VARTYPE,decltype(LDC_U::VARN)>,bool> = true> \
        static uint16_t test(int); \
    }; \
    template<typename LDC_T> \
    static constexpr bool LDC_STATIC_VARIABLE_TEST_VALUE_##VARN = \
        sizeof(LDC_STATIC_VARIABLE_TEST_##VARN::template test<LDC_T>(1)) == sizeof(uint16_t)

#define LDC_STATIC_VARIABLE_VALUE(TYPEN, VARN) LDC_STATIC_VARIABLE_TEST_VALUE_##VARN<TYPEN>


namespace ldc {
struct dummy_struct {};

inline void unreachable() { assert(false && "unreachable"); }

};
