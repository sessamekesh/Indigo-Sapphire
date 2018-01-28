#pragma once

// Camera controller base
//
// This is a polling system - e.g., it is anticipated that every frame, some interested object will
//  ask what the current state of action is with this controller.
// As such, there are no events that fire - only questions to be answered.
// Pure abstract class.

namespace input
{
	class CameraControllerBase
	{
	public:
		virtual void tick() = 0;

		virtual bool isActive() = 0;

		// Returns a number, -1 to 1, of the Y1 axis movement (0 is neutral)
		virtual float getY1Axis() const = 0;

		// Returns a number, -1 to 1, of the Y2 axis movement (0 is neutral)
		virtual float getY2Axis() const = 0;

		// Returns a number, -1 to 1, of the X1 axis movement (0 is neutral)
		virtual float getX1Axis() const = 0;

		// Returns a number, -1 to 1, of the X2 axis movement (0 is neutral)
		virtual float getX2Axis() const = 0;
	};
}