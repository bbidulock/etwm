#ifndef _MWMH_H_
#define _MWMH_H_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Intrinsic.h>
#include <X11/cursorfont.h>
#include <X11/extensions/shape.h>
#include <X11/Xfuncs.h>

#include "types.h"

extern Atom _XA_WM_DESKTOP;
extern Atom _XA_MOTIF_BINDINGS;
extern Atom _XA_MOTIF_DEFAULT_BINDINGS;
extern Atom _XA_MOTIF_WM_HINTS;
extern Atom _XA_MOTIF_WM_MESSAGES;
extern Atom _XA_MOTIF_WM_OFFSET;
extern Atom _XA_MOTIF_WM_MENU;
extern Atom _XA_MOTIF_WM_INFO;

extern Atom _XA_DT_WORKSPACE_HINTS;
extern Atom _XA_DT_WORKSPACE_PRESENCE;
extern Atom _XA_DT_WORKSPACE_LIST;
extern Atom _XA_DT_WORKSPACE_CURRENT;
extern Atom _XA_DT_WORKSPACE_INFO;
extern Atom _XA_DT_WM_HINTS;
extern Atom _XA_DT_WM_REQUEST;
extern Atom _XA_DT_WORKSPACE_EMBEDDED_CLIENTS;
extern Atom _XA_DT_WMSAVE_HINT;

extern Atom _XA_MOTIF_DRAG_WINDOW;
extern Atom _XA_MOTIF_DRAG_PROXY_WINDOW;
extern Atom _XA_MOTIF_DRAG_ATOM_PAIRS;
extern Atom _XA_MOTIF_DRAG_TARGETS;
extern Atom _XA_MOTIF_DRAG_INITIATOR_INFO;
extern Atom _XA_MOTIF_DRAG_RECEIVER_INFO;
extern Atom _XA_MOTIF_DRAG_MESSAGE;
extern Atom _XA_MOTIF_DRAG_AND_DROP_MESSAGE;

/* ICCCM Protocols */
extern Atom _XA_WM_PROTOCOLS;
extern Atom _XA_WM_TAKE_FOCUS;
extern Atom _XA_WM_SAVE_YOURSELF;
extern Atom _XA_WM_DELETE_WINDOW;

typedef struct MotifWMInfo {
    unsigned long flags;
    Window wm_window;
} MotifWMInfo, MwmInfo;

/* flags field for _MOTIF_WM_INFO */
enum MWM_STARTUP {
    MWM_STARTUP_STANDARD_BIT = 0,
    MWM_STARTUP_CUSTOM_BIT
};

#define MWM_STARTUP_STANDARD	(1L << MWM_STARTUP_STANDARD_BIT)
#define MWM_STARTUP_CUSTOM	(1L << MWM_STARTUP_CUSTOM_BIT)

typedef struct MotifWMHints {
    unsigned long flags, functions, decorations;
    long input_mode;
    unsigned long status;
} MotifWMHints, MwmHints;

/* flags field for _MOTIF_WM_HINTS */
enum MWM_HINTS {
    MWM_HINTS_FUNCTIONS_BIT = 0,
    MWM_HINTS_DECORATIONS_BIT,
    MWM_HINTS_INPUT_MODE_BIT,
    MWM_HINTS_STATUS_BIT
};

#define MWM_HINTS_FUNCTIONS	(1L << MWM_HINTS_FUNCTIONS_BIT)
#define MWM_HINTS_DECORATIONS	(1L << MWM_HINTS_DECORATIONS_BIT)
#define MWM_HINTS_INPUT_MODE	(1L << MWM_HINTS_INPUT_MODE_BIT)
#define MWM_HINTS_STATUS	(1L << MWM_HINTS_STATUS_BIT)

/* functions field for _MOTIF_WM_HINTS */
enum MWM_FUNC {
    MWM_FUNC_ALL_BIT = 0,
    MWM_FUNC_RESIZE_BIT,	/* like _NET_WM_ACTION_RESIZE */
    MWM_FUNC_MOVE_BIT,		/* like _NET_WM_ACTION_MOVE */
    MWM_FUNC_MINIMIZE_BIT,	/* like _NET_WM_ACTION_MINIMIZE */
    MWM_FUNC_MAXIMIZE_BIT,	/* like _NET_WM_ACTION_(FULLSCREEN|MAXIMIZE_(HORZ|VERT)) */
    MWM_FUNC_CLOSE_BIT		/* like _NET_WM_ACTION_CLOSE */
};

