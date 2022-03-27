#pragma once

namespace liquid::utils {

inline std::vector<char> readFileIntoBuffer(const String &fileName) {
  std::ifstream file(fileName);

  LIQUID_ASSERT(file.good(), "File cannot be opened");

  std::ostringstream ss;
  ss << file.rdbuf();
  const std::string &s = ss.str();
  std::vector<char> bytes(s.begin(), s.end());
  file.close();

  return bytes;
}

} // namespace liquid::utils
