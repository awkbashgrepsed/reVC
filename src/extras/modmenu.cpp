#include "common.h"

#ifdef DEBUGMENU

#include "debugmenu.h"
#include "World.h"
#include "PlayerInfo.h"
#include "PlayerPed.h"
#include "Streaming.h"
#include "Automobile.h"
#include "ModelIndices.h"
#include "Vehicle.h"

#include <atomic>
#include <chrono>
#include <thread>

namespace ReVCModMenu {

static float health = 100.0f;
static float armour = 100.0f;
static int32 weapon = WEAPONTYPE_COLT45;
static int32 skin = 0;
static int32 &money = CWorld::Players[0].m_nMoney;
static std::atomic<bool> godMode{false};
static std::atomic<bool> vehicleGodMode{false};
static std::atomic<bool> running{true};

static void ApplyHealth() { CPlayerPed *ped = CWorld::Players[CWorld::PlayerInFocus].m_pPed; if (ped) ped->m_fHealth = health; }
static void ApplyArmour() { CPlayerPed *ped = CWorld::Players[CWorld::PlayerInFocus].m_pPed; if (ped) ped->m_fArmour = armour; }
static void FullHealth() { health = 100.0f; ApplyHealth(); }
static void FullArmour() { armour = 100.0f; ApplyArmour(); }
static void ToggleGodMode() { godMode = !godMode.load(); }
static void ToggleVehicleGodMode() { vehicleGodMode = !vehicleGodMode.load(); }

static void GiveSelectedWeapon()
{
	CPlayerPed *ped = CWorld::Players[CWorld::PlayerInFocus].m_pPed;
	if (ped) { ped->GiveWeapon((eWeaponType)weapon, 9999); ped->SetCurrentWeapon((eWeaponType)weapon); }
}

static void GiveAllWeapons()
{
	CPlayerPed *ped = CWorld::Players[CWorld::PlayerInFocus].m_pPed;
	if (ped) for (int32 i = WEAPONTYPE_BRASSKNUCKLE; i <= WEAPONTYPE_CAMERA; i++) ped->GiveWeapon((eWeaponType)i, 9999);
}

static void ApplySkin()
{
	static const char *skinNames[] = { "player", "cop", "swat", "fbi", "army", "medic", "fireman", "golf" };
	if (skin >= 0 && skin < (int32)(sizeof(skinNames) / sizeof(skinNames[0])))
		CWorld::Players[CWorld::PlayerInFocus].SetPlayerSkin(skinNames[skin]);
}

static void SpawnVehicle(int32 modelId)
{
	CVector pos = FindPlayerCoors();
	CStreaming::RequestModel(modelId, STREAMFLAGS_DONT_REMOVE);
	CStreaming::LoadAllRequestedModels(true);
	if (!CStreaming::HasModelLoaded(modelId)) return;
	CAutomobile *vehicle = new CAutomobile(modelId, RANDOM_VEHICLE);
	if (!vehicle) return;
	pos.z += 0.5f;
	vehicle->SetPosition(pos);
	vehicle->SetHeading(FindPlayerHeading());
	vehicle->SetStatus(STATUS_PHYSICS);
	vehicle->SetIsStatic(false);
	vehicle->bUsesCollision = true;
	CWorld::Add(vehicle);
}

static void SpawnInfernus() { SpawnVehicle(MI_INFERNUS); }
static void SpawnRhino() { SpawnVehicle(MI_RHINO); }
static void SpawnPolice() { SpawnVehicle(MI_POLICE); }
static void AddMoney() { money += 10000; }
static void MaxMoney() { money = 99999999; }

static void ProtectEntities()
{
	while (running.load(std::memory_order_relaxed)) {
		CPlayerPed *ped = CWorld::Players[CWorld::PlayerInFocus].m_pPed;
		if (ped) {
			if (godMode.load()) { ped->m_fHealth = 100.0f; ped->m_fArmour = 100.0f; }
			if (vehicleGodMode.load() && ped->bInVehicle && ped->m_pMyVehicle && ped->m_pMyVehicle->pDriver == ped)
				ped->m_pMyVehicle->m_fHealth = 1000.0f;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
}

struct RegisterMenu {
	RegisterMenu()
	{
		DebugMenuAddInt32("Mod Menu|Player", "Money", &money, nil, 1000, 0, 99999999, nil);
		DebugMenuAddCmd("Mod Menu|Player", "Give $10,000", AddMoney);
		DebugMenuAddCmd("Mod Menu|Player", "Max Money", MaxMoney);
		DebugMenuAddCmd("Mod Menu|Player", "Full Health", FullHealth);
		DebugMenuAddCmd("Mod Menu|Player", "Full Armour", FullArmour);
		DebugMenuAddCmd("Mod Menu|Player", "Toggle God Mode", ToggleGodMode);
		DebugMenuAddFloat32("Mod Menu|Player", "Health Value", &health, nil, 10.0f, 0.0f, 100.0f);
		DebugMenuAddCmd("Mod Menu|Player", "Apply Health", ApplyHealth);
		DebugMenuAddFloat32("Mod Menu|Player", "Armour Value", &armour, nil, 10.0f, 0.0f, 100.0f);
		DebugMenuAddCmd("Mod Menu|Player", "Apply Armour", ApplyArmour);
		DebugMenuAddInt32("Mod Menu|Player", "Skin Index", &skin, nil, 1, 0, 7, nil);
		DebugMenuAddCmd("Mod Menu|Player", "Apply Skin", ApplySkin);
		DebugMenuAddInt32("Mod Menu|Weapons", "Weapon Index", &weapon, nil, 1, WEAPONTYPE_UNARMED, WEAPONTYPE_CAMERA, nil);
		DebugMenuAddCmd("Mod Menu|Weapons", "Give Selected Weapon", GiveSelectedWeapon);
		DebugMenuAddCmd("Mod Menu|Weapons", "Give All Weapons", GiveAllWeapons);
		DebugMenuAddCmd("Mod Menu|Vehicles", "Toggle Car God Mode", ToggleVehicleGodMode);
		DebugMenuAddCmd("Mod Menu|Spawn", "Spawn Infernus", SpawnInfernus);
		DebugMenuAddCmd("Mod Menu|Spawn", "Spawn Rhino", SpawnRhino);
		DebugMenuAddCmd("Mod Menu|Spawn", "Spawn Police", SpawnPolice);
		std::thread(ProtectEntities).detach();
	}
};

static RegisterMenu registerMenu;

}

#endif
