#ifndef _EWMH_H_
#define _EWMH_H_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Intrinsic.h>
#include <X11/cursorfont.h>
#include <X11/extensions/shape.h>
#include <X11/Xfuncs.h>

#include "types.h"

extern Atom _XA_UTF8_STRING;

extern Atom _XA_KDE_NET_SYSTEM_TRAY_WINDOWS;
extern Atom _XA_KDE_NET_WM_FRAME_STRUT;
extern Atom _XA_KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR;
extern Atom _XA_KDE_NET_WM_WINDOW_TYPE_OVERRIDE;

/* Root Window Properties (and Related messages) */
extern Atom _XA_NET_SUPPORTED;
extern Atom _XA_NET_CLIENT_LIST;
extern Atom _XA_NET_CLIENT_LIST_STACKING;
extern Atom _XA_NET_NUMBER_OF_DESKTOPS;
extern Atom _XA_NET_DESKTOP_GEOMETRY;
extern Atom _XA_NET_DESKTOP_VIEWPORT;
extern Atom _XA_NET_CURRENT_DESKTOP;
extern Atom _XA_NET_DESKTOP_NAMES;
extern Atom _XA_NET_ACTIVE_WINDOW;
extern Atom _XA_NET_WORKAREA;
extern Atom _XA_NET_SUPPORTING_WM_CHECK;
extern Atom _XA_NET_VIRTUAL_ROOTS;
extern Atom _XA_NET_DESKTOP_LAYOUT;
extern Atom _XA_NET_SHOWING_DESKTOP;

/* Other Root Window Messages */
extern Atom _XA_NET_CLOSE_WINDOW;
extern Atom _XA_NET_MOVERESIZE_WINDOW;
extern Atom _XA_NET_WM_MOVERESIZE;
extern Atom _XA_NET_RESTACK_WINDOW;
extern Atom _XA_NET_REQUEST_FRAME_EXTENTS;

/* Application Window Properties */
extern Atom _XA_NET_WM_NAME;
extern Atom _XA_NET_WM_VISIBLE_NAME;
extern Atom _XA_NET_WM_ICON_NAME;
extern Atom _XA_NET_WM_VISIBLE_ICON_NAME;
extern Atom _XA_NET_WM_DESKTOP;
extern Atom _XA_NET_WM_WINDOW_TYPE;
extern Atom _XA_NET_WM_STATE;
extern Atom _XA_NET_WM_ALLOWED_ACTIONS;
extern Atom _XA_NET_WM_STRUT;
extern Atom _XA_NET_WM_STRUT_PARTIAL;
extern Atom _XA_NET_WM_ICON_GEOMETRY;
extern Atom _XA_NET_WM_ICON;
extern Atom _XA_NET_WM_PID;
extern Atom _XA_NET_WM_HANDLED_ICONS;
extern Atom _XA_NET_WM_USER_TIME;
extern Atom _XA_NET_WM_USER_TIME_WINDOW;
extern Atom _XA_NET_FRAME_EXTENTS;
extern Atom _XA_NET_WM_OPAQUE_REGION;
extern Atom _XA_NET_WM_BYPASS_COMPOSITOR;

/* _NET_WM_WINDOW_TYPE atoms */
extern Atom _XA_NET_WM_WINDOW_TYPE_DESKTOP;
extern Atom _XA_NET_WM_WINDOW_TYPE_DOCK;
extern Atom _XA_NET_WM_WINDOW_TYPE_TOOLBAR;
extern Atom _XA_NET_WM_WINDOW_TYPE_MENU;
extern Atom _XA_NET_WM_WINDOW_TYPE_UTILITY;
extern Atom _XA_NET_WM_WINDOW_TYPE_SPLASH;
extern Atom _XA_NET_WM_WINDOW_TYPE_DIALOG;
extern Atom _XA_NET_WM_WINDOW_TYPE_DROPDOWN_MENU;
extern Atom _XA_NET_WM_WINDOW_TYPE_POPUP_MENU;
extern Atom _XA_NET_WM_WINDOW_TYPE_TOOLTIP;
extern Atom _XA_NET_WM_WINDOW_TYPE_NOTIFICATION;
extern Atom _XA_NET_WM_WINDOW_TYPE_COMBO;
extern Atom _XA_NET_WM_WINDOW_TYPE_DND;
extern Atom _XA_NET_WM_WINDOW_TYPE_NORMAL;

/* _NET_WM_STATE atoms */
extern Atom _XA_NET_WM_STATE_MODAL;
extern Atom _XA_NET_WM_STATE_STICKY;
extern Atom _XA_NET_WM_STATE_MAXIMIZED_VERT;
extern Atom _XA_NET_WM_STATE_MAXIMIZED_HORZ;
extern Atom _XA_NET_WM_STATE_SHADED;
extern Atom _XA_NET_WM_STATE_SKIP_TASKBAR;
extern Atom _XA_NET_WM_STATE_SKIP_PAGER;
extern Atom _XA_NET_WM_STATE_HIDDEN;
extern Atom _XA_NET_WM_STATE_FULLSCREEN;
extern Atom _XA_NET_WM_STATE_ABOVE;
extern Atom _XA_NET_WM_STATE_BELOW;
extern Atom _XA_NET_WM_STATE_DEMANDS_ATTENTION;
extern Atom _XA_NET_WM_STATE_FOCUSED;

extern Atom _XA_NET_WM_STATE_DECOR_BORDER;
extern Atom _XA_NET_WM_STATE_DECOR_HANDLE;
extern Atom _XA_NET_WM_STATE_DECOR_TITLE;
extern Atom _XA_NET_WM_STATE_DECOR;
extern Atom _XA_NET_WM_STATE_PARENTRELATIVE_BACKGROUND;
extern Atom _XA_NET_WM_STATE_STAYS_AT_BOTTOM;
extern Atom _XA_NET_WM_STATE_STAYS_ON_TOP;

