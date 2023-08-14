#include "pch.h"
#include "SDLGlobal.h"
#include "SimpleTypes.h"

SDL3::SDL::SDL(Init flags) {
	if (SDL_Init(static_cast<SDL_InitFlags>(flags)) < 0)
		throw SDLException();
}

SDL3::SDL::~SDL() {
	SDL_Quit();
}

SDL3::Init SDL3::SDL::GetInitFlags(Init flags) const {
	return static_cast<Init>(SDL_WasInit(static_cast<SDL_InitFlags>(flags)));
}

bool SDL3::SDL::PollEvent(Event& evt) {
	return SDL_PollEvent(&evt.e);
}

void SDL3::SDL::EnableExceptions(bool enable) {
	s_EnableExceptions = enable;
}

bool SDL3::SDL::Check(int error) {
	if (error == 0)
		return true;

	if (s_EnableExceptions)
		throw SDLException();
	return false;
}

const char* SDL3::SDLException::Error() const {
	return SDL_GetError();
}
