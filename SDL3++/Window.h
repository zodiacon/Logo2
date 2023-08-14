#pragma once

#include "SDLObject.h"
#include "Surface.h"
#include "Renderer.h"

namespace SDL3 {
	enum class WindowFlags {
		None =				0,
		FullScreen =		SDL_WINDOW_FULLSCREEN,
		OpenGL =			SDL_WINDOW_OPENGL,
		Occluded =			SDL_WINDOW_OCCLUDED,
		Hidden =			SDL_WINDOW_HIDDEN,
		Borderless =		SDL_WINDOW_BORDERLESS,
		Resizable =			SDL_WINDOW_RESIZABLE,
		Minimized =			SDL_WINDOW_MINIMIZED,
		Maximized =			SDL_WINDOW_MAXIMIZED,
		MouseGrabbed =		SDL_WINDOW_MOUSE_GRABBED,
		InputFocus =		SDL_WINDOW_INPUT_FOCUS,
		MouseFocus =		SDL_WINDOW_MOUSE_FOCUS,
		Foreign =			SDL_WINDOW_FOREIGN,
		HighPixelDensity =	SDL_WINDOW_HIGH_PIXEL_DENSITY,
		MouseCapture =		SDL_WINDOW_MOUSE_CAPTURE,
		AlwaysOnTop =		SDL_WINDOW_ALWAYS_ON_TOP,
		Utility =			SDL_WINDOW_UTILITY,
		ToolTip =			SDL_WINDOW_TOOLTIP,
		PopupMenu =			SDL_WINDOW_POPUP_MENU,
		KeyboardGrabbed =	SDL_WINDOW_KEYBOARD_GRABBED,
		Vulkan =			SDL_WINDOW_VULKAN,
		Metal =				SDL_WINDOW_METAL,
		Transparent =		SDL_WINDOW_TRANSPARENT,
	};
	DEFINE_ENUM_FLAG_OPERATORS(SDL3::WindowFlags);

	enum class FlashOperation {
		Cancel =		SDL_FLASH_CANCEL,
		Briefly =		SDL_FLASH_BRIEFLY,
		UntilFocused =	SDL_FLASH_UNTIL_FOCUSED
	};

	class Window : public SDLObject<SDL_Window, decltype(SDL_DestroyWindow)> {
	public:
		using SDLObject::SDLObject;

		Window(const char* title, int width, int height, WindowFlags flags);
		Window(const char* title, int x, int y, int width, int height, WindowFlags flags);
		Window(Window* parent, int offsetX, int offsetY, int width, int height, WindowFlags flags);

		uint32_t GetId() const;
		
		bool FlashWindow(FlashOperation operation);
		bool Show();
		bool Hide();
		bool Raise();
		bool Maximize();
		bool Minimize();
		bool Restore();
		bool SetAlwaysOnTop(bool ontop);
		bool SetResizable(bool resizable);
		bool SetBordered(bool bordered);
		bool GetMaximumSize(int& width, int& height) const;
		bool SetMaximumSize(int width, int height);
		bool SetFullScreen(bool fullScreen);
		bool HasSurface() const;
		bool UpdateSurface();
		Surface GetSurface() const;
		bool DestroySurface();
		Renderer GetRenderer() const;
	};
}

