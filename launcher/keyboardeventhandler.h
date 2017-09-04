#pragma once 

#include <memory>
#include <tuple>
#include <wpe/input.h>

class KeyboardEventHandler {
public:
    static std::unique_ptr<KeyboardEventHandler> create();

    virtual ~KeyboardEventHandler() = default;

    using Result = std::tuple<uint32_t, uint32_t, uint8_t>;
    virtual Result handleKeyboardEvent(struct wpe_input_keyboard_event*) = 0;
};
