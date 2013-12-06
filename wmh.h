#ifndef _WMH_H_
#define _WMH_H_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Intrinsic.h>
#include <X11/cursorfont.h>
#include <X11/extensions/shape.h>
#include <X11/Xfuncs.h>

#include "types.h"

/* 1.1 Detection of GNOME compliant window manager */
extern Atom _XA_WIN_SUPPORTING_WM_CHECK;

/* 1.2 Listing GNOME window manager compliance */
extern Atom _XA_WIN_PROTOCOLS;

/* 1.3 Providing shortcuts managed clients */
extern Atom _XA_WIN_CLIENT_LIST;

/* 1.4 Providing multiple/virtual desktop information */
extern Atom _XA_WIN_WORKSPACE_COUNT;
extern Atom _XA_WIN_WORKSPACE;
extern Atom _XA_WIN_WORKSPACE_NAMES;

/* 2.1 Initial properties set on client window */
extern Atom _XA_WIN_LAYER;
extern Atom _XA_WIN_STATE;
extern Atom _XA_WIN_WORKSPACE;
extern Atom _XA_WIN_EXPANDED_SIZE;
extern Atom _XA_WIN_HINTS;

/* 2.2 State change requests */
extern Atom _XA_WIN_LAYER;
extern Atom _XA_WIN_STATE;
extern Atom _XA_WIN_WORKSPACE;

/* 3.1 Button press and release forwarding for the desktop window. */
extern Atom _XA_WIN_DESKTOP_BUTTON_PROXY;

/* 3.2 Desktop areas as opposed to multiple desktops. */
extern Atom _XA_WIN_AREA_COUNT;
extern Atom _XA_WIN_AREA;

/* mentioned by not defined by wm-comp */
extern Atom _XA_WIN_APP_STATE;
extern Atom _XA_WIN_ICONS;

/* not mentioned by wm-comp but used by window managers, e.g. icewm */
extern Atom _XA_WIN_WORKAREA;
extern Atom _XA_WIN_WORKSPACES;
extern Atom _XA_WIN_CLIENT_MOVING;

/* others */
extern Atom _XA_WIN_MAXIMIZED_GEOMETRY;
extern Atom _XA_WIN_RESIZE;
extern Atom _XA_WIN_FOCUS;

/* values for _WIN_STATE */
enum _WIN_STATE {
    WIN_STATE_STICKY = 0,		/* everyone knows sticky */
    WIN_STATE_MINIMIZED,	/* Reserved - definition is unclear */
    WIN_STATE_MAXIMIZED_VERT,	/* window in maximized V state */
    WIN_STATE_MAXIMIZED_HORIZ,	/* window in maximized H state */
    WIN_STATE_HIDDEN,		/* not on taskbar but window visible */
    WIN_STATE_SHADED,		/* shaded (MacOS / Afterstep style) */
    WIN_STATE_HID_WORKSPACE,	/* not on current desktop */
    WIN_STATE_HID_TRANSIENT,	/* owner of transient is hidden */
    WIN_STATE_FIXED_POSITION,	/* window is fixed in position even */
    WIN_STATE_ARRANGE_IGNORE	/* ignore for auto arranging */
};

#define WIN_STATE_STICKY_BIT		(1<<WIN_STATE_STICKY)
#define WIN_STATE_MINIMIZED_BIT		(1<<WIN_STATE_MINIMIZED)
#define WIN_STATE_MAXIMIZED_VERT_BIT	(1<<WIN_STATE_MAXIMIZED_VERT)
#define WIN_STATE_MAXIMIZED_HORIZ_BIT	(1<<WIN_STATE_MAXIMIZED_HORIZ)
#define WIN_STATE_HIDDEN_BIT		(1<<WIN_STATE_HIDDEN)
#define WIN_STATE_SHADED_BIT		(1<<WIN_STATE_SHADED)
#define WIN_STATE_HID_WORKSPACE_BIT	(1<<WIN_STATE_HID_WORKSPACE)
#define WIN_STATE_HID_TRANSIENT_BIT	(1<<WIN_STATE_HID_TRANSIENT)
#define WIN_STATE_FIXED_POSITION_BIT	(1<<WIN_STATE_FIXED_POSITION)
#define WIN_STATE_ARRANGE_IGNORE_BIT	(1<<WIN_STATE_ARRANGE_IGNORE)

