#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/logger/NoopLogTransport.h"

#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

using ::testing::_;

using LS = quoll::LogSeverity;

class ScriptLoggerTest
    : public LuaScriptingInterfaceTestBase,
      public testing::WithParamInterface<std::tuple<quoll::String, LS>> {
public:
  ScriptLoggerTest()
      : LuaScriptingInterfaceTestBase("scripting-system-logging-tester.lua") {}

  void SetUp() override {
    quoll::Engine::getUserLogger().setTransport(
        [this](auto severity, auto timestamp, auto message) {
          mockTransport(severity, timestamp, message);
        });
  }

  void TearDown() override {
    quoll::Engine::getUserLogger().setTransport(quoll::NoopLogTransport);
  }

  MOCK_METHOD(void, mockTransport,
              (quoll::LogSeverity, quoll::LogTimestamp, quoll::String));
};

TEST_P(ScriptLoggerTest, LogsErrorIfNoArgumentIsProvided) {
  auto [value, severity] = GetParam();

  EXPECT_CALL(*this, mockTransport(quoll::LogSeverity::Error, _, _)).Times(5);

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
