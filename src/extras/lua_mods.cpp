#include "common.h"

#include "PlayerPed.h"
#include "World.h"
#include "Streaming.h"
#include "Automobile.h"
#include "ModelIndices.h"

#include <lua.hpp>
#include <atomic>
#include <chrono>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

#ifdef LIBRW_GLFW
#include <GLFW/glfw3.h>
#endif

namespace ReVCLua {
static std::atomic<bool> running{true};
static CPlayerPed *Player() { return CWorld::Players[CWorld::PlayerInFocus].m_pPed; }
static int LuaPrint(lua_State *L) { printf("[Lua] %s\n", luaL_optstring(L, 1, "")); return 0; }
static int LuaGiveMoney(lua_State *L) { CWorld::Players[CWorld::PlayerInFocus].m_nMoney += (int)luaL_checkinteger(L, 1); return 0; }
static int LuaSetHealth(lua_State *L) { if (Player()) Player()->m_fHealth = (float)luaL_checknumber(L, 1); return 0; }
static int LuaSetArmour(lua_State *L) { if (Player()) Player()->m_fArmour = (float)luaL_checknumber(L, 1); return 0; }
static int LuaGiveWeapon(lua_State *L) { if (Player()) Player()->GiveWeapon((eWeaponType)luaL_checkinteger(L, 1), luaL_optinteger(L, 2, 9999)); return 0; }
static int LuaSpawnVehicle(lua_State *L)
{
	int model = (int)luaL_checkinteger(L, 1); if (!Player()) return 0;
	CVector pos = FindPlayerCoors(); CStreaming::RequestModel(model, STREAMFLAGS_DONT_REMOVE); CStreaming::LoadAllRequestedModels(true);
	if (!CStreaming::HasModelLoaded(model)) return 0;
	CAutomobile *vehicle = new CAutomobile(model, RANDOM_VEHICLE); if (!vehicle) return 0;
	pos.z += 0.5f; vehicle->SetPosition(pos); vehicle->SetHeading(FindPlayerHeading()); vehicle->SetStatus(STATUS_PHYSICS); vehicle->SetIsStatic(false); vehicle->bUsesCollision = true; CWorld::Add(vehicle); return 0;
}
static void RegisterAPI(lua_State *L)
{
	lua_newtable(L);
	lua_pushcfunction(L, LuaPrint); lua_setfield(L, -2, "log");
	lua_pushcfunction(L, LuaGiveMoney); lua_setfield(L, -2, "give_money");
	lua_pushcfunction(L, LuaSetHealth); lua_setfield(L, -2, "set_health");
	lua_pushcfunction(L, LuaSetArmour); lua_setfield(L, -2, "set_armour");
	lua_pushcfunction(L, LuaGiveWeapon); lua_setfield(L, -2, "give_weapon");
	lua_pushcfunction(L, LuaSpawnVehicle); lua_setfield(L, -2, "spawn_vehicle");
	lua_setglobal(L, "revc");
}
static std::vector<std::string> FindScripts()
{
	std::vector<std::string> result;
#ifdef _WIN32
	struct _finddata_t data; intptr_t handle = _findfirst("scripts\\*.lua", &data); if (handle == -1) return result;
	do { if (!(data.attrib & _A_SUBDIR)) result.push_back(std::string("scripts\\") + data.name); } while (_findnext(handle, &data) == 0); _findclose(handle);
#else
	DIR *dir = opendir("scripts"); if (!dir) return result; struct dirent *entry;
	while ((entry = readdir(dir)) != nullptr) { std::string name(entry->d_name); if (name.size() > 4 && name.substr(name.size() - 4) == ".lua") result.push_back(std::string("scripts/") + name); }
	closedir(dir);
#endif
	return result;
}
static bool RunScript(lua_State *L, const std::string &path)
{
	if (luaL_dofile(L, path.c_str()) != LUA_OK) { printf("[Lua] %s: %s\n", path.c_str(), lua_tostring(L, -1)); lua_pop(L, 1); return false; } return true;
}
static void CallStart(lua_State *L)
{
	lua_getglobal(L, "on_start"); if (!lua_isfunction(L, -1)) { lua_pop(L, 1); return; }
	if (lua_pcall(L, 0, 0, 0) != LUA_OK) { printf("[Lua] on_start: %s\n", lua_tostring(L, -1)); lua_pop(L, 1); }
}
static void CallKey(lua_State *L, const char *key)
{
	lua_getglobal(L, "on_key"); if (!lua_isfunction(L, -1)) { lua_pop(L, 1); return; } lua_pushstring(L, key);
	if (lua_pcall(L, 1, 0, 0) != LUA_OK) { printf("[Lua] on_key: %s\n", lua_tostring(L, -1)); lua_pop(L, 1); }
}
static void Worker()
{
	std::this_thread::sleep_for(std::chrono::seconds(2));
#ifdef _WIN32
	CreateDirectoryA("scripts", nullptr);
#else
	mkdir("scripts", 0755);
#endif
	for (const std::string &path : FindScripts()) {
		lua_State *L = luaL_newstate(); if (!L) continue; luaL_openlibs(L); RegisterAPI(L);
		if (RunScript(L, path)) { printf("[Lua] Loaded %s\n", path.c_str()); CallStart(L); }
		bool oldKeys[8] = {};
		while (running.load(std::memory_order_relaxed)) {
			static const char *names[8] = { "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12" };
			for (int i = 0; i < 8; ++i) {
				bool down = false;
#ifdef _WIN32
				down = (GetAsyncKeyState(VK_F5 + i) & 0x8000) != 0;
#elif defined(LIBRW_GLFW)
				if (PSGLOBAL(window) != nil) down = glfwGetKey(PSGLOBAL(window), GLFW_KEY_F5 + i) == GLFW_PRESS;
#endif
				if (down && !oldKeys[i]) CallKey(L, names[i]); oldKeys[i] = down;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
		lua_close(L);
	}
}
struct Starter { Starter() { std::thread(Worker).detach(); } } starter;
}
