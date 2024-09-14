#pragma once

namespace quoll {

enum InputDataType { Boolean, Axis1d, Axis2d, Axis3d };

/**
 * Input data type field
 *
 * Used to identify what part of data
 * the key represents
 *
 * `Value` parameter represents the object itself
 */
enum InputDataTypeField { Value, X, Y, X0, X1, Y0, Y1 };

} // namespace quoll