extern Atom _XA_NET_WM_STATE_MAXIMUS_BOTTOM;
extern Atom _XA_NET_WM_STATE_MAXIMUS_LEFT;
extern Atom _XA_NET_WM_STATE_MAXIMUS_RIGHT;
extern Atom _XA_NET_WM_STATE_MAXIMUS_TOP;
extern Atom _XA_NET_WM_STATE_AUTORAISE;
extern Atom _XA_NET_WM_STATE_AUTOLOWER;

/* _NET_WM_ALLOWED_ACTIONS atoms */
extern Atom _XA_NET_WM_ACTION_MOVE;
extern Atom _XA_NET_WM_ACTION_RESIZE;
extern Atom _XA_NET_WM_ACTION_MINIMIZE;
extern Atom _XA_NET_WM_ACTION_SHADE;
extern Atom _XA_NET_WM_ACTION_STICK;
extern Atom _XA_NET_WM_ACTION_MAXIMIZE_HORZ;
extern Atom _XA_NET_WM_ACTION_MAXIMIZE_VERT;
extern Atom _XA_NET_WM_ACTION_FULLSCREEN;
extern Atom _XA_NET_WM_ACTION_CHANGE_DESKTOP;
extern Atom _XA_NET_WM_ACTION_CLOSE;
extern Atom _XA_NET_WM_ACTION_ABOVE;
extern Atom _XA_NET_WM_ACTION_BELOW;

/* Window Manager Protocols */
extern Atom _XA_NET_WM_PING;
extern Atom _XA_NET_WM_SYNC_REQUEST;
extern Atom _XA_NET_WM_SYNC_REQUEST_COUNTER;
extern Atom _XA_NET_WM_FULLSCREEN_MONITORS;

/* Other Properties */
extern Atom _XA_NET_WM_FULL_PLACEMENT;

/* Compositing Managers */
extern Atom _XA_NET_WM_WINDOW_OPACITY;

/* Startup Notification */
extern Atom _XA_NET_STARTUP_ID;
extern Atom _XA_NET_STARTUP_INFO_BEGIN;
extern Atom _XA_NET_STARTUP_INFO;

/* System Tray */
extern Atom _XA_NET_SYSTEM_TRAY_MESSAGE_DATA;
extern Atom _XA_NET_SYSTEM_TRAY_OPCODE;
extern Atom _XA_NET_SYSTEM_TRAY_ORIENTATION;
extern Atom _XA_NET_SYSTEM_TRAY_VISUAL;

/* Additional non-standard properties */
extern Atom _XA_NET_MAXIMIZED_RESTORE;
extern Atom _XA_NET_PROPERTIES;
extern Atom _XA_NET_RESTART;
extern Atom _XA_NET_SHUTDOWN;
extern Atom _XA_NET_VIRTUAL_POS;
extern Atom _XA_NET_WM_DESKTOP_MASK;

/* ICCCM Protocols */
extern Atom _XA_WM_PROTOCOLS;
extern Atom _XA_WM_TAKE_FOCUS;
extern Atom _XA_WM_SAVE_YOURSELF;
extern Atom _XA_WM_DELETE_WINDOW;

/* used in varioius NetWM/EWMH client messages */
enum _NET_SOURCE {
    _NET_SOURCE_UNSPECIFIED = 0,
    _NET_SOURCE_APPLICATION = 1,
    _NET_SOURCE_PAGER = 2
};

enum _NET_SYSTEM_TRAY_ORIENTATION {
    _NET_SYSTEM_TRAY_ORIENTATION_HORZ = 0,
    _NET_SYSTEM_TRAY_ORIENTATION_VERT = 1
};

enum _NET_WM_MOVERESIZE {
    _NET_WM_MOVERESIZE_first = 0,
    _NET_WM_MOVERESIZE_SIZE_TOPLEFT = 0,
    _NET_WM_MOVERESIZE_SIZE_TOP = 1,
    _NET_WM_MOVERESIZE_SIZE_TOPRIGHT = 2,
    _NET_WM_MOVERESIZE_SIZE_RIGHT = 3,
    _NET_WM_MOVERESIZE_SIZE_BOTTOMRIGHT = 4,
    _NET_WM_MOVERESIZE_SIZE_BOTTOM = 5,
    _NET_WM_MOVERESIZE_SIZE_BOTTOMLEFT = 6,
    _NET_WM_MOVERESIZE_SIZE_LEFT = 7,
    _NET_WM_MOVERESIZE_MOVE = 8,
    _NET_WM_MOVERESIZE_SIZE_KEYBOARD = 9,
    _NET_WM_MOVERESIZE_MOVE_KEYBOARD = 10,
    _NET_WM_MOVERESIZE_CANCEL = 11,
    _NET_WM_MOVERESIZE_last = 11
};

enum _NET_WM_STATE_ACTION {
    _NET_WM_STATE_REMOVE = 0,		/* remove/unset property */
    _NET_WM_STATE_ADD = 1,	/* add/set property */
    _NET_WM_STATE_TOGGLE = 2	/* toggle property */
};

/* used in the _NET_RESTACK_WINDOW client message */
enum _NET_RESTACK_DETAIL {
    _NET_RESTACK_DETAIL_ABOVE = Above,
    _NET_RESTACK_DETAIL_BELOW = Below,
    _NET_RESTACK_DETAIL_TOPIF = TopIf,
    _NET_RESTACK_DETAIL_BOTTOMIF = BottomIf,
    _NET_RESTACK_DETAIL_OPPOSITE = Opposite,
};

enum _NET_COMPOSITOR_HINT {
    _NET_COMPOSITOR_HINT_NOPREFERENCE = 0,
    _NET_COMPOSITOR_HINT_DISABLE = 1,
    _NET_COMPOSITOR_HINT_ALLOW = 2
};