/* values for _WIN_HINTS */
enum _WIN_HINTS {
    WIN_HINTS_SKIP_FOCUS = 0,
    WIN_HINTS_SKIP_WINLIST,
    WIN_HINTS_SKIP_TASKBAR,
    WIN_HINTS_GROUP_TRANSIENT,
    WIN_HINTS_FOCUS_ON_CLICK,
    WIN_HINTS_DO_NOT_COVER
};

#define WIN_HINTS_SKIP_FOCUS_BIT	(1<<WIN_HINTS_SKIP_FOCUS)
#define WIN_HINTS_SKIP_WINLIST_BIT	(1<<WIN_HINTS_SKIP_WINLIST)
#define WIN_HINTS_SKIP_TASKBAR_BIT	(1<<WIN_HINTS_SKIP_TASKBAR)
#define WIN_HINTS_GROUP_TRANSIENT_BIT	(1<<WIN_HINTS_GROUP_TRANSIENT)
#define WIN_HINTS_FOCUS_ON_CLICK_BIT	(1<<WIN_HINTS_FOCUS_ON_CLICK)
#define WIN_HINTS_DO_NOT_COVER_BIT	(1<<WIN_HINTS_DO_NOT_COVER)

/* values for _WIN_LAYER */
enum _WIN_LAYER {
    WIN_LAYER_DESKTOP = 0,
    WIN_LAYER_BELOW = 2,
    WIN_LAYER_NORMAL = 4,
    WIN_LAYER_ONTOP = 6,
    WIN_LAYER_DOCK = 8,
    WIN_LAYER_ABOVE_DOCK = 10,
    WIN_LAYER_MENU = 12
};

/* values for _WIN_APP_STATE */
enum {
    WIN_APP_STATE_NONE = 0,
    WIN_APP_STATE_ACTIVE1,
    WIN_APP_STATE_ACTIVE2,
    WIN_APP_STATE_ERROR1,
    WIN_APP_STATE_ERROR2,
    WIN_APP_STATE_FATAL_ERROR1,
    WIN_APP_STATE_FATAL_ERROR2,
    WIN_APP_STATE_IDLE1,
    WIN_APP_STATE_IDLE2,
    WIN_APP_STATE_WAITING1,
    WIN_APP_STATE_WAITING2,
    WIN_APP_STATE_WORKING1,
    WIN_APP_STATE_WORKING2,
    WIN_APP_STATE_NEED_USER_INPUT1,
    WIN_APP_STATE_NEED_USER_INPUT2,
    WIN_APP_STATE_STRUGGLING1,
    WIN_APP_STATE_STRUGGLING2,
    WIN_APP_STATE_DISK_TRAFFIC1,
    WIN_APP_STATE_DISK_TRAFFIC2,
    WIN_APP_STATE_NETWORK_TRAFFIC1,
    WIN_APP_STATE_NETWORK_TRAFFIC2,
    WIN_APP_STATE_OVERLOADED1,
    WIN_APP_STATE_OVERLOADED2,
    WIN_APP_STATE_PERCENT000_1,
    WIN_APP_STATE_PERCENT000_2,
    WIN_APP_STATE_PERCENT010_1,
    WIN_APP_STATE_PERCENT010_2,
    WIN_APP_STATE_PERCENT020_1,
    WIN_APP_STATE_PERCENT020_2,
    WIN_APP_STATE_PERCENT030_1,
    WIN_APP_STATE_PERCENT030_2,
    WIN_APP_STATE_PERCENT040_1,
    WIN_APP_STATE_PERCENT040_2,
    WIN_APP_STATE_PERCENT050_1,
    WIN_APP_STATE_PERCENT050_2,
    WIN_APP_STATE_PERCENT060_1,
    WIN_APP_STATE_PERCENT060_2,
    WIN_APP_STATE_PERCENT070_1,
    WIN_APP_STATE_PERCENT070_2,
    WIN_APP_STATE_PERCENT080_1,
    WIN_APP_STATE_PERCENT080_2,
    WIN_APP_STATE_PERCENT090_1,
    WIN_APP_STATE_PERCENT090_2,
    WIN_APP_STATE_PERCENT100_1,
    WIN_APP_STATE_PERCENT100_2
};

typedef struct WinArea {
    long col, row;
} WinArea;

typedef struct WinLayout {
    long cols, rows;
} WinLayout;

typedef struct WinGeometry {
    long x, y, width, height;
} WinGeometry;

typedef struct WinWorkarea {
    long x_min, y_min, x_max, y_max;
} WinWorkarea;

