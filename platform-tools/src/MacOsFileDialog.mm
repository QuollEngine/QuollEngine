#include <AppKit/AppKit.h>
#include "platform-tools/NativeFileDialog.h"

namespace liquid::platform_tools {

liquid::String NativeFileDialog::getFilePathFromDialog(const std::vector<liquid::String> &extensions) {
    std::vector<NSString *> nssExtensions(extensions.size());
    std::transform(extensions.begin(), extensions.end(), nssExtensions.begin(), [](const liquid::String &ext) {
        return [NSString stringWithUTF8String:ext.c_str()];
    });
    NSOpenPanel *fileDialog = [NSOpenPanel openPanel];
    fileDialog.canChooseDirectories = false;
    fileDialog.allowsMultipleSelection = false;
    NSArray *arrayExtensions = [NSArray arrayWithObjects:&nssExtensions[0] count:nssExtensions.size()];
    fileDialog.allowedFileTypes = arrayExtensions;
    
    liquid::String filename;
    
    NSModalResponse result = [fileDialog runModal];
    if (result == NSModalResponseOK){
        NSURL *nsFilenameUrl = [fileDialog URL];
        NSString *nsFilename = [nsFilenameUrl path];
        filename = liquid::String([nsFilename UTF8String]);
    }
    
    return filename;
}

std::filesystem::path
NativeFileDialog::getFilePathFromCreateDialog(const std::vector<liquid::String> &extensions) {
    LIQUID_ASSERT(false, "Not implemented");
    return "";
}

} // namespace liquid::platform_tools
