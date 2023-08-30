#pragma once

namespace quoll::entity_utils {

template <class T, class... Types>
constexpr bool AreTypesUnique =
    (!std::is_same_v<T, Types> && ...) && AreTypesUnique<Types...>;

template <class T> constexpr bool AreTypesUnique<T> = std::true_type{};

} // namespace quoll::entity_utils
