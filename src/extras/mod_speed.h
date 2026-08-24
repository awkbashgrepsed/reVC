#pragma once

#ifdef DEBUGMENU

// Mod-menu movement controls. The implementation lives in Pad.cpp so the
// controls are sampled before the normal player control code runs.
namespace ReVCModSpeed {
void Update();
}

#endif
