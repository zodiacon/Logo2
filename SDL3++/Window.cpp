#include "pch.h"
#include "Window.h"

SDL3::Window::Window(const char* title, int width, int height, WindowFlags flags) {
	Set(SDL_CreateWindow(title, width, height, static_cast<uint32_t>(flags)));
}

SDL3::Window::Window(const char* title, int x, int y, int width, int height, WindowFlags flags) {
	Set(SDL_CreateWindowWithPosition(title, x, y, width, height, static_cast<uint32_t>(flags)));
}

SDL3::Window::Window(Window* parent, int offsetX, int offsetY, int width, int height, WindowFlags flags) {
	Set(SDL_CreatePopupWindow(parent->Get(), offsetX, offsetY, width, height, static_cast<uint32_t>(flags)));
}

uint32_t SDL3::Window::GetId() const {
	return SDL_GetWindowID(Get());
}

bool SDL3::Window::FlashWindow(FlashOperation operation) {
	return SDL::Check(SDL_FlashWindow(Get(), static_cast<SDL_FlashOperation>(operation)));
}

bool SDL3::Window::Show() {
	return SDL::Check(SDL_ShowWindow(Get()));
}

bool SDL3::Window::Hide() {
	return SDL::Check(SDL_HideWindow(Get()));
}

bool SDL3::Window::Raise() {
	return SDL::Check(SDL_RaiseWindow(Get()));
}

bool SDL3::Window::Minimize() {
	return SDL::Check(SDL_MinimizeWindow(Get()));
}

bool SDL3::Window::Restore() {
	return SDL::Check(SDL_RestoreWindow(Get()));
}

bool SDL3::Window::Maximize() {
	return SDL::Check(SDL_MaximizeWindow(Get()));
}

bool SDL3::Window::SetAlwaysOnTop(bool ontop) {
	return SDL::Check(SDL_SetWindowAlwaysOnTop(Get(), (SDL_bool)ontop));
}

bool SDL3::Window::SetResizable(bool resizable) {
	return SDL::Check(SDL_SetWindowResizable(Get(), (SDL_bool)resizable));
}

bool SDL3::Window::SetBordered(bool bordered) {
	return SDL::Check(SDL_SetWindowBordered(Get(), (SDL_bool)bordered));
}

bool SDL3::Window::GetMaximumSize(int& width, int& height) const {
	return SDL::Check(SDL_GetWindowMaximumSize(Get(), &width, &height));
}

bool SDL3::Window::SetMaximumSize(int width, int height) {
	return SDL::Check(SDL_SetWindowMaximumSize(Get(), width, height));
}

bool SDL3::Window::SetFullScreen(bool fullScreen) {
	return SDL::Check(SDL_SetWindowFullscreen(Get(), (SDL_bool)fullScreen));
}

bool SDL3::Window::HasSurface() const {
	return (bool)SDL_HasWindowSurface(Get());
}

bool SDL3::Window::UpdateSurface() {
	return SDL::Check(SDL_UpdateWindowSurface(Get()));
}

SDL3::Surface SDL3::Window::GetSurface() const {
	return Surface(SDL_GetWindowSurface(Get()), false);
}

bool SDL3::Window::DestroySurface() {
	return SDL::Check(SDL_DestroyWindowSurface(Get()));
}

SDL3::Renderer SDL3::Window::GetRenderer() const {
	return Renderer(SDL_GetRenderer(Get()), false);
}


