#pragma once

#include "quoll/qui/component/Component.h"
#include "MockView.h"

class MockComponent : public qui::Component {
public:
  MockComponent(u32 value) : value(value) {}

  void build() override {
    mView.value = value;
    buildCount++;
  }

  qui::View *getView() override { return &mView; }

  u32 value;
  MockView mView;

  u32 buildCount = 0;
};
