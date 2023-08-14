#pragma once

namespace SDL3 {
	struct FPoint : SDL_FPoint {
	};

	struct FRect : SDL_FRect {
	};

	struct Rect : SDL_Rect {
	};

	struct Color : SDL_Color {
	};

	enum class EventType {
		First =			SDL_EVENT_FIRST,
		Quit =			SDL_EVENT_QUIT
	};

	struct Event {
		SDL_Event e;

		EventType Type() const;
		bool Poll();
	};
}

