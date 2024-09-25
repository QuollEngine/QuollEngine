#pragma once

namespace qui::concepts {

template <typename T>
concept is_vector = requires { typename T::value_type; } &&
                    std::is_same_v<T, std::vector<typename T::value_type>>;

template <typename Func, typename Ret, typename... Args>
concept invocable_returns =
    std::invocable<Func, Args...> &&
    std::same_as<std::invoke_result_t<Func, Args...>, Ret>;

} // namespace qui::concepts

namespace qui::traits {

// Function
template <typename T> struct function_traits;

template <typename Ret, typename... Args>
struct function_traits<Ret (*)(Args...)> {
  using ReturnType = Ret;
  using ArgTypes = std::tuple<Args...>;
};

template <typename Ret, typename Class, typename... Args>
struct function_traits<Ret (Class::*)(Args...)> {
  using ReturnType = Ret;
  using ArgTypes = std::tuple<Args...>;
};

template <typename Ret, typename Class, typename... Args>
struct function_traits<Ret (Class::*)(Args...) const> {
  using ReturnType = Ret;
  using ArgTypes = std::tuple<Args...>;
};

template <typename FunctorOrLambda> struct function_traits {
  using ReturnType = decltype(&FunctorOrLambda::operator());
  using ArgTypes = typename function_traits<ReturnType>::ArgTypes;
};

// Tuple
template <typename Tuple, typename T> struct tuple_is_first_same {
  static constexpr bool value = false;
};

template <typename First, typename... Rest, typename T>
struct tuple_is_first_same<std::tuple<First, Rest...>, T> {
  using FirstWithoutRef = std::remove_cv_t<std::remove_reference_t<First>>;
  static constexpr bool value = std::is_same_v<FirstWithoutRef, T>;
};

template <typename T, typename... Rest>
static constexpr bool tuple_is_first_same_v =
    tuple_is_first_same<T, Rest...>::value;

template <typename Tuple, typename T> struct tuple_remove_first_if_matches;

template <typename First, typename... Rest, typename T>
struct tuple_remove_first_if_matches<std::tuple<First, Rest...>, T> {
  using FirstWithoutRef = std::remove_cv_t<std::remove_reference_t<First>>;
  static constexpr bool matches = std::is_same_v<FirstWithoutRef, T>;
  using type = std::conditional_t<matches, std::tuple<Rest...>,
                                  std::tuple<First, Rest...>>;
};

template <typename T> struct tuple_remove_first_if_matches<std::tuple<>, T> {
  using type = std::tuple<>;
};

template <typename Tuple, typename X>
using tuple_remove_first_if_matches_type_t =
    typename tuple_remove_first_if_matches<Tuple, X>::type;

} // namespace qui::traits
