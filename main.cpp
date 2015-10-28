#include <WPE/WebKit.h>
#include <bcm_host.h>
#include <glib.h>

WKPageNavigationClientV0 s_navigationClient = {
    { 0, nullptr },
    // decidePolicyForNavigationAction
    [](WKPageRef, WKNavigationActionRef, WKFramePolicyListenerRef listener, WKTypeRef, const void*) {
        WKFramePolicyListenerUse(listener);
    },
    // decidePolicyForNavigationResponse
    [](WKPageRef, WKNavigationResponseRef, WKFramePolicyListenerRef listener, WKTypeRef, const void*) {
        WKFramePolicyListenerUse(listener);
    },
    nullptr, // decidePolicyForPluginLoad
    nullptr, // didStartProvisionalNavigation
    nullptr, // didReceiveServerRedirectForProvisionalNavigation
    nullptr, // didFailProvisionalNavigation
    nullptr, // didCommitNavigation
    nullptr, // didFinishNavigation
    nullptr, // didFailNavigation
    nullptr, // didFailProvisionalLoadInSubframe
    nullptr, // didFinishDocumentLoad
    nullptr, // didSameDocumentNavigation
    nullptr, // renderingProgressDidChange
    nullptr, // canAuthenticateAgainstProtectionSpace
    nullptr, // didReceiveAuthenticationChallenge
    nullptr, // webProcessDidCrash
    nullptr, // copyWebCryptoMasterKey
    nullptr, // didBeginNavigationGesture
    nullptr, // willEndNavigationGesture
    nullptr, // didEndNavigationGesture
    nullptr, // didRemoveNavigationGestureSnapshot
};

int main(int argc, char* argv[])
{
    bcm_host_init();

    GMainLoop* loop = g_main_loop_new(nullptr, FALSE);

    WKContextRef context = WKContextCreate();

    auto pageGroupIdentifier = WKStringCreateWithUTF8CString("WPEPageGroup");
    auto pageGroup = WKPageGroupCreateWithIdentifier(pageGroupIdentifier);
    WKRelease(pageGroupIdentifier);

    auto preferences = WKPreferencesCreate();
    // Allow mixed content.
    WKPreferencesSetAllowRunningOfInsecureContent(preferences, true);
    WKPreferencesSetAllowDisplayOfInsecureContent(preferences, true);
    WKPageGroupSetPreferences(pageGroup, preferences);

    auto pageConfiguration  = WKPageConfigurationCreate();
    WKPageConfigurationSetContext(pageConfiguration, context);
    WKPageConfigurationSetPageGroup(pageConfiguration, pageGroup);

    auto view = WKViewCreate(pageConfiguration);

    uint32_t width = 0, height = 0;
    graphics_get_display_size(DISPMANX_ID_HDMI, &width, &height);
    WKViewResize(view, WKSizeMake(width, height));

    auto page = WKViewGetPage(view);
    WKPageSetPageNavigationClient(page, &s_navigationClient.base);

    const char* url = "http://youtube.com/tv";
    if (argc > 1)
        url = argv[1];

    auto shellURL = WKURLCreateWithUTF8CString(url);
    WKPageLoadURL(page, shellURL);
    WKRelease(shellURL);

    g_main_loop_run(loop);

    WKRelease(view);
    WKRelease(pageConfiguration);
    WKRelease(pageGroup);
    WKRelease(context);
    WKRelease(preferences);
    g_main_loop_unref(loop);
    return 0;
}
