#pragma once

namespace quoll {

/**
 * @brief Input data type
 */
enum InputDataType { Boolean, Axis1d, Axis2d, Axis3d };

/**
 * @brief Input data type field
 *
 * Used to identify what part of data
 * the key represents
 *
 * `Value` parameter represents the object itself
 */
enum InputDataTypeField { Value, X, Y, X0, X1, Y0, Y1 };

} // namespace quoll
