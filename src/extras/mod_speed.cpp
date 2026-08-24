#include "common.h"

#ifdef DEBUGMENU

#include "main.h"
#include "PlayerPed.h"

#ifdef LIBRW_GLFW
#include <GLFW/glfw3.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#include <cmath>

namespace ReVCModSpeed {

static bool FastKeyDown()
{
#ifdef LIBRW_GLFW
	if (PSGLOBAL(window) != nil)
		return glfwGetKey(PSGLOBAL(window), GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS;
#elif defined(_WIN32)
	return (GetAsyncKeyState(VK_OEM_6) & 0x8000) != 0;
#endif
	return false;
}

static bool StopKeyDown()
{
#ifdef LIBRW_GLFW
	if (PSGLOBAL(window) != nil)
		return glfwGetKey(PSGLOBAL(window), GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS;
#elif defined(_WIN32)
	return (GetAsyncKeyState(VK_OEM_4) & 0x8000) != 0;
#endif
	return false;
}

void Process()
{
	CPlayerPed *ped = FindPlayerPed();
	if (ped == nil)
		return;

	if (StopKeyDown()) {
		ped->SetMoveSpeed(0.0f, 0.0f, 0.0f);
		return;
	}

	if (!FastKeyDown())
		return;

	CVector speed = ped->GetMoveSpeed();
	float speed2d = std::sqrt(speed.x * speed.x + speed.y * speed.y);

	if (speed2d > 0.0001f) {
		const float fastSpeed = 5.0f;
		float scale = fastSpeed / speed2d;
		ped->SetMoveSpeed(speed.x * scale, speed.y * scale, speed.z);
	}
}

}

#endif
