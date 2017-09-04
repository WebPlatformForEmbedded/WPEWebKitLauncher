#include "keyboardeventhandler.h"

#include <xkbcommon/xkbcommon.h>

class KeyboardEventHandlerXKB final : public KeyboardEventHandler {
public:
    KeyboardEventHandlerXKB();
    virtual ~KeyboardEventHandlerXKB();

    Result handleKeyboardEvent(struct wpe_input_keyboard_event*) override;

private:
    struct xkb_keymap* m_xkbKeymap;
    struct xkb_state* m_xkbState;

    struct Modifiers {
        xkb_mod_index_t ctrl;
        xkb_mod_index_t shift;

        uint32_t effective;
    } m_modifiers;
};

std::unique_ptr<KeyboardEventHandler> KeyboardEventHandler::create()
{
    return std::unique_ptr<KeyboardEventHandlerXKB>(new KeyboardEventHandlerXKB);
}

KeyboardEventHandlerXKB::KeyboardEventHandlerXKB()
{
    struct xkb_context* context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    struct xkb_rule_names names = { "evdev", "pc105", "us", "", "" };

    m_xkbKeymap = xkb_keymap_new_from_names(context, &names, XKB_KEYMAP_COMPILE_NO_FLAGS);
    m_xkbState = xkb_state_new(m_xkbKeymap);

    m_modifiers.ctrl = xkb_keymap_mod_get_index(m_xkbKeymap, XKB_MOD_NAME_CTRL);
    m_modifiers.shift = xkb_keymap_mod_get_index(m_xkbKeymap, XKB_MOD_NAME_SHIFT);
    m_modifiers.effective = 0;

    xkb_context_unref(context);
}

KeyboardEventHandlerXKB::~KeyboardEventHandlerXKB()
{
    xkb_keymap_unref(m_xkbKeymap);
    xkb_state_unref(m_xkbState);
}

KeyboardEventHandler::Result KeyboardEventHandlerXKB::handleKeyboardEvent(struct wpe_input_keyboard_event* event)
{
    // Keycode system starts at 8. Go figure.
    int key = event->keyCode + 8;

    uint8_t keyModifiers = 0;
    if (m_modifiers.effective & (1 << m_modifiers.ctrl))
        keyModifiers |= wpe_input_keyboard_modifier_control;
    if (m_modifiers.effective & (1 << m_modifiers.shift))
        keyModifiers |= wpe_input_keyboard_modifier_shift;

    Result result{
        xkb_state_key_get_one_sym(m_xkbState, key),
        xkb_state_key_get_utf32(m_xkbState, key),
        keyModifiers
    };

    xkb_state_update_key(m_xkbState, key, event->pressed ? XKB_KEY_DOWN : XKB_KEY_UP);
    m_modifiers.effective = xkb_state_serialize_mods(m_xkbState,
        static_cast<xkb_state_component>(XKB_STATE_MODS_EFFECTIVE | XKB_STATE_LAYOUT_EFFECTIVE));

    return result;
}
