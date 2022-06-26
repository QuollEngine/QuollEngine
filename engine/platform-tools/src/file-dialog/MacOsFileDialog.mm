#include <AppKit/AppKit.h>
#include "NativeFileDialog.h"

namespace liquid::platform_tools {

liquid::Path NativeFileDialog::getFilePathFromDialog(const std::vector<FileTypeEntry> &fileTypes) {
    // TODO: Fix this in macOS
    std::vector<NSString *> nssExtensions;

    for (auto &fileType : fileTypes) {
        for (auto &ext : fileType.extensions) {
            nssExtensions.push_back([NSString stringWithUTF8String:ext.c_str()]);
        }
    }

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

liquid::Path 
NativeFileDialog::getFilePathFromCreateDialog(const std::vector<FileTypeEntry> &fileTypes) {
    LIQUID_ASSERT(false, "Not implemented");
    return "";
}

} // namespace liquid::platform_tools