#define MWM_FUNC_ALL		(1L << MWM_FUNC_ALL_BIT)
#define MWM_FUNC_RESIZE		(1L << MWM_FUNC_RESIZE_BIT)
#define MWM_FUNC_MOVE		(1L << MWM_FUNC_MOVE_BIT)
#define MWM_FUNC_MINIMIZE	(1L << MWM_FUNC_MINIMIZE_BIT)
#define MWM_FUNC_MAXIMIZE	(1L << MWM_FUNC_MAXIMIZE_BIT)
#define MWM_FUNC_CLOSE		(1L << MWM_FUNC_CLOSE_BIT)

/* decorations field for _MOTIF_WM_HINTS */
enum MWM_DECOR {
    MWM_DECOR_ALL_BIT = 0,
    MWM_DECOR_BORDER_BIT,
    MWM_DECOR_RESIZEH_BIT,
    MWM_DECOR_TITLE_BIT,
    MWM_DECOR_MENU_BIT,
    MWM_DECOR_MINIMIZE_BIT,
    MWM_DECOR_MAXIMIZE_BIT
};

#define MWM_DECOR_ALL		(1L << MWM_DECOR_ALL_BIT)
#define MWM_DECOR_BORDER	(1L << MWM_DECOR_BORDER_BIT)
#define MWM_DECOR_RESIZEH	(1L << MWM_DECOR_RESIZEH_BIT)
#define MWM_DECOR_TITLE		(1L << MWM_DECOR_TITLE_BIT)
#define MWM_DECOR_MENU		(1L << MWM_DECOR_MENU_BIT)
#define MWM_DECOR_MINIMIZE	(1L << MWM_DECOR_MINIMIZE_BIT)
#define MWM_DECOR_MAXIMIZE	(1L << MWM_DECOR_MAXIMIZE_BIT)

/* input_mode field for _MOTIF_WM_HINTS */
enum MWM_INPUT {
    MWM_INPUT_MODELESS = 0,
    MWM_INPUT_PRIMARY_APPLICATION_MODAL = 1,
    MWM_INPUT_SYSTEM_MODAL = 2,
    MWM_INPUT_FULL_APPLICATION_MODAL = 3,
    MWM_INPUT_APPLICATION_MODAL = 1
};

/* status field for _MOTIF_WM_HINTS */
enum MWM_STATUS {
    MWM_TEAROFF_WINDOW_BIT = 0
};

#define MWM_TEAROFF_WINDOW	(1L << MWM_TEAROFF_WINDOW_BIT)

/* bit defintitions for _DT_WMSAVE_HINT */
enum WMSAVE_HINT {
    WMSAVE_X_BIT = 0,
    WMSAVE_Y_BIT,
    WMSAVE_WIDTH_BIT,
    WMSAVE_HEIGHT_BIT,
    WMSAVE_STATE_BIT,
    WMSAVE_WORKSPACES_BIT,
    WMSAVE_ICON_X_BIT,
    WMSAVE_ICON_Y_BIT
};

#define WMSAVE_X		(1L << WMSAVE_X_BIT)
#define WMSAVE_Y		(1L << WMSAVE_Y_BIT)
#define WMSAVE_WIDTH		(1L << WMSAVE_WIDTH_BIT)
#define WMSAVE_HEIGHT		(1L << WMSAVE_HEIGHT_BIT)
#define WMSAVE_STATE		(1L << WMSAVE_STATE_BIT)
#define WMSAVE_WORKSPACES	(1L << WMSAVE_WORKSPACES_BIT)
#define WMSAVE_ICON_X		(1L << WMSAVE_ICON_X_BIT)
#define WMSAVE_ICON_Y		(1L << WMSAVE_ICON_Y_BIT)

typedef struct DtWmHints {
    unsigned long flags, functions, behaviors;
    Window attachWindow;
} DtWmHints;

/* flags bits for _DT_WM_HINTS */
enum DTWM_HINTS {
    DTWM_HINTS_FUNCTIONS_BIT = 0,
    DTWM_HINTS_BEHAVIORS_BIT,
    DTWM_HINTS_ATTACH_WINDOW_BIT
};

