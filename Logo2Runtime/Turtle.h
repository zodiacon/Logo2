#pragma once

namespace SDL3 {
	class Renderer;
}

struct TurtleState {
	float X{ 0 }, Y{ 0 };
	float Heading{ 0 };
};

class Turtle {
public:
	explicit Turtle(SDL3::Renderer& r);
	Turtle(Turtle const&) = delete;
	Turtle& operator=(Turtle const&) = delete;

	bool Init();

	void Forward(float amount);
	void Back(float amount);
	void Rotate(float angle);
	void Penup();
	void Pendown();
	bool IsPenup() const;
	void SetStep(float size);
	float GetStep() const;
	void SetRadians(bool radians);
	bool IsRadians() const;

	TurtleState Save() const;
	void Restore(TurtleState const& state);
	void SetCenter(float x, float y);

private:
	float ToRad(float angle) const;
	void DrawLine(TurtleState const& from, TurtleState const& to) const;

	SDL3::Renderer& m_Render;
	float m_CenterX, m_CenterY;
	TurtleState m_State;
	bool m_Penup{ false };
	float m_Step{ 1 };
	bool m_Radians{ false };
};

