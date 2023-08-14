#include "pch.h"
#include "SimpleTypes.h"
#include "SDLGlobal.h"

SDL3::EventType SDL3::Event::Type() const {
    return EventType(e.type);
}

bool SDL3::Event::Poll() {
    return SDL::PollEvent(*this);
}
