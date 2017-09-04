#include "input.h"

#include "keyboardeventhandler.h"
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <wpe/view-backend.h>

struct libinput_interface g_interface = {
    // open_restricted
    [](const char* path, int flags, void*)
    {
        return open(path, flags);
    },
    [](int fd, void*)
    {
        close(fd);
    }
};

Input::Input(Client* client)
    : m_client(client)
    , m_keyboardEventHandler(KeyboardEventHandler::create())
{
    m_udev = udev_new();
    if (!m_udev)
        return;

    m_libinput = libinput_udev_create_context(&g_interface, nullptr, m_udev);
    if (!m_libinput)
        return;

    int ret = libinput_udev_assign_seat(m_libinput, "seat0");
    if (ret)
        return;

    m_source = g_source_new(&EventSource::s_sourceFuncs, sizeof(EventSource));
    auto* source = reinterpret_cast<EventSource*>(m_source);
    source->pfd.fd = libinput_get_fd(m_libinput);
    source->pfd.events = G_IO_IN | G_IO_ERR | G_IO_HUP;
    source->pfd.revents = 0;
    g_source_add_poll(m_source, &source->pfd);
    source->input = this;

    g_source_set_name(m_source, "[WPE] input");
    g_source_set_priority(m_source, G_PRIORITY_DEFAULT + 30);
    g_source_attach(m_source, g_main_context_get_thread_default());

    fprintf(stderr, "[Input] Initialization of linux input system succeeded.\n");
}

Input::~Input()
{
    m_client = nullptr;
    if (m_source)
        g_source_unref(m_source);
    if (m_libinput)
        libinput_unref(m_libinput);
    if (m_udev)
        udev_unref(m_udev);
}

void Input::processEvents()
{
    libinput_dispatch(m_libinput);

    while (auto* event = libinput_get_event(m_libinput)) {
        switch (libinput_event_get_type(event)) {
        case LIBINPUT_EVENT_KEYBOARD_KEY:
        {
            auto* keyEvent = libinput_event_get_keyboard_event(event);

            struct wpe_input_keyboard_event rawEvent{
                libinput_event_keyboard_get_time(keyEvent),
                libinput_event_keyboard_get_key(keyEvent), 0,
                !!libinput_event_keyboard_get_key_state(keyEvent), 0
            };
            KeyboardEventHandler::Result result = m_keyboardEventHandler->handleKeyboardEvent(&rawEvent);

            struct wpe_input_keyboard_event event{ rawEvent.time, std::get<0>(result), std::get<1>(result), rawEvent.pressed, std::get<2>(result) };
            m_client->handleKeyboardEvent(&event);

            break;
        }
        default:
            break;
        }

        libinput_event_destroy(event);
    }
}

GSourceFuncs Input::EventSource::s_sourceFuncs = {
    nullptr, // prepare
    // check
    [](GSource* base) -> gboolean
    {
        auto* source = reinterpret_cast<EventSource*>(base);
        return !!source->pfd.revents;
    },
    // dispatch
    [](GSource* base, GSourceFunc, gpointer) -> gboolean
    {
        auto* source = reinterpret_cast<EventSource*>(base);

        if (source->pfd.revents & (G_IO_ERR | G_IO_HUP))
            return FALSE;

        if (source->pfd.revents & G_IO_IN)
            source->input->processEvents();
        source->pfd.revents = 0;
        return TRUE;
    },
    nullptr, // finalize
    nullptr, // closure_callback
    nullptr, // closure_marshall
};
