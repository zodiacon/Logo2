#pragma once

namespace SDL3 {
	struct SDLException final {
		const char* Error() const;
	};

	enum class Init {
		None = 0,
		Timer =		SDL_INIT_TIMER,
		Audio =		SDL_INIT_AUDIO,
		Video =		SDL_INIT_VIDEO,
		Joystick =	SDL_INIT_JOYSTICK,
		Haptic =	SDL_INIT_HAPTIC,
		GamePad =	SDL_INIT_GAMEPAD,
		Events =	SDL_INIT_EVENTS,
		Sensor =	SDL_INIT_SENSOR,
		Everything = Timer | Audio | Video | Joystick | Haptic | GamePad | Events | Sensor
	};
	DEFINE_ENUM_FLAG_OPERATORS(SDL3::Init);

	struct Event;

	class SDL final {
	public:
		SDL(Init flags);
		~SDL();

		Init GetInitFlags(Init flags = Init::None) const;
		
		static bool PollEvent(Event& evt);
		static void EnableExceptions(bool enable);
		static bool Check(int error);

		inline static bool s_EnableExceptions{ true };
	};
}
