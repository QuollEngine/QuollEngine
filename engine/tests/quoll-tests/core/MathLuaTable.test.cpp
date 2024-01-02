#include "quoll/core/Base.h"
#include "quoll/core/Name.h"

#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class MathLuaTableTestBase : public LuaScriptingInterfaceTestBase {
public:
  MathLuaTableTestBase() : LuaScriptingInterfaceTestBase("math-test.lua") {}
};

// Vector3
using Vector3LuaTableTest = MathLuaTableTestBase;

TEST_F(Vector3LuaTableTest, NewCreatesVector3) {
  auto entity = entityDatabase.create();
  call(entity, "vector3New");
}

// Quaternion
using QuaternionLuaTableTest = MathLuaTableTestBase;

TEST_F(QuaternionLuaTableTest, NewCreatesQuaternion) {
  auto entity = entityDatabase.create();
  call(entity, "quaternionNew");
}

TEST_F(QuaternionLuaTableTest,
       FromEulerAngleCreatesQuaternionFromEulerAngleXYZ) {
  auto entity = entityDatabase.create();
  call(entity, "quaternionFromEulerAngles");
}

TEST_F(QuaternionLuaTableTest, ToEulerAngleReturnsEulerAnglesInXYZ) {
  auto entity = entityDatabase.create();
  call(entity, "quaternionToEulerAngles");
}
