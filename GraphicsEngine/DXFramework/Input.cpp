// Input class
// Functions for retrieving input events/state.
#include "Input.h"

void Input::SetKeyDown(WPARAM key)
{
	keys[key] = true;
}

void Input::SetKeyUp(WPARAM key)
{
	keys[key] = false;
}

bool Input::isKeyDown(int key) const
{
	return keys[key];
}

void Input::setMouseX(int xPosition)
{
	mouse.x = xPosition;
}

void Input::setMouseY(int yPosition)
{
	mouse.y = yPosition;
}

int Input::getMouseX() const
{
	return mouse.x;
}

int Input::getMouseY() const
{
	return mouse.y;
}

void Input::setLeftMouse(bool down)
{
	mouse.left = down;
}

void Input::setRightMouse(bool down)
{
	mouse.right = down;
}

bool Input::isLeftMouseDown() const
{
	return mouse.left;
}

bool Input::isRightMouseDown() const
{
	return mouse.right;
}

void Input::setMouseActive(bool active)
{
	mouse.isActive = active;
}
bool Input::isMouseActive() const
{
	return mouse.isActive;
}