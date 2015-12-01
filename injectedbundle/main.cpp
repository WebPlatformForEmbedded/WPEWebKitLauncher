#include <WPE/WebKit.h>

#include <cstdio>
#include <memory>
#include <string>

std::string toSTDString(WKStringRef string)
{
    size_t bufferSize = WKStringGetMaximumUTF8CStringSize(string);
    std::unique_ptr<char[]> buffer(new char[bufferSize]);
    size_t stringLength = WKStringGetUTF8CString(string, buffer.get(), bufferSize);
    return std::string(buffer.get(), stringLength - 1);
}

static WKBundleClientV1 s_bundleClient = {
    { 1, nullptr },
    nullptr, // didCreatePage
    nullptr, // willDestroyPage
    nullptr, // didInitializePageGroup
    nullptr, // didReceiveMessage
    // didReceiveMessageToPage
    [](WKBundleRef, WKBundlePageRef, WKStringRef messageName, WKTypeRef messageBodyObj, const void*) {
        if (WKStringIsEqualToUTF8CString(messageName, "Hello")) {
            fprintf(stderr, "[WPEInjectedBundle] Hello!\n");

            WKArrayRef messageBody = static_cast<WKArrayRef>(messageBodyObj);
            for (size_t i = 0, size = WKArrayGetSize(messageBody); i < size; ++i) {
                WKStringRef itemString = static_cast<WKStringRef>(WKArrayGetItemAtIndex(messageBody, i));
                std::string item = toSTDString(itemString);
                fprintf(stderr, "\tarray item: %s\n", item.data());
            }
        }
    },
};

extern "C" {

void WKBundleInitialize(WKBundleRef bundle, WKTypeRef)
{
    fprintf(stderr, "[WPEInjectedBundle] Initialized.\n");

    WKBundleSetClient(bundle, &s_bundleClient.base);
}

}
