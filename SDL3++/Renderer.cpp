#include "pch.h"
#include "Renderer.h"
#include "Window.h"
#include "Surface.h"
#include "SimpleTypes.h"

SDL3::Renderer::Renderer(Window& window, const char* name, RendererFlags flags) {
	Set(SDL_CreateRenderer(window, name, static_cast<Uint32>(flags)));
}

SDL3::Renderer::Renderer(Surface& surface) {
	Set(SDL_CreateSoftwareRenderer(surface));
}

SDL3::Window SDL3::Renderer::GetWindow() const {
	return Window(SDL_GetRenderWindow(Get()), false);
}

bool SDL3::Renderer::Flush() {
	return SDL::Check(SDL_RenderFlush(Get()));
}

bool SDL3::Renderer::Clear() {
	return SDL::Check(SDL_RenderClear(Get()));
}

bool SDL3::Renderer::Line(float x1, float y1, float x2, float y2) {
	return SDL::Check(SDL_RenderLine(Get(), x1, y1, x2, y2));
}

bool SDL3::Renderer::Lines(FPoint const* points, int count) {
	return SDL::Check(SDL_RenderLines(Get(), points, count));
}

bool SDL3::Renderer::Lines(std::vector<FPoint> const& points) {
	return Lines(points.data(), (int)points.size());
}

bool SDL3::Renderer::Rectangle(FRect const& rect) {
	return SDL::Check(SDL_RenderRect(Get(), &rect));
}

bool SDL3::Renderer::Rectangles(FRect const* rects, int count) {
	return SDL::Check(SDL_RenderRects(Get(), rects, count));
}

bool SDL3::Renderer::Rectangles(std::vector<FRect> const& rects) {
	return Rectangles(rects.data(), (int)rects.size());
}

bool SDL3::Renderer::SetDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	return SDL::Check(SDL_SetRenderDrawColor(Get(), r, g, b, a));
}

bool SDL3::Renderer::SetDrawColor(Color const& color) {
	return SetDrawColor(color.r, color.g, color.b, color.a);
}

bool SDL3::Renderer::GetDrawColor(Color& color) const {
	return SDL::Check(SDL_GetRenderDrawColor(Get(), &color.r, &color.g, &color.b, &color.a));
}

bool SDL3::Renderer::GetDrawColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) {
	return SDL::Check(SDL_GetRenderDrawColor(Get(), &r, &g, &b, &a));
}

bool SDL3::Renderer::Present() {
	return SDL::Check(SDL_RenderPresent(Get()));
}