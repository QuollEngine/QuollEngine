#pragma once

namespace liquid {

class LuaTable {
public:
  LuaTable(void *data, uint32_t size);

  void set(const char *label, uint32_t value);

private:
  uint32_t mSize = 0;
  void *mScope = nullptr;
};

} // namespace liquid