#define NET_ATOM_ENTRY(atom) TWM_ATOM_ENTRY(atom)

/* bit definitions for _NET_WM_WINDOW_TYPE */
enum _NET_WM_WINDOW_TYPE {
    _NET_WM_WINDOW_TYPE_DESKTOP_BIT = 0,
    _NET_WM_WINDOW_TYPE_DOCK_BIT,
    _NET_WM_WINDOW_TYPE_TOOLBAR_BIT,
    _NET_WM_WINDOW_TYPE_MENU_BIT,
    _NET_WM_WINDOW_TYPE_UTILITY_BIT,
    _NET_WM_WINDOW_TYPE_SPLASH_BIT,
    _NET_WM_WINDOW_TYPE_DIALOG_BIT,
    _NET_WM_WINDOW_TYPE_DROPDOWN_MENU_BIT,
    _NET_WM_WINDOW_TYPE_POPUP_MENU_BIT,
    _NET_WM_WINDOW_TYPE_TOOLTIP_BIT,
    _NET_WM_WINDOW_TYPE_NOTIFICATION_BIT,
    _NET_WM_WINDOW_TYPE_COMBO_BIT,
    _NET_WM_WINDOW_TYPE_DND_BIT,
    _NET_WM_WINDOW_TYPE_NORMAL_BIT,
    _NET_WM_WINDOW_TYPE_last
};

#define _NET_WM_WINDOW_TYPE_DESKTOP	  (1L<<_NET_WM_WINDOW_TYPE_DESKTOP_BIT)
#define _NET_WM_WINDOW_TYPE_DOCK	  (1L<<_NET_WM_WINDOW_TYPE_DOCK_BIT)
#define _NET_WM_WINDOW_TYPE_TOOLBAR	  (1L<<_NET_WM_WINDOW_TYPE_TOOLBAR_BIT)
#define _NET_WM_WINDOW_TYPE_MENU	  (1L<<_NET_WM_WINDOW_TYPE_MENU_BIT)
#define _NET_WM_WINDOW_TYPE_UTILITY	  (1L<<_NET_WM_WINDOW_TYPE_UTILITY_BIT)
#define _NET_WM_WINDOW_TYPE_SPLASH	  (1L<<_NET_WM_WINDOW_TYPE_SPLASH_BIT)
#define _NET_WM_WINDOW_TYPE_DIALOG	  (1L<<_NET_WM_WINDOW_TYPE_DIALOG_BIT)
#define _NET_WM_WINDOW_TYPE_DROPDOWN_MENU (1L<<_NET_WM_WINDOW_TYPE_DROPDOWN_MENU_BIT)
#define _NET_WM_WINDOW_TYPE_POPUP_MENU	  (1L<<_NET_WM_WINDOW_TYPE_POPUP_MENU_BIT)
#define _NET_WM_WINDOW_TYPE_TOOLTIP	  (1L<<_NET_WM_WINDOW_TYPE_TOOLTIP_BIT)
#define _NET_WM_WINDOW_TYPE_NOTIFICATION  (1L<<_NET_WM_WINDOW_TYPE_NOTIFICATION_BIT)
#define _NET_WM_WINDOW_TYPE_COMBO	  (1L<<_NET_WM_WINDOW_TYPE_COMBO_BIT)
#define _NET_WM_WINDOW_TYPE_DND		  (1L<<_NET_WM_WINDOW_TYPE_DND_BIT)
#define _NET_WM_WINDOW_TYPE_NORMAL	  (1L<<_NET_WM_WINDOW_TYPE_NORMAL_BIT)

/* bit definitions to track _NET_WM_STATE */
enum _NET_WM_STATE {
    _NET_WM_STATE_MODAL_BIT = 0,
    _NET_WM_STATE_STICKY_BIT,
    _NET_WM_STATE_MAXIMIZED_HORZ_BIT,
    _NET_WM_STATE_MAXIMIZED_VERT_BIT,
    _NET_WM_STATE_SHADED_BIT,
    _NET_WM_STATE_SKIP_TASKBAR_BIT,
    _NET_WM_STATE_SKIP_PAGER_BIT,
    _NET_WM_STATE_HIDDEN_BIT,
    _NET_WM_STATE_FULLSCREEN_BIT,
    _NET_WM_STATE_ABOVE_BIT,
    _NET_WM_STATE_BELOW_BIT,
    _NET_WM_STATE_DEMANDS_ATTENTION_BIT,
    _NET_WM_STATE_FOCUSED_BIT,
    _NET_WM_STATE_DECOR_BORDER_BIT,
    _NET_WM_STATE_DECOR_HANDLE_BIT,
    _NET_WM_STATE_DECOR_TITLE_BIT,
    _NET_WM_STATE_DECOR_BIT,
    _NET_WM_STATE_PARENTRELATIVE_BACKGROUND_BIT,
    _NET_WM_STATE_STAYS_AT_BOTTOM_BIT,
    _NET_WM_STATE_STAYS_ON_TOP_BIT,
    _NET_WM_STATE_MAXIMUS_BOTTOM_BIT,
    _NET_WM_STATE_MAXIMUS_LEFT_BIT,
    _NET_WM_STATE_MAXIMUS_RIGHT_BIT,
    _NET_WM_STATE_MAXIMUS_TOP_BIT,
    _NET_WM_STATE_AUTORAISE_BIT,
    _NET_WM_STATE_AUTOLOWER_BIT,
    _NET_WM_STATE_last
};