#define DTWM_HINTS_FUNCTIONS		(1L << DTWM_HINTS_FUNCTIONS_BIT)
#define DTWM_HINTS_BEHAVIORS		(1L << DTWM_HINTS_BEHAVIORS_BIT)
#define DTWM_HINTS_ATTACH_WINDOW	(1L << DTWM_HINTS_ATTACH_WINDOW_BIT)

/* functions bits for _DT_WM_HINTS */
enum DTWM_FUNCTION {
    DTWM_FUNCTION_ALL_BIT = 0,
    DTWM_FUNCTION_OCCUPY_WS_BIT = 16	/* like _NET_WM_ACTION_CHANGE_DEKSTOP */
};

#define DTWM_FUNCTION_ALL		(1L << DTWM_FUNCTION_ALL_BIT)
#define DTWM_FUNCTION_OCCUPY_WS		(1L << DTWM_FUNCTION_OCCUPY_WS_BIT)

/* behaviors bits for _DT_WM_HINTS */
enum DTWM_BEHAVIOR {
    DTWM_BEHAVIOR_PANEL_BIT = 1,
    DTWM_BEHAVIOR_SUBPANEL_BIT,
    DTWM_BEHAVIOR_SUB_RESTORED_BIT
};

#define DTWM_BEHAVIOR_PANEL		(1L << DTWM_BEHAVIOR_PANEL_BIT)
#define DTWM_BEHAVIOR_SUBPANEL		(1L << DTWM_BEHAVIOR_SUBPANEL_BIT)
#define DTWM_BEHAVIOR_SUB_RESTORED	(1L << DTWM_BEHAVIOR_SUB_RESTORED_BIT)

typedef struct DtWmWorkspaceHints {
    unsigned long version, flags, wsflags, numWorkspaces;
    Atom *workspaces;
} DtWmWorkspaceHints;

/* flags bits for _DT_WORKSPACE_HINTS */
enum DT_WORKSPACE_HINTS {
    DT_WORKSPACE_HINTS_WSFLAGS_BIT = 0,
    DT_WORKSPACE_HINTS_WORKSPACES_BIT
};

#define DT_WORKSPACE_HINTS_WSFLAGS	(1L << DT_WORKSPACE_HINTS_WSFLAGS_BIT)
#define DT_WORKSPACE_HINTS_WORKSPACES	(1L << DT_WORKSPACE_HINTS_WORKSPACES_BIT)

enum DT_WORKSPACE_FLAGS {
    DT_WORKSPACE_FLAGS_OCCUPY_ALL_BIT = 0
};

#define DT_WORKSPACE_FLAGS_OCCUPY_ALL	(1L << DT_WORKSPACE_FLAGS_OCCUPY_ALL_BIT)

#define MWM_ATOM_ENTRY(atom) TWM_ATOM_ENTRY(atom)

typedef struct MwmWindowBits {
    unsigned WM_PROTOCOLS:1;
    unsigned WM_DESKTOP:1;
    unsigned _MOTIF_BINDINGS:1;
    unsigned _MOTIF_DEFAULT_BINDINGS:1;
    unsigned _MOTIF_WM_HINTS:1;
    unsigned _MOTIF_WM_MESSAGES:1;
    unsigned _MOTIF_WM_OFFSET:1;
    unsigned _MOTIF_WM_MENU:1;
    unsigned _DT_WORKSPACE_HINTS:1;
    unsigned _DT_WORKSPACE_PRESENCE:1;
    unsigned _DT_WM_HINTS:1;
    unsigned _MOTIF_DRAG_WINDOW:1;
    unsigned _MOTIF_DRAG_PROXY_WINDOW:1;
    unsigned _MOTIF_DRAG_ATOM_PAIRS:1;
    unsigned _MOTIF_DRAG_TARGETS:1;
    unsigned _MOTIF_DRAG_INITIATOR_INFO:1;
    unsigned _MOTIF_DRAG_RECEIVER_INFO:1;
    unsigned _MOTIF_DRAG_MESSAGE:1;
    unsigned _MOTIF_DRAG_AND_DROP_MESSAGE:1;
} MwmWindowBits;

