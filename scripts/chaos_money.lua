-- CHAOS MODE
-- F8 toggles a ridiculous money printer.
local chaos = false

function on_start()
    revc.log("CHAOS MONEY loaded - press F8")
end

function on_key(key)
    if key == "F8" then
        chaos = not chaos
        revc.log(chaos and "CHAOS: ON" or "CHAOS: OFF")
    end
end

function on_tick()
    if chaos then
        revc.give_money(100)
        revc.set_health(100.0)
    end
end
