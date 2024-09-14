#ifndef QUOLL_CORE_BASE_H_
#define QUOLL_CORE_BASE_H_

#ifdef QUOLL_DEBUG
#ifndef _DEBUG
#define _DEBUG
#endif
#undef NDEBUG
#else
#ifndef NDEBUG
#define NDEBUG
#endif
#undef _DEBUG
#endif

#include <algorithm>
#include <any>
#include <array>
#include <chrono>
#include <ctime>
#include <exception>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <random>
#include <set>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <variant>
#include <vector>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>

namespace quoll {

using String = std::string;
using StringView = std::string_view;
using Path = std::filesystem::path;

template <class T> using SharedPtr = std::shared_ptr<T>;

} // namespace quoll

#include "Assert.h"
#include "BitwiseEnum.h"
#include "DataTypes.h"
#include "NoCopyMove.h"
#include "Uuid.h"

#endif
