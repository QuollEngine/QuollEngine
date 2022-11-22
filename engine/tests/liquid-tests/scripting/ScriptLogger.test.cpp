#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"
#include "liquid/logger/NoopLogTransport.h"

#include "liquid-tests/Testing.h"
#include "liquid-tests/test-utils/ScriptingInterfaceTestBase.h"

using ::testing::_;

using LS = liquid::LogSeverity;

class ScriptLoggerTest
    : public LuaScriptingInterfaceTestBase,
      public testing::WithParamInterface<std::tuple<liquid::String, LS>> {
public:
  ScriptLoggerTest()
      : LuaScriptingInterfaceTestBase("scripting-system-logging-tester.lua") {}

  void SetUp() override {
    liquid::Engine::getUserLogger().setTransport(
        [this](auto severity, auto timestamp, auto message) {
          mockTransport(severity, timestamp, message);
        });
  }

  void TearDown() override {
    liquid::Engine::getUserLogger().setTransport(liquid::NoopLogTransport);
  }

  MOCK_METHOD(void, mockTransport,
              (liquid::LogSeverity, liquid::LogTimestamp, liquid::String));
};

TEST_P(ScriptLoggerTest, DoesNothingIfProvidedArgumentIsInvalid) {
  auto [value, severity] = GetParam();

  EXPECT_CALL(*this, mockTransport(severity, _, "Hello world")).Times(0);

  auto entity = entityDatabase.create();
  auto &scope = call(entity, value + "_invalid");
}

TEST_P(ScriptLoggerTest, LogsMessageOnCall) {
  auto [value, severity] = GetParam();

  EXPECT_CALL(*this, mockTransport(severity, _, "Hello world")).Times(1);

  auto entity = entityDatabase.create();
  auto &scope = call(entity, value);
}

INSTANTIATE_TEST_SUITE_P(
    ScriptLoggerSuite, ScriptLoggerTest,
    ::testing::Values(std::make_tuple("debug", LS::Debug),
                      std::make_tuple("info", LS::Info),
                      std::make_tuple("warning", LS::Warning),
                      std::make_tuple("error", LS::Error),
                      std::make_tuple("fatal", LS::Fatal)),
    [](const ::testing::TestParamInfo<ScriptLoggerTest::ParamType> &info) {
      return std::get<0>(info.param);
    });
