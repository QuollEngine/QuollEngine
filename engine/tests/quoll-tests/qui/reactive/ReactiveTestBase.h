#pragma once

#include "quoll/qui/reactive/Scope.h"
#include "quoll-tests/Testing.h"

class ReactiveTestBase : public ::testing::Test {
public:
  using ObserverFn = ::testing::MockFunction<void()>;

public:
  qui::Scope scope;
};