#define _NET_WM_STATE_MODAL		  (1L<<_NET_WM_STATE_MODAL_BIT)
#define _NET_WM_STATE_STICKY		  (1L<<_NET_WM_STATE_STICKY_BIT)
#define _NET_WM_STATE_MAXIMIZED_HORZ	  (1L<<_NET_WM_STATE_MAXIMIZED_HORZ_BIT)
#define _NET_WM_STATE_MAXIMIZED_VERT	  (1L<<_NET_WM_STATE_MAXIMIZED_VERT_BIT)
#define _NET_WM_STATE_SHADED		  (1L<<_NET_WM_STATE_SHADED_BIT)
#define _NET_WM_STATE_SKIP_TASKBAR	  (1L<<_NET_WM_STATE_SKIP_TASKBAR_BIT)
#define _NET_WM_STATE_SKIP_PAGER	  (1L<<_NET_WM_STATE_SKIP_PAGER_BIT)
#define _NET_WM_STATE_HIDDEN		  (1L<<_NET_WM_STATE_HIDDEN_BIT)
#define _NET_WM_STATE_FULLSCREEN	  (1L<<_NET_WM_STATE_FULLSCREEN_BIT)
#define _NET_WM_STATE_ABOVE		  (1L<<_NET_WM_STATE_ABOVE_BIT)
#define _NET_WM_STATE_BELOW		  (1L<<_NET_WM_STATE_BELOW_BIT)
#define _NET_WM_STATE_DEMANDS_ATTENTION	  (1L<<_NET_WM_STATE_DEMANDS_ATTENTION_BIT)
#define _NET_WM_STATE_FOCUSED		  (1L<<_NET_WM_STATE_FOCUSED_BIT)
#define _NET_WM_STATE_DECOR_BORDER	  (1L<<_NET_WM_STATE_DECOR_BORDER_BIT)
#define _NET_WM_STATE_DECOR_HANDLE	  (1L<<_NET_WM_STATE_DECOR_HANDLE_BIT)
#define _NET_WM_STATE_DECOR_TITLE	  (1L<<_NET_WM_STATE_DECOR_TITLE_BIT)
#define _NET_WM_STATE_DECOR		  (1L<<_NET_WM_STATE_DECOR_BIT)
#define _NET_WM_STATE_PARENTRELATIVE_BACKGROUND   (1L<<_NET_WM_STATE_PARENTRELATIVE_BACKGROUND_BIT)
#define _NET_WM_STATE_STAYS_AT_BOTTOM	  (1L<<_NET_WM_STATE_STAYS_AT_BOTTOM_BIT)
#define _NET_WM_STATE_STAYS_ON_TOP	  (1L<<_NET_WM_STATE_STAYS_ON_TOP_BIT)
#define _NET_WM_STATE_MAXIMUS_BOTTOM	  (1L<<_NET_WM_STATE_MAXIMUS_BOTTOM_BIT)
#define _NET_WM_STATE_MAXIMUS_LEFT	  (1L<<_NET_WM_STATE_MAXIMUS_LEFT_BIT)
#define _NET_WM_STATE_MAXIMUS_RIGHT	  (1L<<_NET_WM_STATE_MAXIMUS_RIGHT_BIT)
#define _NET_WM_STATE_MAXIMUS_TOP	  (1L<<_NET_WM_STATE_MAXIMUS_TOP_BIT)
#define _NET_WM_STATE_AUTORAISE		  (1L<<_NET_WM_STATE_AUTORAISE_BIT)
#define _NET_WM_STATE_AUTOLOWER		  (1L<<_NET_WM_STATE_AUTOLOWER_BIT)

/* bit definitions to track _NET_WM_ALLOWED_ACTIONS */
enum _NET_WM_ACTION {
    _NET_WM_ACTION_MOVE_BIT = 0,
    _NET_WM_ACTION_RESIZE_BIT,
    _NET_WM_ACTION_MINIMIZE_BIT,
    _NET_WM_ACTION_SHADE_BIT,
    _NET_WM_ACTION_STICK_BIT,
    _NET_WM_ACTION_MAXIMIZE_HORZ_BIT,
    _NET_WM_ACTION_MAXIMIZE_VERT_BIT,
    _NET_WM_ACTION_FULLSCREEN_BIT,
    _NET_WM_ACTION_CHANGE_DESKTOP_BIT,
    _NET_WM_ACTION_CLOSE_BIT,
    _NET_WM_ACTION_ABOVE_BIT,
    _NET_WM_ACTION_BELOW_BIT,
    _NET_WM_ACTION_last
};

#define _NET_WM_ACTION_MOVE		  (1L<<_NET_WM_ACTION_MOVE_BIT)
#define _NET_WM_ACTION_RESIZE		  (1L<<_NET_WM_ACTION_RESIZE_BIT)
#define _NET_WM_ACTION_MINIMIZE		  (1L<<_NET_WM_ACTION_MINIMIZE_BIT)
#define _NET_WM_ACTION_SHADE		  (1L<<_NET_WM_ACTION_SHADE_BIT)
#define _NET_WM_ACTION_STICK		  (1L<<_NET_WM_ACTION_STICK_BIT)
#define _NET_WM_ACTION_MAXIMIZE_HORZ	  (1L<<_NET_WM_ACTION_MAXIMIZE_HORZ_BIT)
#define _NET_WM_ACTION_MAXIMIZE_VERT	  (1L<<_NET_WM_ACTION_MAXIMIZE_VERT_BIT)
#define _NET_WM_ACTION_FULLSCREEN	  (1L<<_NET_WM_ACTION_FULLSCREEN_BIT)
#define _NET_WM_ACTION_CHANGE_DESKTOP	  (1L<<_NET_WM_ACTION_CHANGE_DESKTOP_BIT)
#define _NET_WM_ACTION_CLOSE		  (1L<<_NET_WM_ACTION_CLOSE_BIT)
#define _NET_WM_ACTION_ABOVE		  (1L<<_NET_WM_ACTION_ABOVE_BIT)
#define _NET_WM_ACTION_BELOW		  (1L<<_NET_WM_ACTION_BELOW_BIT)

