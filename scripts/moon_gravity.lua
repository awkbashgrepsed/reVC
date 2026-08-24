-- Moon gravity
-- Hold SPACE for a huge jump (requires revc.set_gravity/jump_force API).
-- This script is intentionally conservative: it only uses APIs exposed by reVC.

function on_start()
    revc.log("Moon gravity: enabled")
    if revc.set_gravity then
        revc.set_gravity(0.18)
    end
end

function on_key(key)
    if key == "F9" and revc.set_gravity then
        revc.set_gravity(0.18)
        revc.log("MOON MODE")
    elseif key == "F10" and revc.set_gravity then
        revc.set_gravity(1.0)
        revc.log("Earth gravity restored")
    end
end
