#include "liquid/core/Base.h"
#include "liquid/core/Errorable.h"

#include "liquid-tests/Testing.h"

struct Data {
  uint32_t value = 25;
};

enum class Error { None = 0, InvalidNumber = 1, MaximumReached = 2 };

TEST(ErrorableTest, NoErrorIfDataIsPassed) {
  quoll::Errorable<Data, Error> errorable(Data{45});

  EXPECT_TRUE(errorable.hasResult());
  EXPECT_FALSE(errorable.hasError());
  EXPECT_EQ(errorable.getError(), Error::None);
  EXPECT_EQ(errorable.getResult().value, 45);
}

TEST(ErrorableTest, HasErrorIfErrorIsPassed) {
  quoll::Errorable<Data, Error> errorable(Error::MaximumReached);

  EXPECT_FALSE(errorable.hasResult());
  EXPECT_TRUE(errorable.hasError());
  EXPECT_EQ(errorable.getError(), Error::MaximumReached);
  EXPECT_THROW({ errorable.getResult(); }, std::bad_optional_access);
}
