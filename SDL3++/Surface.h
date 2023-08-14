#pragma once

#include "SDLObject.h"

namespace SDL3 {
	class Surface : public SDLObject<SDL_Surface, decltype(SDL_DestroySurface)> {
	public:
		using SDLObject::SDLObject;
	};
}

