/*
 * Copyright (C) 2015, 2016 Igalia S.L.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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

static void injectJSExtensions(JSGlobalContextRef context)
{
    static JSValueRef s_value1 = JSValueMakeNumber(context, 15);
    static JSValueRef s_value2 = JSValueMakeNumber(context, 16);

    static JSStaticValue s_staticValues[] = {
        {
            "value1", // name
            // getProperty
            [](JSContextRef context, JSObjectRef, JSStringRef, JSValueRef*) -> JSValueRef {
                fprintf(stderr, "[WPE-EXT] getProperty for 'value1' invoked.\n");
                return s_value1;
            },
            nullptr, // setProperty
            kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete, // attributes
        },
        {
            "value2", // name
            // getProperty
            [](JSContextRef context, JSObjectRef, JSStringRef, JSValueRef*) -> JSValueRef {
                fprintf(stderr, "[WPE-EXT] getProperty for 'value2' invoked.\n");
                return s_value2;
            },
            // setProperty
            [](JSContextRef, JSObjectRef, JSStringRef, JSValueRef value, JSValueRef*) -> bool {
                fprintf(stderr, "[WPE-EXT] setProperty for 'value2' invoked.\n");
                s_value2 = value;
                return true;
            },
            kJSPropertyAttributeNone, // attributes
        },
        { nullptr, nullptr, nullptr, 0 }
    };

    static JSStaticFunction s_staticFunctions[] = {
        {
            "function1", // name
            // callAsFunction
            [](JSContextRef context, JSObjectRef, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef*) -> JSValueRef {
                fprintf(stderr, "[WPE-EXT] callAsFunction for 'function1' invoked.\n");

                JSStringRef returnString = JSStringCreateWithUTF8CString("function1 return value");
                JSValueRef returnValue = JSValueMakeString(context, returnString);
                JSStringRelease(returnString);
                return returnValue;
            },
            kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete, // attributes
        },
        { nullptr, nullptr, 0 }
    };
    static JSClassDefinition s_classDefinition = {
        0, // version
        kJSClassAttributeNone, //attributes
        "WPE", // className
        0, // parentClass
        s_staticValues, // staticValues
        s_staticFunctions, // staticFunctions
        nullptr, //initialize
        nullptr, //finalize
        nullptr, //hasProperty
        nullptr, //getProperty
        nullptr, //setProperty
        nullptr, //deleteProperty
        nullptr, //getPropertyNames
        nullptr, //callAsFunction
        nullptr, //callAsConstructor
        nullptr, //hasInstance
        nullptr, //convertToType
    };

    JSClassRef wpeClass = JSClassCreate(&s_classDefinition);
    JSValueRef wpeObject = JSObjectMake(context, wpeClass, nullptr);
    JSClassRelease(wpeClass);

    JSStringRef extensionString = JSStringCreateWithUTF8CString("wpe");
    JSObjectSetProperty(context, JSContextGetGlobalObject(context), extensionString, wpeObject,
        kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete, nullptr);
    JSStringRelease(extensionString);
}

static WKBundlePageLoaderClientV6 s_pageLoaderClient = {
    { 6, nullptr },
    nullptr, // didStartProvisionalLoadForFrame
    nullptr, // didReceiveServerRedirectForProvisionalLoadForFrame
    nullptr, // didFailProvisionalLoadWithErrorForFrame
    nullptr, // didCommitLoadForFrame
    nullptr, // didFinishDocumentLoadForFrame
    nullptr, // didFinishLoadForFrame
    nullptr, // didFailLoadWithErrorForFrame
    nullptr, // didSameDocumentNavigationForFrame
    nullptr, // didReceiveTitleForFrame
    nullptr, // didFirstLayoutForFrame
    nullptr, // didFirstVisuallyNonEmptyLayoutForFrame
    nullptr, // didRemoveFrameFromHierarchy
    nullptr, // didDisplayInsecureContentForFrame
    nullptr, // didRunInsecureContentForFrame
    // didClearWindowObjectForFrame
    [](WKBundlePageRef, WKBundleFrameRef frame, WKBundleScriptWorldRef scriptWorld, const void*) {
        fprintf(stderr, "didClearWindowObjectForFrame\n");
        JSGlobalContextRef context = WKBundleFrameGetJavaScriptContextForWorld(frame, scriptWorld);
        injectJSExtensions(context);
    },
    nullptr, // didCancelClientRedirectForFrame
    nullptr, // willPerformClientRedirectForFrame
    nullptr, // didHandleOnloadEventsForFrame
    nullptr, // didLayoutForFrame
    nullptr, // didNewFirstVisuallyNonEmptyLayout_unavailable
    nullptr, // didDetectXSSForFrame
    nullptr, // shouldGoToBackForwardListItem
    nullptr, // globalObjectIsAvailableForFrame
    nullptr, // willDisconnectDOMWindowExtensionFromGlobalObject
    nullptr, // didReconnectDOMWindowExtensionToGlobalObject
    nullptr, // willDestroyGlobalObjectForDOMWindowExtension
    nullptr, // didFinishProgress
    nullptr, // shouldForceUniversalAccessFromLocalURL
    nullptr, // didReceiveIntentForFrame_unavailable
    nullptr, // registerIntentServiceForFrame_unavailable
    nullptr, // didLayout
    nullptr, // featuresUsedInPage
    nullptr, // willLoadURLRequest
    nullptr, // willLoadDataRequest
};

static WKBundleClientV1 s_bundleClient = {
    { 1, nullptr },
    // didCreatePage
    [](WKBundleRef, WKBundlePageRef page, const void*) {
        WKBundlePageSetPageLoaderClient(page, &s_pageLoaderClient.base);
    },
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

extern "C"
void WKBundleInitialize(WKBundleRef bundle, WKTypeRef)
{
    fprintf(stderr, "[WPEInjectedBundle] Initialized.\n");

    WKBundleSetClient(bundle, &s_bundleClient.base);
}
