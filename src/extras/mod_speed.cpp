#include "common.h"

#ifdef DEBUGMENU

#include "main.h"
#include "PlayerPed.h"
#include "World.h"
#include "Vehicle.h"

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

static void UpdateVehicleSpeed()
{
	CPlayerPed *ped = CWorld::Players[CWorld::PlayerInFocus].m_pPed;
	if (ped == nil || !ped->bInVehicle || ped->m_pMyVehicle == nil)
		return;

	CVehicle *vehicle = ped->m_pMyVehicle;
	if (vehicle->pDriver != ped)
		return;

	if (StopKeyDown()) {
		vehicle->SetMoveSpeed(0.0f, 0.0f, 0.0f);
		vehicle->SetTurnSpeed(0.0f, 0.0f, 0.0f);
		return;
	}

	if (!FastKeyDown())
		return;

	CVector speed = vehicle->GetMoveSpeed();
	float speed2d = std::sqrt(speed.x * speed.x + speed.y * speed.y);

	if (speed2d > 0.0001f) {
		// Fast, but intentionally kept below the previous extreme value.
		const float fastSpeed = 2.0f;
		float scale = fastSpeed / speed2d;
		vehicle->SetMoveSpeed(speed.x * scale, speed.y * scale, speed.z);
	}
}

static void Worker()
{
	while (running.load(std::memory_order_relaxed)) {
		UpdateVehicleSpeed();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

struct Starter {
	Starter() { std::thread(Worker).detach(); }
};

static Starter starter;

}

#endif
