#pragma once

#include "quoll/qui/component/BuildContext.h"
#include "quoll-tests/Testing.h"

class QuiComponentTest : public ::testing::Test {
public:
  qui::EventManager eventManager;
  qui::BuildContext buildContext{&eventManager};
};