typedef struct NetCounter {
    unsigned long low, high;
} NetCounter;

typedef struct NetPosition {
    long x, y;
} NetPosition;

typedef struct NetSize {
    long width, height;
} NetSize;

typedef struct NetGeometry {
    long x, y, width, height;
} NetGeometry;

typedef struct NetMonitors {
    long top, bottom, left, right;
} NetMonitors;

typedef struct NetStrut {
    long left, right, top, bottom;
    long left_start_y, left_end_y, right_start_y, right_end_y;
    long top_start_x, top_end_x, bottom_start_x, bottom_end_x;
} NetStrut;

typedef struct NetExtents {
    long left, right, top, bottom;
} NetExtents;

typedef struct NetIcon {
    unsigned *data;
    unsigned length;
} NetIcon;

typedef struct NetRestore {
    long x, y, width, height, vx, vy;
} NetRestore;

enum _NET_WM_ORIENTATION {
    _NET_WM_ORIENTATION_HORZ = 0,
    _NET_WM_ORIENTATION_VERT = 1
};

enum _NET_WM_STARTING_CORNER {
    _NET_WM_STARTING_CORNER_TOPLEFT = 0,
    _NET_WM_STARTING_CORNER_TOPRIGHT = 1,
    _NET_WM_STARTING_CORNER_BOTTOMRIGHT = 2,
    _NET_WM_STARTING_CORNER_BOTTOMLEFT = 3
};

typedef struct NetLayout {
    long orientation, columns, rows, starting_corner;
} NetLayout;

typedef struct NetSelection {
    Atom atom;				/* atom of the MANAGER selection */
    Window owner;			/* owner other than us */
    Window window;			/* window when we own the selection */
} NetSelection;

enum _NET_NOTIFY_MSG {
    _NET_NOTIFY_MSG_NEW = 0,
    _NET_NOTIFY_MSG_CHANGE,
    _NET_NOTIFY_MSG_REMOVE
};

enum _NET_NOTIFY_STATE {
    _NET_NOTIFY_IDLE = 0,		/* startup id defined but no msg
					   received */
    _NET_NOTIFY_NEW,		/* new: msg received */
    _NET_NOTIFY_CHANGED,	/* change: msg received */
    _NET_NOTIFY_COMPLETE	/* remove: msg received */
};

enum _NET_STARTUP_FIELD {
    _NET_STARTUP_ID = 0,
    _NET_STARTUP_NAME,
    _NET_STARTUP_SCREEN,
    _NET_STARTUP_BIN,
    _NET_STARTUP_ICON,
    _NET_STARTUP_DESKTOP,
    _NET_STARTUP_TIMESTAMP,
    _NET_STARTUP_DESCRIPTION,
    _NET_STARTUP_WMCLASS,
    _NET_STARTUP_SILENT,
    _NET_STARTUP_APPLICATION_ID,
    _NET_STARTUP_LAUNCHER,
    _NET_STARTUP_LAUNCHEE,
    _NET_STARTUP_HOSTNAME,
    _NET_STARTUP_PID,
    _NET_STARTUP_COMMAND,
    _NET_STARTUP_FILE,
    _NET_STARTUP_URL
};

typedef struct EwmhSequence {
    struct EwmhSequence *next;		/* list */
    struct EwmhSequence **prev;		/* list */
    enum _NET_NOTIFY_STATE state;	/* current state */
    TwmWindow *twm;			/* associated TwmWindow */
    Bool changed;			/* have fields changed? */
    union {
	char *fields[18];
	struct {
	    char *id;			/* ID= field */
	    char *name;			/* NAME= field */
	    char *screen;		/* SCREEN= field */
	    char *bin;			/* BIN= field */
	    char *icon;			/* ICON= field */
	    char *desktop;		/* DESKTOP= field */
	    char *timestamp;		/* TIMESTAMP= field */
	    char *description;		/* DESCRIPTION= field */
	    char *wmclass;		/* WMCLASS= field */
	    char *silent;		/* SILENT= field */
	    char *application_id;	/* APPLICATION_ID= field */
	    char *launcher;		/* LAUNCHER= field */
	    char *launchee;		/* LAUNCHEE= field */
	    char *hostname;		/* HOSTNAME= field */
	    char *pid;			/* PID= field */
	    char *command;		/* COMMAND= field */
	    char *file;			/* FILE= field */
	    char *url;			/* URL= field */
	} field;
    };
    struct {
	unsigned screen;		/* numeric SCREEN= field */
	unsigned desktop;		/* numeric DESKTOP= field */
	unsigned timestamp;		/* numeric TIMESTAMP= field */
	unsigned silent;		/* numeric SILENT= field */
	unsigned pid;			/* numeric PID= field */
    } numb;
} EwmhSequence;

typedef struct NetWindowBits {
    unsigned WM_CLASS:1;
    unsigned WM_PROTOCOLS:1;
    unsigned WM_CLIENT_MACHINE:1;
    unsigned WM_COMMAND:1;
    unsigned _NET_WM_NAME:1;
    unsigned _NET_WM_VISIBLE_NAME:1;
    unsigned _NET_WM_ICON_NAME:1;
    unsigned _NET_WM_VISIBLE_ICON_NAME:1;
    unsigned _NET_WM_DESKTOP:1;
    unsigned _NET_WM_WINDOW_TYPE:1;
    unsigned _NET_WM_STATE:1;
    unsigned _NET_WM_ALLOWED_ACTIONS:1;
    unsigned _NET_WM_STRUT:1;
    unsigned _NET_WM_STRUT_PARTIAL:1;
    unsigned _NET_WM_ICON_GEOMETRY:1;
    unsigned _NET_WM_ICON:1;
    unsigned _NET_WM_PID:1;
    unsigned _NET_WM_HANDLED_ICONS:1;
    unsigned _NET_WM_USER_TIME:1;
    unsigned _NET_WM_USER_TIME_WINDOW:1;
    unsigned _NET_FRAME_EXTENTS:1;
    unsigned _KDE_NET_WM_FRAME_STRUT:1;
    unsigned _NET_WM_OPAQUE_REGION:1;
    unsigned _NET_WM_BYPASS_COMPOSITOR:1;
    unsigned _NET_WM_SYNC_REQUEST_COUNTER:1;
    unsigned _NET_WM_FULLSCREEN_MONITORS:1;
    unsigned _NET_STARTUP_ID:1;
    unsigned _NET_MAXIMIZED_RESTORE:1;
    unsigned _NET_PROPERTIES:1;
    unsigned _NET_WM_DESKTOP_MASK:1;
    unsigned _NET_VIRTUAL_POS:1;
    unsigned _KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR:1;
    unsigned _KDE_NET_WM_WINDOW_TYPE_OVERRIDE:1;
} NetWindowBits;

