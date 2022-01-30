#include "liquid/core/Base.h"
#include "KtxError.h"

// Reference:
// https://www.khronos.org/ktx/documentation/libktx/ktx_8h.html#a8de6927e772cc95a9f49593c3dd72069
static const std::map<ktx_error_code_e, liquid::String> resultMap{
    // Success codes
    {KTX_SUCCESS, "Operation successfully completed"},
    {KTX_FILE_DATA_ERROR, "The data in the file is inconsistent with the spec"},
    {KTX_FILE_ISPIPE, "The file is a pipe or named pipe"},
    {KTX_FILE_OPEN_FAILED, "File could not be opened"},
    {KTX_FILE_OVERFLOW, "Operation exceeds maximum file size"},
    {KTX_FILE_READ_ERROR, "Failed to read from file"},
    {KTX_FILE_SEEK_ERROR, "Failed to seek from file"},
    {KTX_FILE_UNEXPECTED_EOF, "File is not complete"},
    {KTX_FILE_WRITE_ERROR, "Failed to write to file"},
    {KTX_INVALID_OPERATION, "Invalid operation"},
    {KTX_INVALID_VALUE, "Invalid parameter value"},
    {KTX_NOT_FOUND, "Requested key not found"},
    {KTX_OUT_OF_MEMORY, "Not enough memory to complete the operation"},
    {KTX_TRANSCODE_FAILED, "Transcoding block compressed texture failed"},
    {KTX_UNKNOWN_FILE_FORMAT, "File is not in KTX format"},
    {KTX_UNSUPPORTED_TEXTURE_TYPE, "Texture type not supported"},
    {KTX_UNSUPPORTED_FEATURE, "Feature not supported"},
};

namespace liquid {

static String createErrorMessage(const String &what,
                                 ktx_error_code_e resultCode) {
  String errorMessage = "[KtxError] " + what;
  if (resultCode == KTX_SUCCESS) {
    return errorMessage;
  }

  const auto &codeString = "(code: " + std::to_string(resultCode) + ")";
  const auto &it = resultMap.find(resultCode);
  const auto &humanReadableResultString =
      it != resultMap.end() ? (*it).second : "Unknown Error";

  return errorMessage + ": " + humanReadableResultString + " " + codeString;
}

KtxError::KtxError(const String &what, ktx_error_code_e resultCode)
    : std::runtime_error(createErrorMessage(what, resultCode)){};

} // namespace liquid
