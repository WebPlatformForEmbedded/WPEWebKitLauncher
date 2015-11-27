#include <WPE/WebKit.h>

#include <cstdio>

extern "C"
void WKBundleInitialize(WKBundleRef, WKTypeRef)
{
    fprintf(stderr, "[WPEInjectedBundle] Initialized.\n");
}
