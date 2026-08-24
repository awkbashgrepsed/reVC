-- Money rain
-- Press F11 to repeatedly shower Tommy with cash.
local enabled = false

function on_start()
    revc.log("Money rain loaded - press F11")
end

function on_key(key)
    if key == "F11" then
        enabled = not enabled
        revc.log(enabled and "MONEY RAIN: ON" or "MONEY RAIN: OFF")
    end
end

function on_tick()
    if enabled then
        revc.give_money(1000)
    end
end