typedef struct EwmhWindow {
    struct NetWindowBits props;		/* which properties are present */
    XClassHint class;			/* WM_CLASS */
    unsigned protocols;			/* WM_PROTOCOLS */
    char *machine;			/* WM_CLIENT_MACHINE */
    char **command;			/* WM_COMMAND */
    char *name;				/* _NET_WM_NAME */
    char *visible_name;			/* _NET_WM_VISIBLE_NAME */
    char *icon_name;			/* _NET_WM_ICON_NAME */
    char *visible_icon_name;		/* _NET_WM_VISIBLE_ICON_NAME */
    int desktop;			/* _NET_WM_DESKTOP */
    unsigned type;			/* _NET_WM_WINDOW_TYPE */
    unsigned state;			/* _NET_WM_STATE */
    unsigned allowed;			/* _NET_WM_ALLOWED_ACTIONS */
    struct NetStrut strut;		/* _NET_WM_STRUT(_PARTIAL) */
    struct NetGeometry icon_geometry;	/* _NET_WM_ICON_GEOMETRY */
    struct NetIcon icon;		/* _NET_WM_ICON */
    pid_t pid;				/* _NET_WM_PID */
    Time user_time;			/* _NET_WM_USER_TIME */
    Window user_time_window;		/* _NET_WM_USER_TIME_WINDOW */
    struct NetExtents extents;		/* _NET_FRAME_EXTENTS */
    /* _KDE_NET_WM_FRAME_STRUT */
    struct NetCounter sync_counter;	/* _NET_WM_SYNC_REQUEST_COUNTER */
    struct NetMonitors monitors;	/* _NET_WM_FULLSCREEN_MONITORS */
    char *startup_id;			/* _NET_STARTUP_ID */
    EwmhSequence *sequence;		/* _NET_STARTUP_ID */
    struct NetRestore restore;		/* _NET_MAXIMIZED_RESTORE */
    unsigned properties;		/* _NET_PROPERTIES */
    long *mask;				/* _NET_WM_DESKTOP_MASK */
    int masks;				/* _NET_WM_DESKTOP_MASK */
    struct NetPosition virtual_pos;	/* _NET_VIRTUAL_POS */
} EwmhWindow;

typedef struct EwmhNotify {
    struct EwmhNotify *next;		/* linked list */
    struct EwmhNotify **prev;		/* linked list */
    Window source;			/* source of messages */
    char *message;			/* message */
} EwmhNotify;

typedef struct NetScreenBits {
    unsigned _NET_SUPPORTED:1;
    unsigned _NET_CLIENT_LIST:1;
    unsigned _NET_CLIENT_LIST_STACKING:1;
    unsigned _NET_NUMBER_OF_DESKTOPS:1;
    unsigned _NET_DESKTOP_GEOMETRY:1;
    unsigned _NET_DESKTOP_VIEWPORT:1;
    unsigned _NET_CURRENT_DESKTOP:1;
    unsigned _NET_DESKTOP_NAMES:1;
    unsigned _NET_ACTIVE_WINDOW:1;
    unsigned _NET_WORKAREA:1;
    unsigned _NET_SUPPORTING_WM_CHECK:1;
    unsigned _NET_VIRTUAL_ROOTS:1;
    unsigned _NET_DESKTOP_LAYOUT:1;
    unsigned _NET_SHOWING_DESKTOP:1;
    unsigned _KDE_NET_SYSTEM_TRAY_WINDOWS:1;
} NetScreenBits;

typedef struct EwmhScreen {
    struct NetScreenBits props;		/* which properties are present */
    Window *clients;			/* _NET_CLIENT_LIST */
    Window *stacking;			/* _NET_CLIENT_LIST_STACKING */
    int desktops;			/* _NET_NUMBER_OF_DESKTOPS */
    struct NetSize geometry;		/* _NET_DESKTOP_GEOMETRY */
    struct NetPosition *viewport;	/* _NET_DESKTOP_VIEWPORT */
    unsigned viewports;			/* _NET_DESKTOP_VIEWPORT */
    int current;			/* _NET_CURRENT_DESKTOP */
    char **names;			/* _NET_DESKTOP_NAMES */
    Window active;			/* _NET_ACTIVE_WINDOW */
    struct NetGeometry *workarea;	/* _NET_WORKAREA */
    unsigned workareas;			/* _NET_WORKAREA */
    Window check;			/* _NET_SUPPORTING_WM_CHECK */
    Window *vroots;			/* _NET_VIRTUAL_ROOTS */
    struct NetLayout layout;		/* _NET_DESKTOP_LAYOUT */
    struct NetSelection layout_sn;	/* _NET_DESKTOP_LAYOUT_Sn */
    Bool handled_icons;			/* _NET_WM_HANDLED_ICONS */
    Bool showing;			/* _NET_SHOWING_DESKTOP */
    Window *systray;			/* _KDE_NET_SYSTEM_TRAY_WINDOWS */
    EwmhNotify *notify;			/* startup notification message list */
    EwmhSequence *sequence;		/* startup notification sequence list */
    short IconifyByUnmapping;		/* save original setting */
} EwmhScreen;

