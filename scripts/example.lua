-- reVC Lua mod example
-- Scripts are loaded automatically from scripts/*.lua

function on_start()
    revc.log("example.lua loaded")
end

function on_key(key)
    if key == "F5" then
        revc.give_money(10000)
        revc.log("Gave Tommy $10,000")
    elseif key == "F6" then
        revc.set_health(100.0)
        revc.set_armour(100.0)
        revc.log("Restored health and armour")
    elseif key == "F7" then
        -- 130 is an example weapon ID. Vehicle IDs can be passed to spawn_vehicle.
        revc.give_weapon(13, 9999)
    elseif key == "F8" then
        -- Example vehicle model ID.
        revc.spawn_vehicle(130)
    end
end
