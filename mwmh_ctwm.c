#include <stdio.h>
#include "mwmh.h"
#ifdef WMH
#include "wmh.h"
#endif				/* WMH */
#ifdef EWMH
#include "ewmh.h"
#endif				/* EWMH */
#include "screen.h"
#include "parse.h"
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/Xmu/CharSet.h>

/** @brief Get the root window for screen.
  * @param scr - screen
  *
  * CTWM messes around with scr->Root: it sets it to the virtual root when
  * virtual roots are being used.
  */
Window
TwmMwmRoot(ScreenInfo *scr)
{
    return scr->XineramaRoot;
}

Window
TwmMwmManager(ScreenInfo *scr)
{
    return scr->ManagerWindow;
}

void
TwmGetMwmInfo(ScreenInfo *scr, MwmInfo *info)
{
    info->flags = MWM_STARTUP_CUSTOM;
    info->wm_window = TwmMwmManager(scr);
}

void
TwmGetMwmHints(ScreenInfo *scr, TwmWindow *twin, MwmHints *hints)
{
    hints->flags =
	MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS | MWM_HINTS_INPUT_MODE |
	MWM_HINTS_STATUS;
    hints->functions =
	(MWM_FUNC_ALL | MWM_FUNC_RESIZE | MWM_FUNC_MOVE | MWM_FUNC_MINIMIZE |
	 MWM_FUNC_MAXIMIZE | MWM_FUNC_CLOSE);
    hints->decorations =
	(MWM_DECOR_ALL | MWM_DECOR_BORDER | MWM_DECOR_RESIZEH | MWM_DECOR_TITLE |
	 MWM_DECOR_MENU | MWM_DECOR_MINIMIZE | MWM_DECOR_MAXIMIZE);
    hints->input_mode = MWM_INPUT_MODELESS;
    hints->status = 0;
}

void
TwmSetMwmHints(ScreenInfo *scr, TwmWindow *twin, MwmHints *hints)
{
    /* munge this a bit */
    if (!(hints->flags & MWM_HINTS_FUNCTIONS)) {
	hints->flags |= MWM_HINTS_FUNCTIONS;
	hints->functions = MWM_FUNC_ALL;
    }
    if (!(hints->flags & MWM_HINTS_DECORATIONS)) {
	hints->flags |= MWM_HINTS_DECORATIONS;
	hints->decorations = MWM_DECOR_ALL;
    }
    if (!(hints->flags & MWM_HINTS_INPUT_MODE)) {
	hints->flags |= MWM_HINTS_INPUT_MODE;
	hints->input_mode = MWM_INPUT_MODELESS;
    }
    if (!(hints->flags & MWM_HINTS_STATUS)) {
	hints->flags |= MWM_HINTS_STATUS;
	hints->status = 0;
    }
    if ((hints->flags & MWM_HINTS_FUNCTIONS) && (hints->functions & MWM_FUNC_ALL)) {
	hints->functions |=
	    (MWM_FUNC_RESIZE | MWM_FUNC_MOVE | MWM_FUNC_MINIMIZE | MWM_FUNC_MAXIMIZE |
	     MWM_FUNC_CLOSE);
    }
    if ((hints->flags & MWM_HINTS_DECORATIONS) && (hints->decorations & MWM_DECOR_ALL)) {
	hints->decorations |=
	    (MWM_DECOR_BORDER | MWM_DECOR_RESIZEH | MWM_DECOR_TITLE | MWM_DECOR_MENU |
	     MWM_DECOR_MINIMIZE | MWM_DECOR_MAXIMIZE);
    }
}

void
TwmSetDtWmHints(TwmWindow *twin, struct DtWmHints *hints)
{
}

void
TwmGetMwmOffset(ScreenInfo *scr, TwmWindow *twin, long *gravity, long *bw, long *x_off, long *y_off) {
    *gravity = NorthWestGravity;
    *bw = twin->frame_bw + twin->frame_bw3D;
    *x_off = *bw;
    *y_off = *bw + twin->title_height;
}


// vim: sw=4 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
