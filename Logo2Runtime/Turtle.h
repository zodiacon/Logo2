#pragma once

#include <span>

namespace Logo2 {
	struct TurtleState {
		float X{ 0 }, Y{ 0 };
		float Heading{ 0 };
	};

	enum class TurtleCommandType {
		DrawLine,
		SetColor,
		SetWidth,
	};

	struct Point2D {
		float X, Y;
	};

	struct TurtleCommand {
		TurtleCommandType Type;
		union {
			struct {
				Point2D From, To;
			} Line;
			uint32_t Color;
			float Width;
		};
	};

	class Turtle;

	struct ICommandNotify {
		virtual void AddCommand(Turtle* turtle, TurtleCommand const& cmd) = 0;
	};

	class Turtle {
	public:
		Turtle();

		void SetNotify(ICommandNotify* pNotify);

		void Forward(float amount);
		void Back(float amount);
		void Rotate(float angle);
		void Penup();
		void Pendown();
		bool IsPenup() const;
		void SetStep(float size);
		float GetStep() const;
		void SetPenColor(BYTE r, BYTE g, BYTE b, BYTE a = 255);
		void SetPenWidth(float width);
		void SetRadians(bool radians);
		bool IsRadians() const;

		TurtleState Save() const;
		void Restore(TurtleState const& state);

		std::span<const TurtleCommand> GetCommands() const;

	private:
		float ToRad(float angle) const;

		std::vector<TurtleCommand> m_Commands;
		ICommandNotify* m_pNotify{ nullptr };
		TurtleState m_State;
		bool m_Penup{ false };
		float m_Step{ 1 };
		bool m_Radians{ false };
	};
}
