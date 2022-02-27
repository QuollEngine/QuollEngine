#pragma once

namespace liquid::entity_utils {

template <class T, class... Types>
constexpr bool are_types_unique =
    (!std::is_same_v<T, Types> && ...) && are_types_unique<Types...>;

template <class T> constexpr bool are_types_unique<T> = std::true_type{};

} // namespace liquid::entity_utils