typedef struct WinWindowBits {
    unsigned _WIN_WORKSPACE:1;
    unsigned _WIN_LAYER:1;
    unsigned _WIN_STATE:1;
    unsigned _WIN_EXPANDED_SIZE:1;
    unsigned _WIN_HINTS:1;
    unsigned _WIN_APP_STATE:1;
    unsigned _WIN_ICONS:1;
    unsigned _WIN_WORKSPACES:1;
    unsigned _WIN_CLIENT_MOVING:1;
    unsigned _WIN_MAXIMIZED_GEOMETRY:1;
} WinWindowBits;

typedef struct WmhWindow {
    struct WinWindowBits props;		/* mask of properties on window */
    int workspace;			/* _WIN_WORKSPACE */
    unsigned layer;			/* _WIN_LAYER */
    unsigned state;			/* _WIN_STATE */
    struct WinGeometry expanded;	/* _WIN_EXPANDED_SIZE */
    unsigned hints;			/* _WIN_HINTS */
    unsigned app_state;			/* _WIN_APP_STATE */
    long *icons;			/* _WIN_ICONS */
    long *mask;				/* _WIN_WORKSPACES */
    int masks;				/* _WIN_WORKSPACES */
    Bool moving;			/* _WIN_CLIENT_MOVING */
    struct WinGeometry maximized;	/* _WIN_MAXIMIZED_GEOMETRY */
    struct WinWorkarea workarea;	/* _WIN_WORKAREA */
} WmhWindow;

typedef struct WinScreenBits {
    unsigned _WIN_SUPPORTING_WM_CHECK:1;
    unsigned _WIN_PROTOCOLS:1;
    unsigned _WIN_CLIENT_LIST:1;
    unsigned _WIN_WORKSPACE_COUNT:1;
    unsigned _WIN_WORKSPACE:1;
    unsigned _WIN_WORKSPACE_NAMES:1;
    unsigned _WIN_DESKTOP_BUTTON_PROXY:1;
    unsigned _WIN_AREA_COUNT:1;
    unsigned _WIN_AREA:1;
    unsigned _WIN_WORKAREA:1;
    unsigned __SWM_VROOT:1;
} winScreenBits;

typedef struct WmhScreen {
    struct WinScreenBits props;		/* mask of properties on root */
    Window check;			/* _WIN_SUPPORTING_WM_CHECK */
    unsigned protocols;			/* _WIN_PROTOCOLS */
    Window *clients;			/* _WIN_CLIENT_LIST */
    int workspace_count;		/* _WIN_WORKSPACE_COUNT */
    int workspace;			/* _WIN_WORKSPACE */
    char **names;		/* _WIN_WORKSPACE_NAMES */
    Window proxy;			/* _WIN_DESKTOP_BUTTON_PROXY */
    struct WinLayout area_count;	/* _WIN_AREA_COUNT */
    struct WinArea area;		/* _WIN_AREA */
    struct WinWorkarea workarea;	/* _WIN_WORKAREA */
    Window *vroots;			/* __SWM_ROOT */
} WmhScreen;

void Upd_WIN_SUPPORTING_WM_CHECK(ScreenInfo *scr);
void Upd_WIN_PROTOCOLS(ScreenInfo *scr);
void Upd_WIN_CLIENT_LIST(ScreenInfo *scr);
void Upd_WIN_WORKSPACE_COUNT(ScreenInfo *scr);
void Upd_WIN_WORKSPACE_root(ScreenInfo *scr);
void Upd_WIN_WORKSPACE(ScreenInfo *scr, TwmWindow *twin);
void Upd_WIN_WORKSPACE_NAMES(ScreenInfo *scr);
void Upd_WIN_LAYER(TwmWindow *twin);
void Upd_WIN_STATE(TwmWindow *twin);
void Upd_WIN_DESKTOP_BUTTON_PROXY(ScreenInfo *scr);
void Upd_WIN_AREA_COUNT(ScreenInfo *scr);
void Upd_WIN_AREA(ScreenInfo *scr);
void Upd_WIN_WORKAREA(ScreenInfo *scr);
void Upd_WIN_WORKSPACES(ScreenInfo *scr, TwmWindow *twin);
void Upd_WIN_MAXIMIZED_GEOMETRY(ScreenInfo *scr, TwmWindow *twin);
void Upd__SWM_VROOT(ScreenInfo *scr);

void Snd_WIN_DESKTOP_BUTTON_PROXY(ScreenInfo *scr, XEvent *event);

