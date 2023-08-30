#include <AppKit/AppKit.h>
#include "liquid/platform/tools/FileDialog.h"

namespace quoll::platform {

Path FileDialog::getFilePathFromDialog(const std::vector<FileTypeEntry> &fileTypes) {
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
    
    quoll::String filename;
    
    NSModalResponse result = [fileDialog runModal];
    if (result == NSModalResponseOK){
        NSURL *nsFilenameUrl = [fileDialog URL];
        NSString *nsFilename = [nsFilenameUrl path];
        filename = quoll::String([nsFilename UTF8String]);
    }
    
    return filename;
}

Path 
FileDialog::getFilePathFromCreateDialog(const std::vector<FileTypeEntry> &fileTypes) {
    LIQUID_ASSERT(false, "Not implemented");
    return "";
}

} // namespace quoll::platform
