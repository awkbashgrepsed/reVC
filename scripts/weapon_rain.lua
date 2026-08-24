-- Weapon rain
-- Press F12 to cycle through a few ridiculous weapon grants.
local ammo = 999
local weapon = 13

function on_start()
    revc.log("Weapon rain loaded - press F12")
end

function on_key(key)
    if key == "F12" then
        revc.give_weapon(weapon, ammo)
        revc.log("Gave weapon " .. weapon .. " with " .. ammo .. " ammo")
        weapon = weapon + 1
        if weapon > 18 then
            weapon = 13
        end
    end
end