typedef struct EwmhWorkspace {
} EwmhWorkspace;

char *make_command_from_argv(char **argv);

Bool Get_NET_CURRENT_DESKTOP(Window root, int *desktop);

void Upd_NET_SUPPORTED(ScreenInfo *scr);
void Upd_NET_CLIENT_LIST(ScreenInfo *scr);
void Upd_NET_CLIENT_LIST_STACKING(ScreenInfo *scr);
void Upd_NET_NUMBER_OF_DESKTOPS(ScreenInfo *scr);
void Upd_NET_DESKTOP_GEOMETRY(ScreenInfo *scr);
void Upd_NET_DESKTOP_VIEWPORT(ScreenInfo *scr);
void Upd_NET_CURRENT_DESKTOP(ScreenInfo *scr);
void Upd_NET_DESKTOP_NAMES(ScreenInfo *scr);
void Upd_NET_ACTIVE_WINDOW(ScreenInfo *scr);
void Upd_NET_WORKAREA(ScreenInfo *scr);
void Upd_NET_SUPPORTING_WM_CHECK(ScreenInfo *scr);
void Upd_NET_VIRTUAL_ROOTS(ScreenInfo *scr);
void Upd_NET_DESKTOP_LAYOUT(ScreenInfo *scr);
void Upd_NET_SHOWING_DESKTOP(ScreenInfo *scr);
void Upd_WM_CLIENT_MACHINE(ScreenInfo *scr, TwmWindow *twin);
void Upd_NET_WM_VISIBLE_NAME(TwmWindow *twin);
void Upd_NET_WM_VISIBLE_ICON_NAME(TwmWindow *twin);
void Upd_NET_WM_DESKTOP(ScreenInfo *scr, TwmWindow *twin);
void Upd_NET_WM_WINDOW_TYPE(TwmWindow *twin);
void Upd_NET_WM_STATE(ScreenInfo *scr, TwmWindow *twin);
void Upd_NET_WM_ALLOWED_ACTIONS(TwmWindow *twin);
void Upd_NET_WM_PID(ScreenInfo *scr, TwmWindow *twin);
void Upd_NET_WM_USER_TIME(ScreenInfo *scr, TwmWindow *twin);
void Upd_NET_FRAME_EXTENTS(TwmWindow *twin);
void Upd_NET_WM_FULLSCREEN_MONITORS(ScreenInfo *scr, TwmWindow *twin);
void Upd_NET_STARTUP_ID(ScreenInfo *scr, TwmWindow *twin);
void Upd_NET_STARTUP_INFO(ScreenInfo *scr, TwmWindow *twin, Bool forced);
void Upd_KDE_NET_SYSTEM_TRAY_WINDOWS(ScreenInfo *scr);
void Upd_NET_MAXIMIZED_RESTORE(ScreenInfo *scr, TwmWindow *twin);
void Upd_NET_WM_DESKTOP_MASK(ScreenInfo *scr, TwmWindow *twin);
void Upd_NET_VIRTUAL_POS(ScreenInfo *scr, TwmWindow *twin);

EwmhSequence *Seq_NET_STARTUP_ID(ScreenInfo *scr, TwmWindow *twin);
void Chg_NET_STARTUP_INFO(ScreenInfo *scr, EwmhSequence *seq);

void InitEwmh(ScreenInfo *scr);
void UpdateEwmh(ScreenInfo *scr);
void TermEwmh(ScreenInfo *scr);

void AddWindowEwmh(ScreenInfo *scr, TwmWindow *twin);
void UpdWindowEwmh(ScreenInfo *scr, TwmWindow *twin);
void DelWindowEwmh(ScreenInfo *scr, TwmWindow *twin);

/* Event handlers */
Bool HandleNetClientMessage(ScreenInfo *scr, TwmWindow *twin, XEvent *xev);
Bool HandleNetPropertyNotify(ScreenInfo *scr, TwmWindow *twin, XEvent *xev);
Bool HandleNetSelectionClear(ScreenInfo *scr, TwmWindow *twin, XEvent *xev);
Bool HandleNetDestroyNotify(ScreenInfo *scr, TwmWindow *twin, XEvent *xev);

/* Hooks to the TWM variant. */

TwmWindow *TwmFindWindow(ScreenInfo *scr, Window window);

Window TwmNetRoot(ScreenInfo *scr);
Window TwmNetManager(ScreenInfo *scr);

void TwmGetClientList(ScreenInfo *scr, Window **windows, int *count);
void TwmGetClientListStacking(ScreenInfo *scr, Window **windows, int *count);
void TwmGetNumberOfDesktops(ScreenInfo *scr, int *number);
void TwmSetNumberOfDesktops(ScreenInfo *scr, int number);
void TwmGetDesktopGeometry(ScreenInfo *scr, struct NetSize *size);
void TwmSetDesktopGeometry(ScreenInfo *scr, struct NetSize *size);
void TwmGetDesktopViewport(ScreenInfo *scr, int desktop, struct NetPosition *viewport);
void TwmGetDesktopViewports(ScreenInfo *scr, struct NetPosition **viewport,
			    int *viewports);
void TwmSetDesktopViewport(ScreenInfo *scr, int desktop, struct NetPosition *viewport);
void TwmGetCurrentDesktop(ScreenInfo *scr, int *current);
void TwmSetCurrentDesktop(ScreenInfo *scr, int current, Time timestamp);
void TwmGetDesktopNames(ScreenInfo *scr, char ***names, int *count);
void TwmSetDesktopNames(ScreenInfo *scr, char **names, int count);
void TwmGetActiveWindow(ScreenInfo *scr, Window *window);
void TwmSetActiveWindow(ScreenInfo *scr, Window window, Window active, Time timestamp,
			enum _NET_SOURCE source);
