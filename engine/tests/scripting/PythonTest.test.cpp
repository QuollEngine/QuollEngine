#include "liquid/core/Base.h"

#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <gtest/gtest.h>

namespace py = pybind11;

class Rectangle {
public:
  Rectangle(float width, float height) : mWidth(width), mHeight(height) {}

  float area() const { return mWidth * mHeight; }

private:
  float mWidth;
  float mHeight;
};

PYBIND11_EMBEDDED_MODULE(testmath, m) {
  py::class_<Rectangle>(m, "Rectangle")
      .def(py::init<float, float>())
      .def("area", &Rectangle::area);
}

TEST(PythonTest, RunsPythonScript) {
  py::scoped_interpreter guard{true, 0, nullptr, true};

  auto pymod = py::module::import("simple-script");
  auto calculateArea = pymod.attr("calculate_area");

  Rectangle rect{2.5f, 5.0f};
  auto area = calculateArea(rect).cast<float>();

  EXPECT_EQ(area, 12.5f);
}
