#include "pch.h"
#include "Turtle.h"
#include <cmath>
#include <Renderer.h>
#include <numbers>

Turtle::Turtle(SDL3::Renderer& r) : m_Render(r) {
	
}

void Turtle::Draw() {
	m_Render.SetDrawColor(255, 255, 255, 255);
	m_Render.Clear();
	m_Render.SetDrawColor(0, 0, 0, 255);

	for (auto& [p1, p2] : m_Lines)
		DrawLine(p1, p2);
}

void Turtle::Forward(float amount) {
	auto state = Save();
	m_State.X += std::cos(ToRad(m_State.Heading)) * amount * m_Step;
	m_State.Y -= std::sin(ToRad(m_State.Heading)) * amount * m_Step;
	if (!m_Penup)
		m_Lines.push_back({ { state.X, state.Y }, { m_State.X, m_State.Y } });
}

void Turtle::Back(float amount) {
	Forward(-amount);
}

void Turtle::Rotate(float angle) {
	m_State.Heading += angle;
}

void Turtle::Penup() {
	m_Penup = true;
}

void Turtle::Pendown() {
	m_Penup = false;
}

void Turtle::SetStep(float size) {
	m_Step = size;
}

void Turtle::SetRadians(bool radians) {
	m_Radians = radians;
}

bool Turtle::IsRadians() const {
	return m_Radians;
}

TurtleState Turtle::Save() const {
	return TurtleState(m_State.X, m_State.Y, m_State.Heading);
}

void Turtle::Restore(TurtleState const& state) {
	m_State = state;
}

void Turtle::SetCenter(float x, float y) {
	m_CenterX = x;
	m_CenterY = y;
}

bool Turtle::IsPenup() const {
	return m_Penup;
}

float Turtle::GetStep() const {
	return m_Step;
}

float Turtle::ToRad(float angle) const {
	return m_Radians ? angle : angle * std::numbers::pi_v<float> / 180;
}

void Turtle::DrawLine(SDL3::FPoint const& from, SDL3::FPoint const& to) const {
	m_Render.Line(m_CenterX + from.x, m_CenterY + from.y, m_CenterX + to.x, m_CenterY + to.y);
}
