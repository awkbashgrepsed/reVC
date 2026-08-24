#include "common.h"

#ifdef DEBUGMENU

#include "main.h"
#include "Ped.h"

#ifdef LIBRW_GLFW
#include <GLFW/glfw3.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#include <atomic>
#include <chrono>
#include <cmath>
#include <thread>

namespace ReVCModSpeed {

static std::atomic<bool> running{true};

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

static void UpdatePlayerSpeed()
{
	CPed *ped = FindPlayerPed();
	if (ped == nil)
		return;

	if (StopKeyDown()) {
		ped->m_vecMoveSpeed.x = 0.0f;
		ped->m_vecMoveSpeed.y = 0.0f;
		ped->m_vecMoveSpeed.z = 0.0f;
		return;
	}

	if (!FastKeyDown())
		return;

	// Keep the direction selected by normal player input, but force an
	// extremely high horizontal velocity while ] is held.
	float speedX = ped->m_vecMoveSpeed.x;
	float speedY = ped->m_vecMoveSpeed.y;
	float speed2d = std::sqrt(speedX * speedX + speedY * speedY);

	if (speed2d > 0.0001f) {
		const float fastSpeed = 2.0f;
		float scale = fastSpeed / speed2d;
		ped->m_vecMoveSpeed.x = speedX * scale;
		ped->m_vecMoveSpeed.y = speedY * scale;
	}
}

static void Worker()
{
	while (running.load(std::memory_order_relaxed)) {
		UpdatePlayerSpeed();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

// DEBUGMENU builds start the tiny input poller automatically. The normal
// keyboard system still handles movement direction; this only overrides
// velocity while one of the two mod keys is held.
struct Starter {
	Starter()
	{
		std::thread(Worker).detach();
	}
};

static Starter starter;

}

#endif