void InitWmh(ScreenInfo *scr);
void UpdateWmh(ScreenInfo *scr);
void RestartWmh(ScreenInfo *scr);
void TermWmh(ScreenInfo *scr);

void AddWindowWmh(ScreenInfo *scr, TwmWindow *twin);
void UpdWindowWmh(ScreenInfo *scr, TwmWindow *twin);
void DelWindowWmh(ScreenInfo *scr, TwmWindow *twin);

Bool HandleWinClientMessage(ScreenInfo *scr, TwmWindow *twin, XEvent *event);
Bool HandleWinPropertyNotify(ScreenInfo *scr, TwmWindow *twin, XEvent *event);

Window TwmWinRoot(ScreenInfo *scr);

/* _WIN_CLIENT_LIST */
void TwmGetWinClientList(ScreenInfo *scr, Window **windows, int *count);

/* _WIN_WORKSPACE_COUNT */
void TwmGetWorkspaceCount(ScreenInfo *scr, int *count);
void TwmSetWorkspaceCount(ScreenInfo *scr, int count);

/* _WIN_WORKSPACE */
void TwmGetWorkspace(ScreenInfo *scr, int *workspace);
void TwmSetWorkspace(ScreenInfo *scr, int workspace);
void TwmGetWinWorkspace(ScreenInfo *scr, TwmWindow *twin, int *workspace);
void TwmSetWinWorkspace(ScreenInfo *scr, TwmWindow *twin, int workspace);

/* _WIN_WORKSPACE_NAMES */
void TwmGetWorkspaceNames(ScreenInfo *scr, char ***names, int *count);
void TwmSetWorkspaceNames(ScreenInfo *scr, char **names, int count);

/* _WIN_LAYER */
void TwmGetWinLayer(TwmWindow *twin, unsigned *layer);
void TwmSetWinLayer(TwmWindow *twin, unsigned layer);

/* _WIN_STATE */
void TwmGetWinState(TwmWindow *twin, unsigned *state);
void TwmSetWinState(ScreenInfo *scr, TwmWindow *twin, unsigned mask, unsigned state);

/* _WIN_EXPANDED_SIZE */
void TwmSetExpandedSize(TwmWindow *twin, struct WinGeometry *expanded);
void TwmDelExpandedSize(TwmWindow *twin, struct WinGeometry *expanded);

/* _WIN_HINTS */
void TwmSetWinHints(TwmWindow *twin, unsigned hints);
void TwmDelWinHints(TwmWindow *twin, unsigned *hints);

/* _WIN_AREA_COUNT */
void TwmGetWinAreaCount(ScreenInfo *scr, struct WinLayout *count);
void TwmSetWinAreaCount(ScreenInfo *scr, struct WinLayout *count);

/* _WIN_AREA */
void TwmGetWinArea(ScreenInfo *scr, struct WinArea *area);
void TwmSetWinArea(ScreenInfo *scr, struct WinArea *area);

/* _WIN_APP_STATE */
void TwmSetWinAppState(TwmWindow *twin, unsigned app_state);
void TwmDelWinAppState(TwmWindow *twin, unsigned *app_state);

/* _WIN_ICONS */
void TwmSetWinIcons(TwmWindow *twin, long *icons);

/* _WIN_WORKAREA */
void TwmGetWinWorkarea(ScreenInfo *scr, struct WinWorkarea *workarea);

/* _WIN_WORKSPACES */
void TwmGetWMWorkspaces(ScreenInfo *scr, TwmWindow *twin, long **mask, int *masks);
void TwmSetWMWorkspaces(ScreenInfo *scr, TwmWindow *twin, long *mask, int masks);
void TwmChgWMWorkspaces(ScreenInfo *scr, TwmWindow *twin, unsigned index, unsigned mask);

/* _WIN_CLIENT_MOVING */
void TwmSetMoving(TwmWindow *twin, Bool moving);

/* _WIN_MAXIMIZED_GEOMETRY */
void TwmGetMaximizedGeometry(ScreenInfo *scr, TwmWindow *twin, struct WinGeometry *maximized);
void TwmSetMaximizedGeometry(ScreenInfo *scr, TwmWindow *twin, struct WinGeometry *maximized);

/* __SWM_VROOT */
void TwmGetSwmVroots(ScreenInfo *scr, Window **vroots, int *count);

#endif				/* _WMH_H_ */

// vim: sw=4 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
