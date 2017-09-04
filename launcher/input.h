#pragma once

#include <glib.h>
#include <libudev.h>
#include <libinput.h>
#include <memory>
#include <wpe/input.h>

class KeyboardEventHandler;

class Input {
public:
    class Client {
    public:
        virtual void handleKeyboardEvent(struct wpe_input_keyboard_event*) = 0;
    };

    Input(Client*);
    ~Input();

private:
    void processEvents();

    Client* m_client;
    std::unique_ptr<KeyboardEventHandler> m_keyboardEventHandler;

    struct udev* m_udev { nullptr };
    struct libinput* m_libinput { nullptr };
    GSource* m_source { nullptr };

    class EventSource {
    public:
        static GSourceFuncs s_sourceFuncs;

        GSource source;
        GPollFD pfd;
        Input* input;
    };
};