void TwmGetWorkarea(ScreenInfo *scr, int desktop, struct NetGeometry *workarea);
void TwmGetWorkareas(ScreenInfo *scr, struct NetGeometry **workarea, int *workareas);
void TwmGetVirtualRoots(ScreenInfo *scr, Window **roots, int *count);
void TwmGetDesktopLayout(ScreenInfo *scr, struct NetLayout *layout);
void TwmSetDesktopLayout(ScreenInfo *scr, struct NetLayout *layout);
void TwmGetShowingDesktop(ScreenInfo *scr, Bool *showing);
void TwmSetShowingDesktop(ScreenInfo *scr, Bool showing);
void TwmCloseWindow(TwmWindow *twin, Time timestamp, enum _NET_SOURCE source);
void TwmMoveResizeWindow(TwmWindow *twin, unsigned gravity, unsigned mask,
			 XWindowChanges *changes, enum _NET_SOURCE source);
void TwmStartMoveResize(TwmWindow *twin, int x_root, int y_root,
			enum _NET_WM_MOVERESIZE direction, int button,
			enum _NET_SOURCE source);
void TwmRestackWindow(TwmWindow *twin, unsigned mask, XWindowChanges *changes,
		      enum _NET_SOURCE source);
void TwmGetWMClientMachine(ScreenInfo *scr, TwmWindow *twin, char **machine);
void TwmEstFrameExtents(Window window, struct NetExtents *extents);
void TwmSetWMName(ScreenInfo *scr, TwmWindow *twin, char *name);
void TwmGetWMVisibleName(TwmWindow *twin, char **name);
void TwmSetWMIconName(TwmWindow *twin, char *name);
void TwmGetWMVisibleIconName(TwmWindow *twin, char **name);
void TwmGetWMDesktop(ScreenInfo *scr, TwmWindow *twin, int *desktop);
void TwmIniWMDesktop(ScreenInfo *scr, TwmWindow *twin, int *desktop);
void TwmSetWMDesktop(ScreenInfo *scr, TwmWindow *twin, int desktop,
		     enum _NET_SOURCE source);
void TwmGetWMWindowType(TwmWindow *twin, unsigned *type);
void TwmIniWMWindowType(TwmWindow *twin, unsigned type);
void TwmSetWMWindowType(ScreenInfo *scr, TwmWindow *twin, unsigned type);
void TwmGetWMState(ScreenInfo *scr, TwmWindow *twin, unsigned *flags);
void TwmChgWMState(ScreenInfo *scr, TwmWindow *twin, int action1, int action2,
		   unsigned action, unsigned source);
void TwmGetWMAllowedActions(TwmWindow *twin, unsigned *flags);
void TwmGetWMPid(ScreenInfo *scr, TwmWindow *twin, pid_t *pid);
void TwmUpdWMHandledIcons(ScreenInfo *scr);
void TwmGetWMUserTimeWindow(ScreenInfo *scr, TwmWindow *twin, Window *time_window);
void TwmSetWMUserTimeWindow(ScreenInfo *scr, TwmWindow *twin, Window time_window);
void TwmDelWMUserTimeWindow(ScreenInfo *scr, TwmWindow *twin, Window time_window);
void TwmGetWMUserTime(ScreenInfo *scr, TwmWindow *twin, Time *time);
void TwmSetUserTime(Time time);
void TwmGetUserTime(Time *time);
void TwmGetWMFrameExtents(TwmWindow *twin, struct NetExtents *extents);
void TwmGetFullscreenMonitors(ScreenInfo *scr, int *monitors);
void TwmGetWMFullscreenMonitors(ScreenInfo *scr, TwmWindow *twin,
				struct NetMonitors *monitors);
void TwmIniWMFullscreenMonitors(ScreenInfo *scr, TwmWindow *twin,
				struct NetMonitors *monitors);
void TwmSetWMFullscreenMonitors(ScreenInfo *scr, TwmWindow *twin,
				struct NetMonitors *monitors);
void TwmGetGroupLeader(TwmWindow *twin, Window *leader);
void TwmSetStartupId(ScreenInfo *scr, TwmWindow *twin, char *startup_id);
void TwmGetWMDesktopMask(ScreenInfo *scr, TwmWindow *twin, long **mask, int *masks);
void TwmSetWMDesktopMask(ScreenInfo *scr, TwmWindow *twin, long *mask, int masks);
void TwmChgWMDesktopMask(ScreenInfo *scr, TwmWindow *twin, unsigned index, unsigned mask);
void TwmGetWMVirtualPos(ScreenInfo *scr, TwmWindow *twin, struct NetPosition *virtual_pos);
void TwmSetWMVirtualPos(ScreenInfo *scr, TwmWindow *twin, struct NetPosition *virtual_pos);
void TwmSetWMSyncRequestCounter(TwmWindow *twin, struct NetCounter *counter);
void TwmGotWMPing(TwmWindow *twin, Time timestamp, Window client);
void TwmGetKdeSystemTrayWindows(ScreenInfo *scr, Window **systray, int *count);
void TwmGetMaximizedRestore(ScreenInfo *scr, TwmWindow *twin, struct NetRestore *restore);
void TwmSetMaximizedRestore(ScreenInfo *scr, TwmWindow *twin, struct NetRestore *restore);
void TwmSetWMSystemTrayWindowFor(TwmWindow *twin);
void TwmIniWMWindowTypeOverride(TwmWindow *twin);
void TwmSetWMWindowTypeOverride(TwmWindow *twin);

#endif				/* _EWMH_H_ */

// vim: sw=4 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
