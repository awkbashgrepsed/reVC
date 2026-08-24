#include "common.h"

#ifdef DEBUGMENU

#include "rtcharse.h"

#include <chrono>
#include <thread>

// Keep normal debug-menu text clean. The old implementation used the charset
// background as a backdrop for every glyph, which made the text look slightly
// corrupted and did not create the large panel we actually want.
extern RtCharset *fontStyles[4];

namespace ReVCModMenuUI {

static void ApplyCleanTextStyle()
{
	for (;;) {
		if (fontStyles[0] != nil) {
			RwRGBA fg = { 255, 255, 255, 255 };
			RwRGBA bg = { 0, 0, 0, 0 };
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
		std::thread(ApplyCleanTextStyle).detach();
	}
};

static Starter starter;

}

#endif
