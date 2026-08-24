#include "common.h"

#ifdef DEBUGMENU

#include "main.h"
#include "Ped.h"
#include "Pad.h"
#include "Timer.h"

#ifdef LIBRW_GLFW
#include <GLFW/glfw3.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#include <atomic>
#include <chrono>
#include <thread>

namespace ReVCModSpeed {

static std::atomic<bool> running{true};
static std::once_flag startOnce;

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
		ped->SetMoveSpeed(0.0f, 0.0f, 0.0f);
		return;
	}

	if (!FastKeyDown())
		return;

	// Keep the current movement direction, but replace the normal pedestrian
	// velocity with a deliberately extreme speed. Normal input still chooses
	// the direction, while ] acts as a temporary speed multiplier.
	CVector speed = ped->GetMoveSpeed();
	float speed2d = speed.Magnitude2D();
	if (speed2d > 0.0001f) {
		const float fastSpeed = 2.0f;
		float scale = fastSpeed / speed2d;
		ped->SetMoveSpeed(speed.x * scale, speed.y * scale, speed.z);
	}
}

static void Worker()
{
	while (running.load(std::memory_order_relaxed)) {
		UpdatePlayerSpeed();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

// The existing DEBUGMENU is initialized early and this small background
// poller lets the speed keys work without changing the platform-specific
// keyboard code. It is intentionally disabled outside DEBUGMENU builds.
struct Starter {
	Starter()
	{
		std::call_once(startOnce, [] {
			std::thread(Worker).detach();
		});
	}
};

static Starter starter;

}

#endif
