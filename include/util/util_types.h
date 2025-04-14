#include <type_traits>
#include <utility>

template <class T, class EqualTo>
struct has_operator_equal_impl {
        template <class U, class V>
        static auto test(U*) -> decltype(std::declval<U>() == std::declval<V>());
        template <typename, typename>
        static auto test(...) -> std::false_type;

        using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
};

template <class T, class EqualTo = T>
struct has_operator_equal : has_operator_equal_impl<T, EqualTo>::type {};

template <class T, class LessThan>
struct has_operator_less_impl {
        template <class U, class V>
        static auto test(U*) -> decltype(std::declval<U>() < std::declval<V>());
        template <typename, typename>
        static auto test(...) -> std::false_type;

        using type = typename std::is_same<bool, decltype(test<T, LessThan>(0))>::type;
};

template <class T, class LessThan = T>
struct has_operator_less : has_operator_less_impl<T, LessThan>::type {};

template <class T, class MoreThan>
struct has_operator_more_impl {
        template <class U, class V>
        static auto test(U*) -> decltype(std::declval<U>() > std::declval<V>());
        template <typename, typename>
        static auto test(...) -> std::false_type;

        using type = typename std::is_same<bool, decltype(test<T, MoreThan>(0))>::type;
};

template <class T, class MoreThan = T>
struct has_operator_more : has_operator_more_impl<T, MoreThan>::type {};