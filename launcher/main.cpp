#include <WPE/WebKit.h>
#include <WPE/WebKit/WKCookieManagerSoup.h>
#include <WPE/WebKit/WKUserMediaPermissionRequest.h>

#include <cstdio>
#include <glib.h>
#include <initializer_list>
#include <string.h>

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
    // didFinishDocumentLoad
    [](WKPageRef page, WKNavigationRef, WKTypeRef, const void*) {
        WKStringRef messageName = WKStringCreateWithUTF8CString("Hello");
        WKMutableArrayRef messageBody = WKMutableArrayCreate();

        for (auto& item : { "Test1", "Test2", "Test3" }) {
            WKStringRef itemString = WKStringCreateWithUTF8CString(item);
            WKArrayAppendItem(messageBody, itemString);
            WKRelease(itemString);
        }

        fprintf(stderr, "[WPELauncher] Hello InjectedBundle ...\n");
        WKPagePostMessageToInjectedBundle(page, messageName, messageBody);
        WKRelease(messageBody);
        WKRelease(messageName);
    },
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

WKViewClientV0 s_viewClient = {
    { 0, nullptr },
    // frameDisplayed
    [](WKViewRef, const void*) {
        static unsigned s_frameCount = 0;
        static gint64 lastDumpTime = g_get_monotonic_time();

        ++s_frameCount;
        gint64 time = g_get_monotonic_time();
        if (time - lastDumpTime >= 5 * G_USEC_PER_SEC) {
            fprintf(stderr, "[WPELauncher] %.2fFPS\n",
                s_frameCount * G_USEC_PER_SEC * 1.0 / (time - lastDumpTime));
            s_frameCount = 0;
            lastDumpTime = time;
        }
    },
};

WKPageUIClientV6 s_uiClient;

static void decidePolicyForUserMediaPermissionRequestCallBack(WKPageRef, WKFrameRef, WKSecurityOriginRef, WKSecurityOriginRef, WKUserMediaPermissionRequestRef permissionRequest, const void*)
{
    WKArrayRef audioDeviceUIDs = WKUserMediaPermissionRequestAudioDeviceUIDs(permissionRequest);
    WKArrayRef videoDeviceUIDs = WKUserMediaPermissionRequestVideoDeviceUIDs(permissionRequest);

    if (WKArrayGetSize(videoDeviceUIDs) || WKArrayGetSize(audioDeviceUIDs)) {
        WKStringRef videoDeviceUID;
        if (WKArrayGetSize(videoDeviceUIDs))
            videoDeviceUID = reinterpret_cast<WKStringRef>(WKArrayGetItemAtIndex(videoDeviceUIDs, 0));
        else
            videoDeviceUID = WKStringCreateWithUTF8CString("");

        WKStringRef audioDeviceUID;
        if (WKArrayGetSize(audioDeviceUIDs))
            audioDeviceUID = reinterpret_cast<WKStringRef>(WKArrayGetItemAtIndex(audioDeviceUIDs, 0));
        else
            audioDeviceUID = WKStringCreateWithUTF8CString("");

        WKUserMediaPermissionRequestAllow(permissionRequest, audioDeviceUID, videoDeviceUID);
    }
}

int main(int argc, char* argv[])
{
    GMainLoop* loop = g_main_loop_new(nullptr, FALSE);

    auto contextConfiguration = WKContextConfigurationCreate();
    auto injectedBundlePath = WKStringCreateWithUTF8CString("/usr/lib/libWPEInjectedBundle.so");
    WKContextConfigurationSetInjectedBundlePath(contextConfiguration, injectedBundlePath);

    gchar *wpeStoragePath = g_build_filename(g_get_user_cache_dir(), "wpe", "local-storage", nullptr);
    g_mkdir_with_parents(wpeStoragePath, 0700);
    auto storageDirectory = WKStringCreateWithUTF8CString(wpeStoragePath);
    g_free(wpeStoragePath);
    WKContextConfigurationSetLocalStorageDirectory(contextConfiguration, storageDirectory);

    gchar *wpeDiskCachePath = g_build_filename(g_get_user_cache_dir(), "wpe", "disk-cache", nullptr);
    g_mkdir_with_parents(wpeDiskCachePath, 0700);
    auto diskCacheDirectory = WKStringCreateWithUTF8CString(wpeDiskCachePath);
    g_free(wpeDiskCachePath);
    WKContextConfigurationSetDiskCacheDirectory(contextConfiguration, diskCacheDirectory);

    WKRelease(injectedBundlePath);

    WKContextRef context = WKContextCreateWithConfiguration(contextConfiguration);
    WKRelease(contextConfiguration);

    auto pageGroupIdentifier = WKStringCreateWithUTF8CString("WPEPageGroup");
    auto pageGroup = WKPageGroupCreateWithIdentifier(pageGroupIdentifier);
    WKRelease(pageGroupIdentifier);

    auto preferences = WKPreferencesCreate();
    // Allow mixed content.
    WKPreferencesSetAllowRunningOfInsecureContent(preferences, true);
    WKPreferencesSetAllowDisplayOfInsecureContent(preferences, true);

    // By default allow console log messages to system console reporting.
    if (!g_getenv("WPE_SHELL_DISABLE_CONSOLE_LOG"))
      WKPreferencesSetLogsPageMessagesToSystemConsoleEnabled(preferences, true);

    WKPageGroupSetPreferences(pageGroup, preferences);

    auto pageConfiguration  = WKPageConfigurationCreate();
    WKPageConfigurationSetContext(pageConfiguration, context);
    WKPageConfigurationSetPageGroup(pageConfiguration, pageGroup);
    WKPreferencesSetFullScreenEnabled(preferences, true);

    if (!!g_getenv("WPE_SHELL_COOKIE_STORAGE")) {
      gchar *cookieDatabasePath = g_build_filename(g_get_user_cache_dir(), "cookies.db", nullptr);
      auto path = WKStringCreateWithUTF8CString(cookieDatabasePath);
      g_free(cookieDatabasePath);
      auto cookieManager = WKContextGetCookieManager(context);
      WKCookieManagerSetCookiePersistentStorage(cookieManager, path, kWKCookieStorageTypeSQLite);
    }

    auto view = WKViewCreate(pageConfiguration);
    WKViewSetViewClient(view, &s_viewClient.base);

    auto page = WKViewGetPage(view);
    WKPageSetPageNavigationClient(page, &s_navigationClient.base);
    memset(&s_uiClient, 0, sizeof(s_uiClient));
    s_uiClient.base.version = 6;
    s_uiClient.decidePolicyForUserMediaPermissionRequest = decidePolicyForUserMediaPermissionRequestCallBack;
    WKPageSetPageUIClient(page, &s_uiClient.base);

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
