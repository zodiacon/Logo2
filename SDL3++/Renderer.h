#pragma once

#include "SDLObject.h"

namespace SDL3 {
	class Window;
	class Surface;
	struct FPoint;
	struct FRect;
	struct Color;

	enum class RendererFlags {
		Default = 0,
		Software = SDL_RENDERER_SOFTWARE,
		Accelerated = SDL_RENDERER_ACCELERATED,
		PresentVSync = SDL_RENDERER_PRESENTVSYNC,
	};
	DEFINE_ENUM_FLAG_OPERATORS(SDL3::RendererFlags);

	class Renderer : public SDLObject<SDL_Renderer, decltype(SDL_DestroyRenderer)> {
	public:
		using SDLObject::SDLObject;

		explicit Renderer(Window& window, const char* name = nullptr, RendererFlags flags = RendererFlags::Default);
		explicit Renderer(Surface& surface);

		Window GetWindow() const;

		bool Present();
		bool Flush();
		bool Clear();
		bool Point(float x, float y);
		bool Points(FPoint const* points, int count);
		bool Line(float x1, float y1, float x2, float y2);
		bool Lines(FPoint const* points, int count);
		bool Lines(std::vector<FPoint> const& points);
		bool Rectangle(FRect const& rect);
		bool Rectangles(FRect const* rects, int count);
		bool Rectangles(std::vector<FRect> const& rects);
		bool FillRectangle(FRect const& rect);
		bool FillRectangles(FRect const* rects, int count);
		bool FillRectangles(std::vector<FRect> const& rects);

		bool SetDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		bool SetDrawColor(Color const& color);
		bool GetDrawColor(Color& color) const;
		bool GetDrawColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a);
	};
}

