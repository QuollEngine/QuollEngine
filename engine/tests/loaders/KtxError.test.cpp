#include "core/Base.h"
#include "loaders/KtxError.h"

#include <gtest/gtest.h>

TEST(KtxErrorTest, ShowsErrorMessageWithoutCodeIfSuccess) {
  liquid::KtxError error("Test error message", KTX_SUCCESS);

  EXPECT_EQ(liquid::String(error.what()), "[KtxError] Test error message");
}

TEST(KtxErrorTest, ShowsErrorMessageWithCodeAndExplanationIfCodeExists) {
  liquid::KtxError error("Test error message", KTX_INVALID_OPERATION);

  EXPECT_EQ(liquid::String(error.what()),
            "[KtxError] Test error message: Invalid operation (code: 10)");
}

TEST(KtxErrorTest,
     ShowsErrorMessageWithCodeWithUnknownErrorIfCodeDoesNotExist) {
  liquid::KtxError error("Test error message", (ktx_error_code_e)12345);

  EXPECT_EQ(liquid::String(error.what()),
            "[KtxError] Test error message: Unknown Error (code: 12345)");
}
