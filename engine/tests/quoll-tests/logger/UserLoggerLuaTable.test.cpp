#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/logger/NoopLogTransport.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

using LS = quoll::LogSeverity;

struct Output {
  quoll::LogSeverity severity;
  quoll::LogTimestamp timestamp;
  quoll::String message;
};

class UserLoggerLuaTableTest
    : public LuaScriptingInterfaceTestBase,
      public testing::WithParamInterface<std::tuple<quoll::String, LS>> {
public:
  UserLoggerLuaTableTest()
      : LuaScriptingInterfaceTestBase("scripting-system-logging-tester.lua") {}

  void SetUp() override {
    LuaScriptingInterfaceTestBase::SetUp();

    quoll::Engine::getUserLogger().setTransport(
        [this](auto severity, auto timestamp, auto message) {
          output.severity = severity;
          output.timestamp = timestamp;
          output.message = message;
        });
    quoll::Engine::getUserLogger().setMinSeverity(quoll::LogSeverity::Debug);
  }

  void TearDown() override {
    LuaScriptingInterfaceTestBase::TearDown();

    quoll::Engine::getUserLogger().setTransport(quoll::NoopLogTransport);
  }

  Output output;
};

TEST_P(UserLoggerLuaTableTest, LogsMessageOnCall) {
  auto [value, severity] = GetParam();

  auto entity = entityDatabase.entity();
  call(entity, value);

  EXPECT_THAT(output.message,
              ::testing::MatchesRegex(
                  "Hello world\t12\ttrue\tnil\tfunction: \\S+\ttable: \\S+\t"));
  EXPECT_EQ(output.severity, severity);
}

INSTANTIATE_TEST_SUITE_P(
    UserLoggerLuaTableTest, UserLoggerLuaTableTest,
    ::testing::Values(std::make_tuple("stdPrint", LS::Debug),
                      std::make_tuple("debug", LS::Debug),
                      std::make_tuple("info", LS::Info),
                      std::make_tuple("warning", LS::Warning),
                      std::make_tuple("error", LS::Error),
                      std::make_tuple("fatal", LS::Fatal)),
    [](const ::testing::TestParamInfo<UserLoggerLuaTableTest::ParamType>
           &info) { return std::get<0>(info.param); });
