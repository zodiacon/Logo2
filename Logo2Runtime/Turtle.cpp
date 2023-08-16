#include "pch.h"
#include "Turtle.h"
#include <cmath>
#include <numbers>

using namespace Gdiplus;
using namespace Logo2;

Turtle::Turtle() {
}

void Logo2::Turtle::SetNotify(ICommandNotify* pNotify) {
	m_pNotify = pNotify;
}

void Turtle::Forward(float amount) {
	auto state = Save();
	m_State.X += std::cos(ToRad(m_State.Heading)) * amount * m_Step;
	m_State.Y += std::sin(ToRad(m_State.Heading)) * amount * m_Step;
	if (!m_Penup) {
		TurtleCommand cmd{ TurtleCommandType::DrawLine };
		cmd.Line.From = Point2D(state.X, state.Y);
		cmd.Line.To = Point2D(m_State.X, m_State.Y);
		m_Commands.push_back(cmd);
		if (m_pNotify)
			m_pNotify->AddCommand(this, cmd);
	}
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

std::span<const TurtleCommand> Logo2::Turtle::GetCommands() const {
	return m_Commands;
}

bool Turtle::IsPenup() const {
	return m_Penup;
}

float Turtle::GetStep() const {
	return m_Step;
}

void Logo2::Turtle::SetPenColor(BYTE r, BYTE g, BYTE b, BYTE a) {
	TurtleCommand cmd;
	cmd.Type = TurtleCommandType::SetColor;
	cmd.Color = (a << 24) | (r << 16) | (g << 8) | b;
	m_Commands.push_back(cmd);
}

float Turtle::ToRad(float angle) const {
	return m_Radians ? angle : angle * std::numbers::pi_v<float> / 180;
}

