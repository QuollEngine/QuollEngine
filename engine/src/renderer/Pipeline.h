#pragma once

namespace liquid {

class Pipeline {
public:
  /**
   * @brief Virtual destructor
   */
  virtual ~Pipeline() = default;

  /**
   * @brief Default constructor
   */
  Pipeline() = default;

  Pipeline(const Pipeline &) = delete;
  Pipeline(Pipeline &&) = delete;
  Pipeline &operator=(const Pipeline &) = delete;
  Pipeline &operator=(Pipeline &&) = delete;
};

} // namespace liquid
