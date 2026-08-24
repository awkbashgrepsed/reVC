#include "common.h"

#ifdef DEBUGMENU

#include "rtcharse.h"

#include <atomic>
#include <chrono>
#include <thread>

// debugmenu.cpp owns these font objects. The stock normal style has a
// transparent background, which makes the menu nearly impossible to read
// over the game world. Replace only the normal style with the same font
// colours plus a dark opaque background once DebugMenuInit has created it.
extern RtCharset *fontStyles[4];

namespace ReVCModMenuUI {

static void ApplyBackground()
{
	for (;;) {
		if (fontStyles[0] != nil) {
			RwRGBA fg = { 255, 255, 255, 255 };
			RwRGBA bg = { 12, 12, 12, 220 };
			RtCharset *replacement = RtCharsetCreate(&fg, &bg);
			if (replacement != nil) {
				RtCharset *old = fontStyles[0];
				fontStyles[0] = replacement;
				RtCharsetDestroy(old);
			}
			return;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

struct Starter {
	Starter()
	{
		std::thread(ApplyBackground).detach();
	}
};

static Starter starter;

}

#endif