typedef struct MwmWindow {
    struct MwmWindowBits props;
    unsigned protocols;			/* WM_PROTOCOLS */
    unsigned desktop;			/* WM_DESKTOP */
    char *bindings;			/* _MOTIF_BINDINGS */
    char *default_bindings;		/* _MOTIF_DEFAULT_BINDINGS */
    struct MotifWMHints hints;		/* _MOTIF_WM_HINTS */
    Bool messages;			/* _MOTIF_WM_MESSAGES */
    Bool offset;			/* _MOTIF_WM_OFFSET */
    char *menu;				/* _MOTIF_WM_MENU */
    struct DtWmWorkspaceHints wshints;	/* _DT_WORKSPACE_HINTS */
    Atom *presence;			/* _DT_WORKSPACE_PRESENCE */
    struct DtWmHints dthints;		/* _DT_WM_HINTS */
} MwmWindow;

typedef struct MwmScreenBits {
    unsigned _MOTIF_WM_INFO:1;
    unsigned _DT_WORKSPACE_LIST:1;
    unsigned _DT_WORKSPACE_CURRENT:1;
    unsigned _DT_WORKSPACE_INFO:1;
    unsigned _DT_WM_REQUEST:1;
    unsigned _DT_WORKSPACE_EMBEDDED_CLIENTS:1;
} MwmScreenBits;

typedef struct MwmScreen {
    struct MwmScreenBits props;		/* which properties are present */
    struct MotifWMInfo info;		/* _MOTIF_WM_INFO */
    Atom *list;				/* _DT_WORKSPACE_LIST */
    Atom current;			/* _DT_WORKSPACE_CURRENT */
    char **request;			/* _DT_WM_REQUEST */
    Window *embedded;			/* _DT_WORKSPACE_EMBEDDED_CLIENTS */
} MwmScreen;

void Upd_MOTIF_WM_INFO(ScreenInfo *scr);
void Upd_DT_WORKSPACE_LIST(ScreenInfo *scr);
void Upd_DT_WORKSPACE_CURRENT(ScreenInfo *scr);
void Upd_MOTIF_WM_HINTS(ScreenInfo *scr, TwmWindow *twin);
void Upd_DT_WORKSPACE_HINTS(ScreenInfo *scr, TwmWindow *twin);
void Upd_DT_WORKSPACE_PRESENCE(ScreenInfo *scr, TwmWindow *twin);

void Snd_MOTIF_WM_OFFSET(ScreenInfo *scr, TwmWindow *twin);

void InitMwmh(ScreenInfo *scr);
void UpdateMwmh(ScreenInfo *scr);
void RestartMwmh(ScreenInfo *scr);
void TermMwmh(ScreenInfo *scr);

void AddWindowMwmh(ScreenInfo *scr, TwmWindow *twin);
void UpdWindowMwmh(ScreenInfo *scr, TwmWindow *twin);
void DelWindowMwmh(ScreenInfo *scr, TwmWindow *twin);

Bool HandleMwmClientMessage(ScreenInfo *scr, TwmWindow *twin, XEvent *xev);
Bool HandleMwmPropertyNotify(ScreenInfo *scr, TwmWindow *twin, XEvent *xev);

Window TwmMwmRoot(ScreenInfo *scr);
Window TwmMwmManager(ScreenInfo *scr);

void TwmGetMwmInfo(ScreenInfo *scr, MwmInfo *info);
void TwmGetMwmHints(ScreenInfo *scr, TwmWindow *twin, MwmHints *hints);
void TwmSetMwmHints(ScreenInfo *scr, TwmWindow *twin, MwmHints *hints);
void TwmSetDtWmHints(TwmWindow *twin, struct DtWmHints *hints);
void TwmGetMwmOffset(ScreenInfo *scr, TwmWindow *twin, long *gravity, long *bw, long *x_off, long *y_off);
void TwmGetWorkspacePresence(ScreenInfo *scr, TwmWindow *twin, Atom **presence, int *count);
void TwmGetWorkspaceCurrent(ScreenInfo *scr, Atom *current);
void TwmGetWorkspaceList(ScreenInfo *scr, Atom **list, int *count);



#endif				/* _MWMH_H_ */

// vim: sw=4 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
