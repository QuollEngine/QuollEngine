#pragma once

namespace qui::concepts {

template <typename T>
concept is_vector = requires { typename T::value_type; } &&
                    std::is_same_v<T, std::vector<typename T::value_type>>;

} // namespace qui::concepts
