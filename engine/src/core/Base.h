#pragma once

#include <iostream>
#include <algorithm>
#include <filesystem>
#include <sstream>
#include <string>
#include <exception>
#include <optional>
#include <array>
#include <set>
#include <limits>
#include <fstream>
#include <memory>
#include <vector>
#include <chrono>
#include <ctime>
#include <functional>
#include <iomanip>
#include <any>
#include <map>
#include <unordered_map>
#include <memory>
#include <list>
#include <type_traits>
#include <variant>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Assert.h"

namespace liquid {

using String = std::string;

template <class T> using SharedPtr = std::shared_ptr<T>;

} // namespace liquid
