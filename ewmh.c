#include <stdio.h>
#include "ewmh.h"
#include "screen.h"
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/Xmu/CharSet.h>

/** @file
  *
  * This file contains a generic implementation of NetWM/EWMH specification
  * Version 1.5 Draft 1.
  *
  * Function naming conventions are as follows:
  *
  * - Set_*	sets the root or window property
  * - Get_*	gets the root or window property
  * - Upd_*	updates the root or window property by setting it to its current
  *		value according to the window manager
  * - Ret_*	retrieves the root or window property and synchronizes the
  *		setting with the window manager
  * - Rcv_*	processes a client message
  * - Snd_*	sends a client message
  */

Atom _XA_UTF8_STRING;

Atom _XA_KDE_NET_SYSTEM_TRAY_WINDOWS;
Atom _XA_KDE_NET_WM_FRAME_STRUT;
Atom _XA_KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR;
Atom _XA_KDE_NET_WM_WINDOW_TYPE_OVERRIDE;

/* Root Window Properties (and Related messages) */
Atom _XA_NET_SUPPORTED;
Atom _XA_NET_CLIENT_LIST;
Atom _XA_NET_CLIENT_LIST_STACKING;
Atom _XA_NET_NUMBER_OF_DESKTOPS;
Atom _XA_NET_DESKTOP_GEOMETRY;
Atom _XA_NET_DESKTOP_VIEWPORT;
Atom _XA_NET_CURRENT_DESKTOP;
Atom _XA_NET_DESKTOP_NAMES;
Atom _XA_NET_ACTIVE_WINDOW;
Atom _XA_NET_WORKAREA;
Atom _XA_NET_SUPPORTING_WM_CHECK;
Atom _XA_NET_VIRTUAL_ROOTS;
Atom _XA_NET_DESKTOP_LAYOUT;
Atom _XA_NET_SHOWING_DESKTOP;

/* Other Root Window Messages */
Atom _XA_NET_CLOSE_WINDOW;
Atom _XA_NET_MOVERESIZE_WINDOW;
Atom _XA_NET_WM_MOVERESIZE;
Atom _XA_NET_RESTACK_WINDOW;
Atom _XA_NET_REQUEST_FRAME_EXTENTS;

/* Application Window Properties */
Atom _XA_NET_WM_NAME;
Atom _XA_NET_WM_VISIBLE_NAME;
Atom _XA_NET_WM_ICON_NAME;
Atom _XA_NET_WM_VISIBLE_ICON_NAME;
Atom _XA_NET_WM_DESKTOP;
Atom _XA_NET_WM_WINDOW_TYPE;
Atom _XA_NET_WM_STATE;
Atom _XA_NET_WM_ALLOWED_ACTIONS;
Atom _XA_NET_WM_STRUT;
Atom _XA_NET_WM_STRUT_PARTIAL;
Atom _XA_NET_WM_ICON_GEOMETRY;
Atom _XA_NET_WM_ICON;
Atom _XA_NET_WM_PID;
Atom _XA_NET_WM_HANDLED_ICONS;
Atom _XA_NET_WM_USER_TIME;
Atom _XA_NET_WM_USER_TIME_WINDOW;
Atom _XA_NET_FRAME_EXTENTS;
Atom _XA_NET_WM_OPAQUE_REGION;
Atom _XA_NET_WM_BYPASS_COMPOSITOR;

/* _NET_WM_WINDOW_TYPE atoms */
Atom _XA_NET_WM_WINDOW_TYPE_DESKTOP;
Atom _XA_NET_WM_WINDOW_TYPE_DOCK;
Atom _XA_NET_WM_WINDOW_TYPE_TOOLBAR;
Atom _XA_NET_WM_WINDOW_TYPE_MENU;
Atom _XA_NET_WM_WINDOW_TYPE_UTILITY;
Atom _XA_NET_WM_WINDOW_TYPE_SPLASH;
Atom _XA_NET_WM_WINDOW_TYPE_DIALOG;
Atom _XA_NET_WM_WINDOW_TYPE_DROPDOWN_MENU;
Atom _XA_NET_WM_WINDOW_TYPE_POPUP_MENU;
Atom _XA_NET_WM_WINDOW_TYPE_TOOLTIP;
Atom _XA_NET_WM_WINDOW_TYPE_NOTIFICATION;
Atom _XA_NET_WM_WINDOW_TYPE_COMBO;
Atom _XA_NET_WM_WINDOW_TYPE_DND;
Atom _XA_NET_WM_WINDOW_TYPE_NORMAL;

/* _NET_WM_STATE atoms */
Atom _XA_NET_WM_STATE_MODAL;
Atom _XA_NET_WM_STATE_STICKY;
Atom _XA_NET_WM_STATE_MAXIMIZED_VERT;
Atom _XA_NET_WM_STATE_MAXIMIZED_HORZ;
Atom _XA_NET_WM_STATE_SHADED;
Atom _XA_NET_WM_STATE_SKIP_TASKBAR;
Atom _XA_NET_WM_STATE_SKIP_PAGER;
Atom _XA_NET_WM_STATE_HIDDEN;
Atom _XA_NET_WM_STATE_FULLSCREEN;
Atom _XA_NET_WM_STATE_ABOVE;
Atom _XA_NET_WM_STATE_BELOW;
Atom _XA_NET_WM_STATE_DEMANDS_ATTENTION;
Atom _XA_NET_WM_STATE_FOCUSED;

Atom _XA_NET_WM_STATE_DECOR_BORDER;
Atom _XA_NET_WM_STATE_DECOR_HANDLE;
Atom _XA_NET_WM_STATE_DECOR_TITLE;
Atom _XA_NET_WM_STATE_DECOR;
Atom _XA_NET_WM_STATE_PARENTRELATIVE_BACKGROUND;
Atom _XA_NET_WM_STATE_STAYS_AT_BOTTOM;
Atom _XA_NET_WM_STATE_STAYS_ON_TOP;

Atom _XA_NET_WM_STATE_MAXIMUS_BOTTOM;
Atom _XA_NET_WM_STATE_MAXIMUS_LEFT;
Atom _XA_NET_WM_STATE_MAXIMUS_RIGHT;
Atom _XA_NET_WM_STATE_MAXIMUS_TOP;
Atom _XA_NET_WM_STATE_AUTORAISE;
Atom _XA_NET_WM_STATE_AUTOLOWER;

/* _NET_WM_ALLOWED_ACTIONS atoms */
Atom _XA_NET_WM_ACTION_MOVE;
Atom _XA_NET_WM_ACTION_RESIZE;
Atom _XA_NET_WM_ACTION_MINIMIZE;
Atom _XA_NET_WM_ACTION_SHADE;
Atom _XA_NET_WM_ACTION_STICK;
Atom _XA_NET_WM_ACTION_MAXIMIZE_HORZ;
Atom _XA_NET_WM_ACTION_MAXIMIZE_VERT;
Atom _XA_NET_WM_ACTION_FULLSCREEN;
Atom _XA_NET_WM_ACTION_CHANGE_DESKTOP;
Atom _XA_NET_WM_ACTION_CLOSE;
Atom _XA_NET_WM_ACTION_ABOVE;
Atom _XA_NET_WM_ACTION_BELOW;

/* Window Manager Protocols */
Atom _XA_NET_WM_PING;
Atom _XA_NET_WM_SYNC_REQUEST;
Atom _XA_NET_WM_SYNC_REQUEST_COUNTER;
Atom _XA_NET_WM_FULLSCREEN_MONITORS;

/* Other Properties */
Atom _XA_NET_WM_FULL_PLACEMENT;

/* Compositing Managers */
Atom _XA_NET_WM_WINDOW_OPACITY;

/* Startup Notification */
Atom _XA_NET_STARTUP_ID;
Atom _XA_NET_STARTUP_INFO_BEGIN;
Atom _XA_NET_STARTUP_INFO;

/* System Tray */
Atom _XA_NET_SYSTEM_TRAY_MESSAGE_DATA;
Atom _XA_NET_SYSTEM_TRAY_OPCODE;
Atom _XA_NET_SYSTEM_TRAY_ORIENTATION;
Atom _XA_NET_SYSTEM_TRAY_VISUAL;

/* Additional non-standard properties */
Atom _XA_NET_MAXIMIZED_RESTORE;		/* awesome */
Atom _XA_NET_PROPERTIES;		/* ??? */
Atom _XA_NET_RESTART;			/* waimea */
Atom _XA_NET_SHUTDOWN;			/* waimea */
Atom _XA_NET_VIRTUAL_POS;		/* waimea */
Atom _XA_NET_WM_DESKTOP_MASK;		/* waimea */

/* ICCCM Protocols */
Atom _XA_WM_PROTOCOLS;
Atom _XA_WM_TAKE_FOCUS;
Atom _XA_WM_SAVE_YOURSELF;
Atom _XA_WM_DELETE_WINDOW;

typedef struct NetAtom {
    Atom *atom_p;
    const char *name;
} NetAtom;

static const struct NetAtom KdeAtoms[] = {
    {&_XA_KDE_NET_SYSTEM_TRAY_WINDOWS, "_KDE_NET_SYSTEM_TRAY_WINDOWS"},
    {&_XA_KDE_NET_WM_FRAME_STRUT, "_KDE_NET_WM_FRAME_STRUT"},
    {&_XA_KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR, "_KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR"},
    {&_XA_KDE_NET_WM_WINDOW_TYPE_OVERRIDE, "_KDE_NET_WM_WINDOW_TYPE_OVERRIDE"},
    {NULL, NULL}
};

void
InternKdeAtoms(void)
{
    const struct NetAtom *p;

    for (p = KdeAtoms; p->atom_p != NULL; p++) {
	*(p->atom_p) = XInternAtom(dpy, p->name, False);
    }
}

static const struct NetAtom NetAtoms[] = {
    {&_XA_UTF8_STRING, "UTF8_STRING"},
    /* Root Window Properties (and Related messages) */
    {&_XA_NET_SUPPORTED, "_NET_SUPPORTED"},
    {&_XA_NET_CLIENT_LIST, "_NET_CLIENT_LIST"},
    {&_XA_NET_CLIENT_LIST_STACKING, "_NET_CLIENT_LIST_STACKING"},
    {&_XA_NET_NUMBER_OF_DESKTOPS, "_NET_NUMBER_OF_DESKTOPS"},
    {&_XA_NET_DESKTOP_GEOMETRY, "_NET_DESKTOP_GEOMETRY"},
    {&_XA_NET_DESKTOP_VIEWPORT, "_NET_DESKTOP_VIEWPORT"},
    {&_XA_NET_CURRENT_DESKTOP, "_NET_CURRENT_DESKTOP"},
    {&_XA_NET_DESKTOP_NAMES, "_NET_DESKTOP_NAMES"},
    {&_XA_NET_ACTIVE_WINDOW, "_NET_ACTIVE_WINDOW"},
    {&_XA_NET_WORKAREA, "_NET_WORKAREA"},
    {&_XA_NET_SUPPORTING_WM_CHECK, "_NET_SUPPORTING_WM_CHECK"},
    {&_XA_NET_VIRTUAL_ROOTS, "_NET_VIRTUAL_ROOTS"},
    {&_XA_NET_DESKTOP_LAYOUT, "_NET_DESKTOP_LAYOUT"},
    {&_XA_NET_SHOWING_DESKTOP, "_NET_SHOWING_DESKTOP"},
    /* Other Root Window Messages */
    {&_XA_NET_CLOSE_WINDOW, "_NET_CLOSE_WINDOW"},
    {&_XA_NET_MOVERESIZE_WINDOW, "_NET_MOVERESIZE_WINDOW"},
    {&_XA_NET_WM_MOVERESIZE, "_NET_WM_MOVERESIZE"},
    {&_XA_NET_RESTACK_WINDOW, "_NET_RESTACK_WINDOW"},
    {&_XA_NET_REQUEST_FRAME_EXTENTS, "_NET_REQUEST_FRAME_EXTENTS"},
    /* Application Window Properties */
    {&_XA_NET_WM_NAME, "_NET_WM_NAME"},
    {&_XA_NET_WM_VISIBLE_NAME, "_NET_WM_VISIBLE_NAME"},
    {&_XA_NET_WM_ICON_NAME, "_NET_WM_ICON_NAME"},
    {&_XA_NET_WM_VISIBLE_ICON_NAME, "_NET_WM_VISIBLE_ICON_NAME"},
    {&_XA_NET_WM_DESKTOP, "_NET_WM_DESKTOP"},
    {&_XA_NET_WM_WINDOW_TYPE, "_NET_WM_WINDOW_TYPE"},
    {&_XA_NET_WM_STATE, "_NET_WM_STATE"},
    {&_XA_NET_WM_ALLOWED_ACTIONS, "_NET_WM_ALLOWED_ACTIONS"},
    {&_XA_NET_WM_STRUT, "_NET_WM_STRUT"},
    {&_XA_NET_WM_STRUT_PARTIAL, "_NET_WM_STRUT_PARTIAL"},
    {&_XA_NET_WM_ICON_GEOMETRY, "_NET_WM_ICON_GEOMETRY"},
    {&_XA_NET_WM_ICON, "_NET_WM_ICON"},
    {&_XA_NET_WM_PID, "_NET_WM_PID"},
    {&_XA_NET_WM_HANDLED_ICONS, "_NET_WM_HANDLED_ICONS"},
    {&_XA_NET_WM_USER_TIME, "_NET_WM_USER_TIME"},
    {&_XA_NET_WM_USER_TIME_WINDOW, "_NET_WM_USER_TIME_WINDOW"},
    {&_XA_NET_FRAME_EXTENTS, "_NET_FRAME_EXTENTS"},
    {&_XA_NET_WM_OPAQUE_REGION, "_NET_WM_OPAQUE_REGION"},
    {&_XA_NET_WM_BYPASS_COMPOSITOR, "_NET_WM_BYPASS_COMPOSITOR"},
    /* _NET_WM_WINDOW_TYPE atoms */
    {&_XA_NET_WM_WINDOW_TYPE_DESKTOP, "_NET_WM_WINDOW_TYPE_DESKTOP"},
    {&_XA_NET_WM_WINDOW_TYPE_DOCK, "_NET_WM_WINDOW_TYPE_DOCK"},
    {&_XA_NET_WM_WINDOW_TYPE_TOOLBAR, "_NET_WM_WINDOW_TYPE_TOOLBAR"},
    {&_XA_NET_WM_WINDOW_TYPE_MENU, "_NET_WM_WINDOW_TYPE_MENU"},
    {&_XA_NET_WM_WINDOW_TYPE_UTILITY, "_NET_WM_WINDOW_TYPE_UTILITY"},
    {&_XA_NET_WM_WINDOW_TYPE_SPLASH, "_NET_WM_WINDOW_TYPE_SPLASH"},
    {&_XA_NET_WM_WINDOW_TYPE_DIALOG, "_NET_WM_WINDOW_TYPE_DIALOG"},
    {&_XA_NET_WM_WINDOW_TYPE_DROPDOWN_MENU, "_NET_WM_WINDOW_TYPE_DROPDOWN_MENU"},
    {&_XA_NET_WM_WINDOW_TYPE_POPUP_MENU, "_NET_WM_WINDOW_TYPE_POPUP_MENU"},
    {&_XA_NET_WM_WINDOW_TYPE_TOOLTIP, "_NET_WM_WINDOW_TYPE_TOOLTIP"},
    {&_XA_NET_WM_WINDOW_TYPE_NOTIFICATION, "_NET_WM_WINDOW_TYPE_NOTIFICATION"},
    {&_XA_NET_WM_WINDOW_TYPE_COMBO, "_NET_WM_WINDOW_TYPE_COMBO"},
    {&_XA_NET_WM_WINDOW_TYPE_DND, "_NET_WM_WINDOW_TYPE_DND"},
    {&_XA_NET_WM_WINDOW_TYPE_NORMAL, "_NET_WM_WINDOW_TYPE_NORMAL"},
    /* _NET_WM_STATE atoms */
    {&_XA_NET_WM_STATE_MODAL, "_NET_WM_STATE_MODAL"},
    {&_XA_NET_WM_STATE_STICKY, "_NET_WM_STATE_STICKY"},
    {&_XA_NET_WM_STATE_MAXIMIZED_VERT, "_NET_WM_STATE_MAXIMIZED_VERT"},
    {&_XA_NET_WM_STATE_MAXIMIZED_HORZ, "_NET_WM_STATE_MAXIMIZED_HORZ"},
    {&_XA_NET_WM_STATE_SHADED, "_NET_WM_STATE_SHADED"},
    {&_XA_NET_WM_STATE_SKIP_TASKBAR, "_NET_WM_STATE_SKIP_TASKBAR"},
    {&_XA_NET_WM_STATE_SKIP_PAGER, "_NET_WM_STATE_SKIP_PAGER"},
    {&_XA_NET_WM_STATE_HIDDEN, "_NET_WM_STATE_HIDDEN"},
    {&_XA_NET_WM_STATE_FULLSCREEN, "_NET_WM_STATE_FULLSCREEN"},
    {&_XA_NET_WM_STATE_ABOVE, "_NET_WM_STATE_ABOVE"},
    {&_XA_NET_WM_STATE_BELOW, "_NET_WM_STATE_BELOW"},
    {&_XA_NET_WM_STATE_DEMANDS_ATTENTION, "_NET_WM_STATE_DEMANDS_ATTENTION"},
    {&_XA_NET_WM_STATE_FOCUSED, "_NET_WM_STATE_FOCUSED"},
    {&_XA_NET_WM_STATE_DECOR_BORDER, "_NET_WM_STATE_DECOR_BORDER"},
    {&_XA_NET_WM_STATE_DECOR_HANDLE, "_NET_WM_STATE_DECOR_HANDLE"},
    {&_XA_NET_WM_STATE_DECOR_TITLE, "_NET_WM_STATE_DECOR_TITLE"},
    {&_XA_NET_WM_STATE_DECOR, "_NET_WM_STATE_DECOR"},
    {&_XA_NET_WM_STATE_PARENTRELATIVE_BACKGROUND,
     "_NET_WM_STATE_PARENTRELATIVE_BACKGROUND"},
    {&_XA_NET_WM_STATE_STAYS_AT_BOTTOM, "_NET_WM_STATE_STAYS_AT_BOTTOM"},
    {&_XA_NET_WM_STATE_STAYS_ON_TOP, "_NET_WM_STATE_STAYS_ON_TOP"},
    {&_XA_NET_WM_STATE_MAXIMUS_BOTTOM, "_NET_WM_STATE_MAXIMUS_BOTTOM"},
    {&_XA_NET_WM_STATE_MAXIMUS_LEFT, "_NET_WM_STATE_MAXIMUS_LEFT"},
    {&_XA_NET_WM_STATE_MAXIMUS_RIGHT, "_NET_WM_STATE_MAXIMUS_RIGHT"},
    {&_XA_NET_WM_STATE_MAXIMUS_TOP, "_NET_WM_STATE_MAXIMUS_TOP"},
    {&_XA_NET_WM_STATE_AUTORAISE, "_NET_WM_STATE_AUTORAISE"},
    {&_XA_NET_WM_STATE_AUTOLOWER, "_NET_WM_STATE_AUTOLOWER"},
    /* _NET_WM_ALLOWED_ACTIONS atoms */
    {&_XA_NET_WM_ACTION_MOVE, "_NET_WM_ACTION_MOVE"},
    {&_XA_NET_WM_ACTION_RESIZE, "_NET_WM_ACTION_RESIZE"},
    {&_XA_NET_WM_ACTION_MINIMIZE, "_NET_WM_ACTION_MINIMIZE"},
    {&_XA_NET_WM_ACTION_SHADE, "_NET_WM_ACTION_SHADE"},
    {&_XA_NET_WM_ACTION_STICK, "_NET_WM_ACTION_STICK"},
    {&_XA_NET_WM_ACTION_MAXIMIZE_HORZ, "_NET_WM_ACTION_MAXIMIZE_HORZ"},
    {&_XA_NET_WM_ACTION_MAXIMIZE_VERT, "_NET_WM_ACTION_MAXIMIZE_VERT"},
    {&_XA_NET_WM_ACTION_FULLSCREEN, "_NET_WM_ACTION_FULLSCREEN"},
    {&_XA_NET_WM_ACTION_CHANGE_DESKTOP, "_NET_WM_ACTION_CHANGE_DESKTOP"},
    {&_XA_NET_WM_ACTION_CLOSE, "_NET_WM_ACTION_CLOSE"},
    {&_XA_NET_WM_ACTION_ABOVE, "_NET_WM_ACTION_ABOVE"},
    {&_XA_NET_WM_ACTION_BELOW, "_NET_WM_ACTION_BELOW"},
    /* Window Manager Protocols */
    {&_XA_NET_WM_PING, "_NET_WM_PING"},
    {&_XA_NET_WM_SYNC_REQUEST, "_NET_WM_SYNC_REQUEST"},
    {&_XA_NET_WM_SYNC_REQUEST_COUNTER, "_NET_WM_SYNC_REQUEST_COUNTER"},
    {&_XA_NET_WM_FULLSCREEN_MONITORS, "_NET_WM_FULLSCREEN_MONITORS"},
    /* Other Properties */
    {&_XA_NET_WM_FULL_PLACEMENT, "_NET_WM_FULL_PLACEMENT"},
    /* Compositing Managers */
    {&_XA_NET_WM_WINDOW_OPACITY, "_NET_WM_WINDOW_OPACITY"},
    /* Startup Notification */
    {&_XA_NET_STARTUP_ID, "_NET_STARTUP_ID"},
    {&_XA_NET_STARTUP_INFO_BEGIN, "_NET_STARTUP_INFO_BEGIN"},
    {&_XA_NET_STARTUP_INFO, "_NET_STARTUP_INFO"},
    /* System Tray */
    {&_XA_NET_SYSTEM_TRAY_MESSAGE_DATA, "_NET_SYSTEM_TRAY_MESSAGE_DATA"},
    {&_XA_NET_SYSTEM_TRAY_OPCODE, "_NET_SYSTEM_TRAY_OPCODE"},
    {&_XA_NET_SYSTEM_TRAY_ORIENTATION, "_NET_SYSTEM_TRAY_ORIENTATION"},
    {&_XA_NET_SYSTEM_TRAY_VISUAL, "_NET_SYSTEM_TRAY_VISUAL"},

    {&_XA_NET_MAXIMIZED_RESTORE, "_NET_MAXIMIZED_RESTORE"},
    {&_XA_NET_PROPERTIES, "_NET_PROPERTIES"},
    {&_XA_NET_RESTART, "_NET_RESTART"},
    {&_XA_NET_SHUTDOWN, "_NET_SHUTDOWN"},
    {&_XA_NET_VIRTUAL_POS, "_NET_VIRTUAL_POS"},
    {&_XA_NET_WM_DESKTOP_MASK, "_NET_WM_DESKTOP_MASK"},
    {NULL, NULL}
};

void
InternNetAtoms(void)
{
    const struct NetAtom *p;

    for (p = NetAtoms; p->atom_p != NULL; p++) {
	*(p->atom_p) = XInternAtom(dpy, p->name, False);
    }
}

static void
bits_to_atoms(long *atoms, int *count, const Atom **defs, unsigned bits)
{
    int nitems = 0, n;
    const Atom **p;

    for (n = 0, p = defs; *p != NULL && n < 32; p++, n++)
	if (bits & (1 << n))
	    atoms[nitems++] = **p;
    *count = nitems;
}

static void
Set_NET_WM_atoms(Window window, Atom property, const Atom **defs, unsigned flags)
{
    long data[32] = { 0, };
    int count = 0;

    bits_to_atoms(data, &count, defs, flags);
    XChangeProperty(dpy, window, property, XA_ATOM, 32, PropModeReplace,
		    (unsigned char *) data, count);
}

static int
atom_to_index(Atom atom, const Atom **defs)
{
    const Atom **p;
    int n, result = -1;

    for (n = 0, p = defs; *p != NULL && n < 32; p++, n++) {
	if (atom == **p) {
	    result = n;
	    break;
	}
    }
    return result;
}

static unsigned
atoms_to_bits(long *atoms, unsigned long count, const Atom **defs)
{
    unsigned bits = 0;
    long a, *data;
    int n;

    for (a = 0, data = atoms; a < count; a++, data++) {
	if ((n = atom_to_index(*data, defs)) >= 0) {
	    bits |= (1 << n);
	    break;
	}
    }
    return bits;
}

static Bool
Get_NET_WM_atoms(Window window, Atom property, const Atom **defs, unsigned *flags)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, window, property, 0L, 32L, False, XA_ATOM, &actual_type,
			   &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    if (status != Success || actual_type == None) {
	if (prop != NULL)
	    XFree(prop);
	*flags = -1U;
	return False;
    }
    *flags = atoms_to_bits(prop, nitems, defs);
    XFree(prop);
    return True;
}

/** @brief Compare two strings considering null pointers.
  * @param s1 - string 1
  * @param s2 - string 2
  * @return int - zero when strings match
  */
static inline int
strcmp_null(char *s1, char *s2)
{
    if (s1 == NULL && s2 == NULL)
	return (0);
    if (s1 == NULL)
	return (-1);
    if (s2 == NULL)
	return (1);
    return strcmp(s1, s2);
}

/** @brief Compare two string lists.
  * @param l1 - string list 1
  * @param l2 - string list 2
  * @param int - zero when lists match
  */
static inline int
strcmp_list(char **l1, char **l2)
{
    int cmp;

    if (l1 == NULL && l2 == NULL)
	return (0);
    if (l1 == NULL)
	return (-1);
    if (l2 == NULL)
	return (1);
    for (;;) {
	if (*l1 == NULL && *l2 == NULL)
	    return (0);
	if (*l1 == NULL)
	    return (-1);
	if (*l2 == NULL)
	    return (1);
	if ((cmp = strcmp(*l1++, *l2++)) != 0)
	    return (cmp);
    }
}

static inline int
cmp_window_list(Window *wl1, Window *wl2)
{
    if (wl1 == NULL && wl2 == NULL)
	return (0);
    if (wl1 == NULL)
	return (-1);
    if (wl2 == NULL)
	return (1);
    for (; *wl1 != None || *wl2 != None; wl1++, wl2++) {
	if (*wl1 == None)
	    return (-1);
	if (*wl2 == None)
	    return (1);
	if (*wl1 < *wl2)
	    return (-1);
	if (*wl1 > *wl2)
	    return (1);
    }
    return (0);
}

static inline int
cmp_mask_list(long *l1, int n1, long *l2, int n2)
{
    int n;

    if (n1 == 0 && n2 == 0)
	return (0);
    if (n1 == 0)
	return (-1);
    if (n2 == 0)
	return (1);
    if (n1 < n2)
	return (-1);
    if (n1 > n2)
	return (1);
    for (n = 0; n < n1; n++) {
	if (l1[n] < l2[n])
	    return (-1);
	if (l1[n] > l2[n])
	    return (1);
    }
    return (0);
}

static inline int
cmp_size(struct NetSize *s1, struct NetSize *s2)
{
    if (s1->width < s2->width)
	return (-1);
    if (s1->width > s2->width)
	return (1);
    if (s1->height < s2->height)
	return (-1);
    if (s1->height > s2->height)
	return (1);
    return (0);
}

static inline int
cmp_pos(struct NetPosition *p1, struct NetPosition *p2)
{
    if (p1->x < p2->x)
	return (-1);
    if (p1->x > p2->x)
	return (1);
    if (p1->y < p2->y)
	return (-1);
    if (p1->y > p2->y)
	return (1);
    return (0);
}

static inline int
cmp_pos_list(struct NetPosition *p1, int n1, struct NetPosition *p2, int n2)
{
    int n, cmp;

    if (n1 == 0 && n2 == 0)
	return (0);
    if (n1 == 0)
	return (-1);
    if (n2 == 0)
	return (1);
    if (n1 < n2)
	return (-1);
    if (n1 > n2)
	return (1);
    for (n = 0; n < n1; n++)
	if ((cmp = cmp_pos(p1, p2)) != 0)
	    return (cmp);
    return (0);
}

static inline int
cmp_layout(struct NetLayout *l1, struct NetLayout *l2)
{
    if (l1->orientation < l2->orientation)
	return (-1);
    if (l1->orientation > l2->orientation)
	return (1);
    if (l1->columns < l2->columns)
	return (-1);
    if (l1->columns > l2->columns)
	return (1);
    if (l1->rows < l2->rows)
	return (-1);
    if (l1->rows > l2->rows)
	return (1);
    if (l1->starting_corner < l2->starting_corner)
	return (-1);
    if (l1->starting_corner > l2->starting_corner)
	return (1);
    return (0);
}

static inline int
cmp_extents(struct NetExtents *e1, struct NetExtents *e2)
{
    if (e1->left < e2->left)
	return (-1);
    if (e1->left > e2->left)
	return (1);
    if (e1->right < e2->right)
	return (-1);
    if (e1->right > e2->right)
	return (1);
    if (e1->top < e2->top)
	return (-1);
    if (e1->top > e2->top)
	return (1);
    if (e1->bottom < e2->bottom)
	return (-1);
    if (e1->bottom > e2->bottom)
	return (1);
    return (0);
}

static inline int
cmp_restore(struct NetRestore *r1, struct NetRestore *r2)
{
    if (r1->x < r2->x)
	return (-1);
    if (r1->x > r2->x)
	return (1);
    if (r1->y < r2->y)
	return (-1);
    if (r1->y > r2->y)
	return (1);
    if (r1->width < r2->width)
	return (-1);
    if (r1->width > r2->width)
	return (1);
    if (r1->height < r2->height)
	return (-1);
    if (r1->height > r2->height)
	return (1);
    if (r1->vx < r2->vx)
	return (-1);
    if (r1->vx > r2->vx)
	return (1);
    if (r1->vy < r2->vy)
	return (-1);
    if (r1->vy > r2->vy)
	return (1);
    return (0);
}

static inline int
cmp_mons(struct NetMonitors *m1, struct NetMonitors *m2)
{
    if (m1->top < m2->top)
	return (-1);
    if (m1->top > m2->top)
	return (1);
    if (m1->bottom < m2->bottom)
	return (-1);
    if (m1->bottom > m2->bottom)
	return (1);
    if (m1->left < m2->left)
	return (-1);
    if (m1->left > m2->left)
	return (1);
    if (m1->right < m2->right)
	return (-1);
    if (m1->right > m2->right)
	return (1);
    return (0);
}

static inline int
cmp_geo(struct NetGeometry *g1, struct NetGeometry *g2)
{
    if (g1->x < g2->x)
	return (-1);
    if (g1->x > g2->x)
	return (1);
    if (g1->y < g2->y)
	return (-1);
    if (g1->y > g2->y)
	return (1);
    if (g1->width < g2->width)
	return (-1);
    if (g1->width > g2->width)
	return (1);
    if (g1->height < g2->height)
	return (-1);
    if (g1->height > g2->height)
	return (1);
    return (0);
}

static inline int
cmp_geo_list(struct NetGeometry *g1, int n1, struct NetGeometry *g2, int n2)
{
    int n, cmp;

    if (n1 == 0 && n2 == 0)
	return (0);
    if (n1 == 0)
	return (-1);
    if (n2 == 0)
	return (1);
    if (n1 < n2)
	return (-1);
    if (n1 > n2)
	return (1);
    for (n = 0; n < n1; n++)
	if ((cmp = cmp_geo(g1, g2)) != 0)
	    return (cmp);
    return (0);
}

/** @name Root Window Properties (and Related messages)
  *
  * @{ */

/** @name _NET_SUPPORTED, ATOM[]/32
  *
  * This property MUST be set by the window manager to indicate which hints it
  * supports.  For example: considering _NET_WM_STATE both this atom and all
  * suported states e.g. _NET_WM_STATE_MODAL, _NET_WM_STATE_STICKY, would be
  * listed.  This assumes that backwards incompatible changes will not be made to
  * the hints (without being renamed).
  * @{ */

/** @brief Set the supported features array.
  */
static void
Set_NET_SUPPORTED(Window root)
{
    long data[] = {
	_XA_KDE_NET_SYSTEM_TRAY_WINDOWS,
	_XA_KDE_NET_WM_FRAME_STRUT,
	_XA_KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR,
	_XA_KDE_NET_WM_WINDOW_TYPE_OVERRIDE,
	_XA_KDE_SPLASH_PROGRESS,
	_XA_KDE_WM_CHANGE_STATE,

	/* Root Window Properties (and Related messages) */
	_XA_NET_SUPPORTED,
	_XA_NET_CLIENT_LIST,
	_XA_NET_CLIENT_LIST_STACKING,
	_XA_NET_NUMBER_OF_DESKTOPS,
	_XA_NET_DESKTOP_GEOMETRY,
	_XA_NET_DESKTOP_VIEWPORT,
	_XA_NET_CURRENT_DESKTOP,
	_XA_NET_DESKTOP_NAMES,
	_XA_NET_ACTIVE_WINDOW,
	_XA_NET_WORKAREA,
	_XA_NET_SUPPORTING_WM_CHECK,
	_XA_NET_VIRTUAL_ROOTS,
	_XA_NET_DESKTOP_LAYOUT,
	_XA_NET_SHOWING_DESKTOP,

	/* Other Root Window Messages */
	_XA_NET_CLOSE_WINDOW,
	_XA_NET_MOVERESIZE_WINDOW,
	_XA_NET_WM_MOVERESIZE,
	_XA_NET_RESTACK_WINDOW,
	_XA_NET_REQUEST_FRAME_EXTENTS,

	/* Application Window Properties */
	_XA_NET_WM_NAME,
	_XA_NET_WM_VISIBLE_NAME,
	_XA_NET_WM_ICON_NAME,
	_XA_NET_WM_VISIBLE_ICON_NAME,
	_XA_NET_WM_DESKTOP,
	_XA_NET_WM_WINDOW_TYPE,
	_XA_NET_WM_STATE,
	_XA_NET_WM_ALLOWED_ACTIONS,
	_XA_NET_WM_STRUT,
	_XA_NET_WM_STRUT_PARTIAL,
	_XA_NET_WM_ICON_GEOMETRY,
	_XA_NET_WM_ICON,
	_XA_NET_WM_PID,
	_XA_NET_WM_HANDLED_ICONS,
	_XA_NET_WM_USER_TIME,
	_XA_NET_WM_USER_TIME_WINDOW,
	_XA_NET_FRAME_EXTENTS,
	_XA_NET_WM_OPAQUE_REGION,
	_XA_NET_WM_BYPASS_COMPOSITOR,

	/* _NET_WM_WINDOW_TYPE atoms */
	_XA_NET_WM_WINDOW_TYPE_DESKTOP,
	_XA_NET_WM_WINDOW_TYPE_DOCK,
	_XA_NET_WM_WINDOW_TYPE_TOOLBAR,
	_XA_NET_WM_WINDOW_TYPE_MENU,
	_XA_NET_WM_WINDOW_TYPE_UTILITY,
	_XA_NET_WM_WINDOW_TYPE_SPLASH,
	_XA_NET_WM_WINDOW_TYPE_DIALOG,
	_XA_NET_WM_WINDOW_TYPE_DROPDOWN_MENU,
	_XA_NET_WM_WINDOW_TYPE_POPUP_MENU,
	_XA_NET_WM_WINDOW_TYPE_TOOLTIP,
	_XA_NET_WM_WINDOW_TYPE_NOTIFICATION,
	_XA_NET_WM_WINDOW_TYPE_COMBO,
	_XA_NET_WM_WINDOW_TYPE_DND,
	_XA_NET_WM_WINDOW_TYPE_NORMAL,

	/* _NET_WM_STATE atoms */
	_XA_NET_WM_STATE_MODAL,
	_XA_NET_WM_STATE_STICKY,
	_XA_NET_WM_STATE_MAXIMIZED_VERT,
	_XA_NET_WM_STATE_MAXIMIZED_HORZ,
	_XA_NET_WM_STATE_SHADED,
	_XA_NET_WM_STATE_SKIP_TASKBAR,
	_XA_NET_WM_STATE_SKIP_PAGER,
	_XA_NET_WM_STATE_HIDDEN,
	_XA_NET_WM_STATE_FULLSCREEN,
	_XA_NET_WM_STATE_ABOVE,
	_XA_NET_WM_STATE_BELOW,
	_XA_NET_WM_STATE_DEMANDS_ATTENTION,
	_XA_NET_WM_STATE_FOCUSED,
	_XA_NET_WM_STATE_DECOR_BORDER,
	_XA_NET_WM_STATE_DECOR_HANDLE,
	_XA_NET_WM_STATE_DECOR_TITLE,
	_XA_NET_WM_STATE_DECOR,
	_XA_NET_WM_STATE_PARENTRELATIVE_BACKGROUND,
	_XA_NET_WM_STATE_STAYS_AT_BOTTOM,
	_XA_NET_WM_STATE_STAYS_ON_TOP,
	_XA_NET_WM_STATE_MAXIMUS_BOTTOM,
	_XA_NET_WM_STATE_MAXIMUS_LEFT,
	_XA_NET_WM_STATE_MAXIMUS_RIGHT,
	_XA_NET_WM_STATE_MAXIMUS_TOP,
	_XA_NET_WM_STATE_AUTORAISE,
	_XA_NET_WM_STATE_AUTOLOWER,

	/* _NET_WM_ALLOWED_ACTIONS atoms */
	_XA_NET_WM_ACTION_MOVE,
	_XA_NET_WM_ACTION_RESIZE,
	_XA_NET_WM_ACTION_MINIMIZE,
	_XA_NET_WM_ACTION_SHADE,
	_XA_NET_WM_ACTION_STICK,
	_XA_NET_WM_ACTION_MAXIMIZE_HORZ,
	_XA_NET_WM_ACTION_MAXIMIZE_VERT,
	_XA_NET_WM_ACTION_FULLSCREEN,
	_XA_NET_WM_ACTION_CHANGE_DESKTOP,
	_XA_NET_WM_ACTION_CLOSE,
	_XA_NET_WM_ACTION_ABOVE,
	_XA_NET_WM_ACTION_BELOW,

	/* Window Manager Protocols */
	_XA_NET_WM_PING,
	_XA_NET_WM_SYNC_REQUEST,
	_XA_NET_WM_SYNC_REQUEST_COUNTER,
	_XA_NET_WM_FULLSCREEN_MONITORS,

	/* Other Properties */
	/* _XA_NET_WM_FULL_PLACEMENT, */

	/* Compositing Managers */
	_XA_NET_WM_WINDOW_OPACITY,

	/* Startup Notification */
	_XA_NET_STARTUP_ID,
	_XA_NET_STARTUP_INFO_BEGIN,
	_XA_NET_STARTUP_INFO,

	/* System Tray */
	/* _XA_NET_SYSTEM_TRAY_MESSAGE_DATA, */
	/* _XA_NET_SYSTEM_TRAY_OPCODE, */
	/* _XA_NET_SYSTEM_TRAY_ORIENTATION, */
	/* _XA_NET_SYSTEM_TRAY_VISUAL, */

	/* Additional non-standard properties */
	_XA_NET_MAXIMIZED_RESTORE,
	_XA_NET_PROPERTIES,
	_XA_NET_RESTART,
	_XA_NET_SHUTDOWN,
	/* _XA_NET_VIRTUAL_POS, */
	_XA_NET_WM_DESKTOP_MASK
    };
#ifdef DEBUG_EWMH
    fprintf(stderr, "Setting _NET_SUPPORTED on root 0x%08x\n", root);
#endif
    XChangeProperty(dpy, root, _XA_NET_SUPPORTED, XA_ATOM, 32, PropModeReplace,
		    (unsigned char *) data, sizeof(data) / sizeof(long));
}

/** @brief Update the supported features array.
  *
  * Called automatically by the initialization function, InitEwmh().
  */
void
Upd_NET_SUPPORTED(ScreenInfo *scr)
{
    Set_NET_SUPPORTED(scr->Root);
    scr->ewmh.props._NET_SUPPORTED = 1;
}

/** @brief Delete the supported features array.
  *
  * Called automatically by the termination function, TermEwmh(), to remove the
  * supported features array from the root window at exit.  It is not necessary
  * to call this for restart.
  */
static void
Del_NET_SUPPORTED(ScreenInfo *scr)
{
    XDeleteProperty(dpy, scr->Root, _XA_NET_SUPPORTED);
    scr->ewmh.props._NET_SUPPORTED = 0;
}

/** @} */

/** @name _NET_CLIENT_LIST, WINDOW[]/32
  *
  * These arrays contains all X windows managed by the window manager.
  * _NET_CLIENT_LIST has initial mapping order, starting with the oldest window.
  * _NET_CLIENT_LIST_STACKING has bottom-to-top stacking order.  These properties
  * SHOULD be set and updated by the window manager.
  *
  * @{ */
static void
Set_NET_CLIENT_LIST(Window root, Window *windows, int count)
{
    if (count < 0) {
	XDeleteProperty(dpy, root, _XA_NET_CLIENT_LIST);
    } else {
	long data[count];
	int i;

	for (i = 0; i < count; i++)
	    data[i] = (long) windows[i];

#ifdef DEBUG_EWMH
	fprintf(stderr, "Setting _NET_CLIENT_LIST on root 0x%08x\n", root);
#endif
	XChangeProperty(dpy, root, _XA_NET_CLIENT_LIST, XA_WINDOW, 32, PropModeReplace,
			(unsigned char *) data, count);
    }
}

/** @brief Update the client list.
  * @param scr - screen
  *
  * Should be called by the window manager whenever it adds or removes a managed
  * window.  Note that this property is only updated when the list actually
  * changes (or changes order).
  */
void
Upd_NET_CLIENT_LIST(ScreenInfo *scr)
{
    Window *clients = NULL;
    int count = 0;

    TwmGetClientList(scr, &clients, &count);
    if (!scr->ewmh.props._NET_CLIENT_LIST
	|| cmp_window_list(scr->ewmh.clients, clients) != 0) {
	Set_NET_CLIENT_LIST(scr->Root, clients, count);
	scr->ewmh.props._NET_CLIENT_LIST = 1;
	free(scr->ewmh.clients);
	scr->ewmh.clients = clients;
    }
}

/** @brief Delete the client list.
  *
  * Called by the window manager when exiting (but not restarting).
  */
static void
Del_NET_CLIENT_LIST(ScreenInfo *scr)
{
    XDeleteProperty(dpy, scr->Root, _XA_NET_CLIENT_LIST);
    scr->ewmh.props._NET_CLIENT_LIST = 0;
}

/** @} */

/** @name _NET_CLIENT_LIST_STACKING, WINDOW[]/32
  *
  * These arrays contains all X windows managed by the window manager.
  * _NET_CLIENT_LIST has initial mapping order, starting with the oldest window.
  * _NET_CLIENT_LIST_STACKING has bottom-to-top stacking order.  These properties
  * SHOULD be set and updated by the window manager.
  *
  * Stacking order: To obtain good interoperability between different Desktop
  * Environments the following layered stacking order is recommended, from the
  * bottom:
  *
  * 1. windows of type _NET_WM_TYPE_DESKTOP
  *
  * 2. windows having state _NET_WM_STATE_BELOW
  *
  * 3. windows not belonging in any other layer
  *
  * 4. windows of type _NET_WM_TYPE_DOCK (unless they have state
  *    _NET_WM_STATE_BELOW) and windows having state _NET_WM_STATE_ABOVE
  *
  * 5. focused windows having state _NET_WM_STATE_FULLSCREEN
  *
  * @{ */
static void
Set_NET_CLIENT_LIST_STACKING(Window root, Window *windows, int count)
{
    if (count < 1 || windows == NULL) {
	XDeleteProperty(dpy, root, _XA_NET_CLIENT_LIST_STACKING);
    } else {
	long data[count];
	int i;

	for (i = 0; i < count; i++) {
	    data[i] = (long) windows[i];
	}
#ifdef DEBUG_EWMH
	fprintf(stderr, "Setting _NET_CLIENT_LIST_STACKING on root 0x%08x\n", root);
#endif
	XChangeProperty(dpy, root, _XA_NET_CLIENT_LIST_STACKING, XA_WINDOW, 32,
			PropModeReplace, (unsigned char *) data, count);
    }
}

/** @brief Update the client list in stacking order.
  * @param scr - screen
  *
  * Should be called by the window manager whenever it adds or removes a managed
  * window, or the stacking order of windows changes.  Note that this property
  * is only updated when the list actually changes (or changes order).
  */
void
Upd_NET_CLIENT_LIST_STACKING(ScreenInfo *scr)
{
    Window *stacking = NULL;
    int count = 0;

    TwmGetClientListStacking(scr, &stacking, &count);
    if (!scr->ewmh.props._NET_CLIENT_LIST_STACKING
	|| cmp_window_list(scr->ewmh.stacking, stacking) != 0) {
	Set_NET_CLIENT_LIST_STACKING(scr->Root, stacking, count);
	scr->ewmh.props._NET_CLIENT_LIST_STACKING = 1;
	free(scr->ewmh.stacking);
	scr->ewmh.stacking = stacking;
    }
}

static void
Del_NET_CLIENT_LIST_STACKING(ScreenInfo *scr)
{
    XDeleteProperty(dpy, scr->Root, _XA_NET_CLIENT_LIST_STACKING);
}

/** @} */

/** @name _NET_NUMBER_OF_DESKTOPS, CARDINAL/32
  *
  * This property SHOULD be set and updated by the window manager to indicate the
  * number of virtual desktops.
  *
  * @{ */
static void
Set_NET_NUMBER_OF_DESKTOPS(Window root, long number)
{
#ifdef DEBUG_EWMH
    fprintf(stderr, "Setting _NET_NUMBER_OF_DESKTOPS to %ld on root 0x%08x\n", number, root);
#endif
    XChangeProperty(dpy, root, _XA_NET_NUMBER_OF_DESKTOPS, XA_CARDINAL, 32,
		    PropModeReplace, (unsigned char *) &number, 1);
}

static Bool
Get_NET_NUMBER_OF_DESKTOPS(Window root, int *number)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, root, _XA_NET_NUMBER_OF_DESKTOPS, 0L, 1L, False,
			   XA_CARDINAL, &actual_type, &actual_format, &nitems,
			   &bytes_after, (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 1) {
	if (prop != NULL)
	    XFree(prop);
	*number = 0;
	return False;
    }
    *number = prop[0];
    XFree(prop);
    return True;
}

/** @brief Update the number of desktops.
  * @param scr - screen
  *
  * Should be called if ever the number of desktops changes.  The property will
  * not be updated unless the number actually changes.
  */
void
Upd_NET_NUMBER_OF_DESKTOPS(ScreenInfo *scr)
{
    int number = 1;

    TwmGetNumberOfDesktops(scr, &number);
    if (!scr->ewmh.props._NET_NUMBER_OF_DESKTOPS || scr->ewmh.desktops != number) {
	Set_NET_NUMBER_OF_DESKTOPS(scr->Root, number);
	scr->ewmh.props._NET_NUMBER_OF_DESKTOPS = 1;
	scr->ewmh.desktops = number;

	/* these things need to be updated when the number of desktops changes */
	Upd_NET_DESKTOP_VIEWPORT(scr);
	Upd_NET_DESKTOP_NAMES(scr);
	Upd_NET_WORKAREA(scr);
	Upd_NET_VIRTUAL_ROOTS(scr);
	Upd_NET_DESKTOP_LAYOUT(scr);
    }
}

static void
Ret_NET_NUMBER_OF_DESKTOPS(ScreenInfo *scr)
{
    int number;

    if (Get_NET_NUMBER_OF_DESKTOPS(scr->Root, &number)) {
	scr->ewmh.props._NET_NUMBER_OF_DESKTOPS = 1;
	TwmSetNumberOfDesktops(scr, number);
    } else
	Upd_NET_NUMBER_OF_DESKTOPS(scr);
}

/* A pager can request a change in the number of desktops by sending a
 * _NET_NUMBER_OF_DESKTOP message to the root window.
 *
 * The window manager is free to honor or reject this request.  If the request
 * is honored, _NET_NUMBER_OF_DESKTOPS MUST be set to the new number of
 * desktops, _NET_VIRTUAL_ROOTS MUST be set to store the new number of desktop
 * virtual root window IDs and _NET_DESKTOP_VIEWPORT and _NET_WORKAREA must also
 * be changed accordingly.  The _NET_DESKTOP_NAMES property MAY remain
 * unchanged.
 *
 * If the number of desktops is shrinking and _NET_CURRENT_DESKTOP is out of the
 * new range of available desktops, then this MUST be set to the last available
 * desktop from the new set. Clients that are still present on desktops that are
 * out of the new range MUST be moved to the very last desktop from the new set.
 * For these _NET_WM_DESKTOP MUST be updated.
 */
static void
Rcv_NET_NUMBER_OF_DESKTOPS(ScreenInfo *scr, XClientMessageEvent *event)
{
    long number = event->data.l[0];

    if (64L >= number && number >= 1L)
	TwmSetNumberOfDesktops(scr, number);
}

/** @} */

/** @name _NET_DESKTOP_GEOMETRY width, height, CARDINAL[2]/32
  *
  * Array of two cardinals that defines the common size of all desktops (this is
  * equal to the screen size if the Window Manager doesn't support large
  * desktops, otherwise it's equal to the virtual size of the desktop). This
  * property SHOULD be set by the Window Manager.
  *
  * @{ */
static void
Set_NET_DESKTOP_GEOMETRY(Window root, struct NetSize *geometry)
{
#ifdef DEBUG_EWMH
    fprintf(stderr, "Setting _NET_DESKTOP_GEOMETRY to %ld x %ld on root 0x%08x\n", geometry->width, geometry->height, root);
#endif
    XChangeProperty(dpy, root, _XA_NET_DESKTOP_GEOMETRY, XA_CARDINAL, 32, PropModeReplace,
		    (unsigned char *) geometry, 2);
}

static Bool
Get_NET_DESKTOP_GEOMETRY(Window root, struct NetSize *geometry)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, root, _XA_NET_DESKTOP_GEOMETRY, 0L, 2L, False,
			   XA_CARDINAL, &actual_type, &actual_format, &nitems,
			   &bytes_after, (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 2) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    *geometry = *(struct NetSize *) prop;
    XFree(prop);
    return True;
}

/** @brief Update the desktop geometry.
  * @param scr - screen
  *
  * Should be called whenever the desktop geometry changes.  The property will
  * not be updated unless the geometry actually changes.
  */
void
Upd_NET_DESKTOP_GEOMETRY(ScreenInfo *scr)
{
    struct NetSize geometry = { scr->rootw, scr->rooth };

    TwmGetDesktopGeometry(scr, &geometry);
    if (!scr->ewmh.props._NET_DESKTOP_GEOMETRY
	|| cmp_size(&scr->ewmh.geometry, &geometry) != 0) {
	Set_NET_DESKTOP_GEOMETRY(scr->Root, &geometry);
	scr->ewmh.props._NET_DESKTOP_GEOMETRY = 1;
	scr->ewmh.geometry = geometry;
    }
}

static void
Ret_NET_DESKTOP_GEOMETRY(ScreenInfo *scr)
{
    struct NetSize geometry = { scr->rootw, scr->rooth };

    if (Get_NET_DESKTOP_GEOMETRY(scr->Root, &geometry)) {
	TwmSetDesktopGeometry(scr, &geometry);
	scr->ewmh.props._NET_DESKTOP_GEOMETRY = 1;
	scr->ewmh.geometry = geometry;
    } else {
	scr->ewmh.props._NET_DESKTOP_GEOMETRY = 0;
	scr->ewmh.geometry = geometry;
    }
    Upd_NET_DESKTOP_GEOMETRY(scr);
}

/*
 * A pager can request a change in the desktop geometry by sending a
 * _NET_DESKTOP_GEOMETRY client message to the root window.
 *
 * The Window Manager MAY choose to ignore this message, in which case,
 * _NET_DESKTOP_GEOMETRY property will remain unchanged.
 *
 * Setting the geometry of the virtual desktop does not make sense unless large
 * virtual desktops (larger than the screen size) is supported; otherwise, it
 * cannot be set.  So this won't make any sense until we integrate VTWM.
 */
static void
Rcv_NET_DESKTOP_GEOMETRY(ScreenInfo *scr, XClientMessageEvent *event)
{
    struct NetSize geometry = { 0, 0 };
    geometry.width = event->data.l[0];
    geometry.height = event->data.l[1];
    TwmSetDesktopGeometry(scr, &geometry);
    Upd_NET_DESKTOP_GEOMETRY(scr);
}

/** @} */

/** @name _NET_DESKTOP_VIEWPORT x, y, CARDINAL[][2]/32
  *
  * Array of pairs of cardinals that define the top left corner of each desktop's
  * viewport. For Window Managers that don't support large desktops, this MUST
  * always be set to (0,0).
  *
  * @{ */
static void
Set_NET_DESKTOP_VIEWPORT(Window root, struct NetPosition *pos, int desktops)
{
#ifdef DEBUG_EWMH
    fprintf(stderr, "Setting _NET_DESKTOP_VIEWPORT for %d desktops on root 0x%08x\n", desktops, root);
#endif
    XChangeProperty(dpy, root, _XA_NET_DESKTOP_VIEWPORT, XA_CARDINAL, 32, PropModeReplace,
		    (unsigned char *) pos, desktops * 2);
}

static Bool
Get_NET_DESKTOP_VIEWPORT(Window root, struct NetPosition **pos, int *desktops)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0, len, n, i;
    long *prop = NULL;

    if (*pos != NULL) {
	free(*pos);
	*pos = NULL;
	*desktops = 0;
    }
    status =
	XGetWindowProperty(dpy, root, _XA_NET_DESKTOP_VIEWPORT, 0L, 2L, False,
			   XA_CARDINAL, &actual_type, &actual_format, &nitems,
			   &bytes_after, (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 2) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    XFree(prop);
    len = ((bytes_after + 3) >> 2) + 2;
    status =
	XGetWindowProperty(dpy, root, _XA_NET_DESKTOP_VIEWPORT, 0L, len, False,
			   XA_CARDINAL, &actual_type, &actual_format, &nitems,
			   &bytes_after, (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < len || nitems & 0x1) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    if ((*pos = calloc(nitems >> 1, sizeof(struct NetPosition))) == NULL)
	return False;
    for (n = 0, i = 0; n < nitems; n += 2, i++) {
	(*pos)[i].x = prop[n + 0];
	(*pos)[i].y = prop[n + 1];
    }
    *desktops = i;
    XFree(prop);
    return True;
}

/** @brief Update the desktop viewports.
  * @param scr - screen
  *
  * Should be called if any of the viewports have changed or when the number of
  * desktops changes.  Note that this function only updates the property when
  * some viewport in the list changes.
  *
  * This function should also be called whenever the number of desktops changes.
  */
void
Upd_NET_DESKTOP_VIEWPORT(ScreenInfo *scr)
{
    struct NetPosition *viewport = NULL;
    int viewports = 0;

    TwmGetDesktopViewports(scr, &viewport, &viewports);
    if (!scr->ewmh.props._NET_DESKTOP_VIEWPORT
	|| cmp_pos_list(scr->ewmh.viewport, scr->ewmh.viewports, viewport,
			viewports) != 0) {
	Set_NET_DESKTOP_VIEWPORT(scr->Root, viewport, viewports);
	scr->ewmh.props._NET_DESKTOP_VIEWPORT = 1;
	free(scr->ewmh.viewport);
	scr->ewmh.viewport = viewport;
	scr->ewmh.viewports = viewports;
    }
}

static void
Ret_NET_DESKTOP_VIEWPORT(ScreenInfo *scr)
{
    struct NetPosition *viewport = NULL;
    int viewports = 0, n;

    if (Get_NET_DESKTOP_VIEWPORT(scr->Root, &viewport, &viewports)) {
	for (n = 0; n < viewports; n++)
	    TwmSetDesktopViewport(scr, n, &viewport[n]);
	scr->ewmh.props._NET_DESKTOP_VIEWPORT = 1;
	free(scr->ewmh.viewport);
	scr->ewmh.viewport = viewport;
	scr->ewmh.viewports = viewports;
    } else {
	scr->ewmh.props._NET_DESKTOP_VIEWPORT = 0;
	free(scr->ewmh.viewport);
	scr->ewmh.viewport = NULL;
	scr->ewmh.viewports = 0;
    }
    Upd_NET_DESKTOP_VIEWPORT(scr);
}

/** @brief Receive change request for desktop viewport.
  * @param scr - screen
  * @param event - client message event
  *
  * A pager can request to change the viewport for the current desktop by
  * sending a _NET_DESKTOP_VIEWPORT client message to the root window.  The
  * Window Manager MAY choose to ignore this message, in which case the
  * _NET_DESKTOP_VIEWPORT property will remain unchanged.
  *
  * Currently there are no large desktops and the viewport is always (0,0) for
  * each desktop.  When we strap in the vtwm(1) code, there will be large
  * desktops and a way to change the viewport.
  */
static void
Rcv_NET_DESKTOP_VIEWPORT(ScreenInfo *scr, XClientMessageEvent *event)
{
    struct NetPosition viewport = { 0, 0 };
    int current;

    viewport.x = event->data.l[0];
    viewport.y = event->data.l[1];
    TwmGetCurrentDesktop(scr, &current);
    TwmSetDesktopViewport(scr, current, &viewport);
}

/** @} */

/** @name _NET_CURRENT_DESKTOP
  *
  * @{ */
static void
Set_NET_CURRENT_DESKTOP(Window root, long desktop)
{
#ifdef DEBUG_EWMH
    fprintf(stderr, "Setting _NET_CURRENT_DESKTOP to %ld on root 0x%08x\n", desktop, root);
#endif
    XChangeProperty(dpy, root, _XA_NET_CURRENT_DESKTOP, XA_CARDINAL, 32, PropModeReplace,
		    (unsigned char *) &desktop, 1);
}

static Bool
Get_NET_CURRENT_DESKTOP(Window root, int *desktop)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, root, _XA_NET_CURRENT_DESKTOP, 0L, 1L, False, XA_CARDINAL,
			   &actual_type, &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 1) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    *desktop = prop[0];
    XFree(prop);
    return True;
}

/** @brief Update the current desktop.
  * @param scr -screen
  *
  * Should be called whenever the current desktop changes.  Note that this
  * function will not change the property unless the current desktop actually
  * changes.
  */
void
Upd_NET_CURRENT_DESKTOP(ScreenInfo *scr)
{
    int current = -1;

    TwmGetCurrentDesktop(scr, &current);
    if (!scr->ewmh.props._NET_CURRENT_DESKTOP || scr->ewmh.current != current) {
	Set_NET_CURRENT_DESKTOP(scr->Root, current);
	scr->ewmh.props._NET_CURRENT_DESKTOP = 1;
	scr->ewmh.current = current;
    }
}

/** @brief Retrieve the current desktop.
  * @param scr - screen
  *
  * Call this function when initializing TWM to read or write the current
  * desktop to the root.
  */
static void
Ret_NET_CURRENT_DESKTOP(ScreenInfo *scr)
{
    int current = -1;

    if (Get_NET_CURRENT_DESKTOP(scr->Root, &current)) {
	TwmSetCurrentDesktop(scr, current, CurrentTime);
	scr->ewmh.props._NET_CURRENT_DESKTOP = 1;
	scr->ewmh.current = current;
    } else {
	scr->ewmh.props._NET_CURRENT_DESKTOP = 0;
	scr->ewmh.current = -1;
    }
    Upd_NET_CURRENT_DESKTOP(scr);
}

/*
 * If a pager wants to switch to another virtual desktop, it MUST send a
 * _NET_CURRENT_DESKTOP client message to the root window.  Note that the
 * timestamp may be 0 for clients using an older version of the specification,
 * in which case, the timestamp field should be ignored.
 */
static void
Rcv_NET_CURRENT_DESKTOP(ScreenInfo *scr, XClientMessageEvent *event)
{
    int new_index = event->data.l[0];
    Time timestamp = event->data.l[1];

    TwmSetCurrentDesktop(scr, new_index, timestamp);
}

/** @} */

/** @name _NET_DESKTOP_NAMES, UTF8_STRING[]
  *
  * The names of all virtual desktops.  This is a list of NULL-terminated strings
  * in UTF-8 encoding [UTF8].  This property MAY be changed by a Pager or the
  * Window Manager at any time.
  *
  * Note: The number of names could be different from _NET_NUMBER_OF_DESKTOPS.
  * If it is less than _NET_NUMBER_OF_DESKTOPS, then the desktops with high
  * numbers are unnamed.  If it is larger than _NET_NUMBER_OF_DESKTOPS, then the
  * excess names outside of the _NET_NUMBER_OF_DESKTOPS are considered to be
  * reserved in case the number of desktops is increased.
  *
  * Rationale: The name is not a necessary attribute of a virtual desktop.  Thus
  * the availability or unavailability of names has no impact on virtual desktop
  * functionality.  Since names are set by users and users are likely to preset
  * names for a fixed number of desktops, it doesn't make sense to shrink or grow
  * this list when the number of available desktops changes.
  *
  * @{ */
static void
Set_NET_DESKTOP_NAMES(Window root, char **names, int count)
{
    int status;
    XTextProperty text = { NULL, };

    status = Xutf8TextListToTextProperty(dpy, names, count, XUTF8StringStyle, &text);
    if (status == Success) {
	/* Nul terminated, not null separated */
	text.value[text.nitems] = '\0';
	text.nitems += 1;
	XSetTextProperty(dpy, root, &text, _XA_NET_DESKTOP_NAMES);
	XFree(text.value);
    }
}

static Bool
Get_NET_DESKTOP_NAMES(Window root, char ***names, int *count)
{
    XTextProperty text = { NULL, };

    if (*names != NULL) {
	XFreeStringList(*names);
	*count = 0;
	*names = NULL;
    }
    if (XGetTextProperty(dpy, root, &text, _XA_NET_DESKTOP_NAMES) == Success) {
	/* Nul terminated, not null separated */
	if (text.nitems > 0)
	    text.nitems -= 1;
	Xutf8TextPropertyToTextList(dpy, &text, names, count);
	return True;
    }
    return False;
}

/** @brief Update the desktop names.
  * @param scr - screen
  *
  * Should be called whenever the desktop names change.  Note that this function
  * does not set the property unless the names actually change.
  */
void
Upd_NET_DESKTOP_NAMES(ScreenInfo *scr)
{
    char **names = NULL;
    int count = 0;

    TwmGetDesktopNames(scr, &names, &count);
    if (!scr->ewmh.props._NET_DESKTOP_NAMES || strcmp_list(names, scr->ewmh.names) != 0) {
	Set_NET_DESKTOP_NAMES(scr->Root, names, count);
	scr->ewmh.props._NET_DESKTOP_NAMES = 1;
	if (scr->ewmh.names != NULL)
	    XFreeStringList(scr->ewmh.names);
	scr->ewmh.names = names;
    } else if (names != NULL)
	XFreeStringList(names);
}

static void
Ret_NET_DESKTOP_NAMES(ScreenInfo *scr)
{
    char **names = NULL;
    int count = 0;

    if (Get_NET_DESKTOP_NAMES(scr->Root, &names, &count)) {
	TwmSetDesktopNames(scr, names, count);
	scr->ewmh.props._NET_DESKTOP_NAMES = 1;
	if (strcmp_list(names, scr->ewmh.names) != 0) {
	    if (scr->ewmh.names != NULL)
		XFreeStringList(scr->ewmh.names);
	    scr->ewmh.names = names;
	} else if (names != NULL)
	    XFreeStringList(names);
    } else {
	scr->ewmh.props._NET_DESKTOP_NAMES = 0;
	if (scr->ewmh.names != NULL)
	    XFreeStringList(scr->ewmh.names);
	scr->ewmh.names = NULL;
    }
    Upd_NET_DESKTOP_NAMES(scr);
}

/** @} */

/** @name _NET_ACTIVE_WINDOW, WINDOW/32
  * 
  * The window ID of the currently active window or None if no window has the
  * focus. This is a read-only property set by the Window Manager.
  *
  * @{ */
static void
Set_NET_ACTIVE_WINDOW(Window root, Window window)
{
    long data = (long) window;

#ifdef DEBUG_EWMH
    fprintf(stderr, "Setting _NET_ACTIVE_WINOW to 0x%08x on root 0x%08x\n", window, root);
#endif
    XChangeProperty(dpy, root, _XA_NET_ACTIVE_WINDOW, XA_WINDOW, 32, PropModeReplace,
		    (unsigned char *) &data, 1);
}

static Bool
Get_NET_ACTIVE_WINDOW(Window root, Window *window)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, root, _XA_NET_ACTIVE_WINDOW, 0L, 1L, False, XA_WINDOW,
			   &actual_type, &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 1) {
	if (prop != NULL)
	    XFree(prop);
	*window = None;
	return False;
    }
    *window = prop[0];
    XFree(prop);
    return True;
}

/** @brief Update the active window.
  * @param scr - screen
  *
  * Should be called by TWM whenever the active window changes.  This function
  * does not change the property unless it actually changes.
  */
void
Upd_NET_ACTIVE_WINDOW(ScreenInfo *scr)
{
    Window active = None;

    TwmGetActiveWindow(scr, &active);
    if (!scr->ewmh.props._NET_ACTIVE_WINDOW || active != scr->ewmh.active) {
	Set_NET_ACTIVE_WINDOW(scr->Root, active);
	scr->ewmh.props._NET_ACTIVE_WINDOW = 1;
	scr->ewmh.active = active;
    }
}

static void
Ret_NET_ACTIVE_WINDOW(ScreenInfo *scr)
{
    Window active = None;
    TwmWindow *twin = NULL;

    if (Get_NET_ACTIVE_WINDOW(scr->Root, &active)) {
	if ((twin = TwmFindWindow(scr, active)) != NULL)
	    TwmSetActiveWindow(twin, None, CurrentTime, _NET_SOURCE_UNSPECIFIED);
	scr->ewmh.props._NET_ACTIVE_WINDOW = 1;
	scr->ewmh.active = active;
    } else {
	scr->ewmh.props._NET_ACTIVE_WINDOW = 0;
	scr->ewmh.active = None;
    }
    Upd_NET_ACTIVE_WINDOW(scr);
}

/** @brief Handle request to change active window.
  * @param scr - screen
  * @param event - client message
  *
  * If a client wants to activate a window, it MUST send a _NET_ACTIVE_WINDOW
  * client message to the root window.
  *
  * Source indiciation should be 1 when the request comes from an application,
  * and 2 when it comes from a pager.  Clients using older versions of the
  * specification use 0 as source indication.  The timestamp is Client's last
  * user activity timestamp at the time of the request, and the currently active
  * window is the Client's active toplevel window, if any (the Window Manager
  * may be more likely to obey the request if it will mean transferring focus
  * from one active window to another).
  *
  * Depending on the information provided with the message, the Window Manager
  * may decide to refuse the request (either completely ignore it, or use
  * _NET_WM_STATE_DEMANDS_ATTENTION).
  */
static void
Rcv_NET_ACTIVE_WINDOW(ScreenInfo *scr, XClientMessageEvent *event)
{
    TwmWindow *twin = NULL;
    Window active = event->window;
    enum _NET_SOURCE source = (int) event->data.l[0];
    Time timestamp = (Time) event->data.l[1];
    Window present = (Window) event->data.l[2];

    switch (source) {
    case _NET_SOURCE_UNSPECIFIED:
    case _NET_SOURCE_APPLICATION:
    case _NET_SOURCE_PAGER:
	break;
    default:
#ifdef DEBUG_EWMH
	fprintf(stderr, "Received _NET_ACTIVE_WINDOW message with invalid source %d\n", source);
	fflush(stderr);
#endif
	return;
    }
    if ((twin = TwmFindWindow(scr, active)) != NULL)
	TwmSetActiveWindow(twin, present, timestamp, source);
    Upd_NET_ACTIVE_WINDOW(scr);
}

/** @} */

/** @name _NET_WORKAREA, x, y, width, height CARDINAL[][4]/32
  *
  * This property MUST be set by the Window Manager upon calculating the work
  * area for each desktop. Contains a geometry for each desktop.  These
  * geometries are specified relative to the viewport on each desktop and specify
  * an area that is completely contained within the viewport.  Work area SHOULD
  * be used by desktop applications to place desktop icons appropriately.
  *
  * The Window Manager SHOULD calculate this space by taking the current page
  * minus space occupied by dock and panel windows, as indicated by the
  * _NET_WM_STRUT or _NET_WM_STRUT_PARTIAL properties set on client windows.
  *
  * @{ */
static void
Set_NET_WORKAREA(Window root, struct NetGeometry *workarea, int workareas)
{
    XChangeProperty(dpy, root, _XA_NET_WORKAREA, XA_CARDINAL, 32, PropModeReplace,
		    (unsigned char *) workarea, workareas * 4);
}

/** @brief Update the desktop workarea.
  * @param scr - screen
  *
  * Should be called whenever a window is added or removed that has a strut or
  * partial strut or when the number of desktops changes.  This can be called
  * internally and need not be called by TWM.  Note that this function only
  * changes the property when some workarea in the list changes.
  *
  * This function should also be called whenever the number of desktops changes.
  */
void
Upd_NET_WORKAREA(ScreenInfo *scr)
{
    struct NetGeometry *workarea = NULL;
    int workareas = 0;

    TwmGetWorkareas(scr, &workarea, &workareas);
    if (!scr->ewmh.props._NET_WORKAREA
	|| cmp_geo_list(scr->ewmh.workarea, scr->ewmh.workareas, workarea,
			workareas) != 0) {
	Set_NET_WORKAREA(scr->Root, workarea, workareas);
	scr->ewmh.props._NET_WORKAREA = 1;
	free(scr->ewmh.workarea);
	scr->ewmh.workarea = workarea;
	scr->ewmh.workareas = workareas;
    }
}

static void
Del_NET_WORKAREA(ScreenInfo *scr)
{
    XDeleteProperty(dpy, scr->Root, _XA_NET_WORKAREA);
    scr->ewmh.props._NET_WORKAREA = 0;
}

/** @} */

/** @name _NET_SUPPORTING_WM_CHECK, WINDOW/32
  *
  * The Window Manager MUST set this property on the root window to be the ID of
  * a child window created by himself, to indicate that a compliant window
  * manager is active. The child window MUST also have the
  * _NET_SUPPORTING_WM_CHECK property set to the ID of the child window.  The
  * child window MUST also have the _NET_WM_NAME property set to the name of the
  * Window Manager.
  *
  * Rationale: The child window is used to distinguish an active Window Manager
  * from a stale _NET_SUPPORTING_WM_CHECK property that happens to point to
  * another window. If the _NET_SUPPORTING_WM_CHECK window on the client window
  * is missing or not properly set, clients SHOULD assume that no conforming
  * Window Manager is present.
  *
  * @{ */
static void
Set_NET_SUPPORTING_WM_CHECK(Window root, Window window)
{
    long data = (long) window;

    XChangeProperty(dpy, window, _XA_NET_SUPPORTING_WM_CHECK, XA_WINDOW, 32,
		    PropModeReplace, (unsigned char *) &data, 1);
    XChangeProperty(dpy, root, _XA_NET_SUPPORTING_WM_CHECK, XA_WINDOW, 32,
		    PropModeReplace, (unsigned char *) &data, 1);
}

void
Upd_NET_SUPPORTING_WM_CHECK(ScreenInfo *scr)
{
    Window check = scr->ManagerWindow;

    if (!scr->ewmh.props._NET_SUPPORTING_WM_CHECK || scr->ewmh.check != check) {
	Set_NET_SUPPORTING_WM_CHECK(scr->Root, check);
	scr->ewmh.props._NET_SUPPORTING_WM_CHECK = 1;
	scr->ewmh.check = check;
    }
}

static void
Del_NET_SUPPORTING_WM_CHECK(ScreenInfo *scr)
{
    XDeleteProperty(dpy, scr->Root, _XA_NET_SUPPORTING_WM_CHECK);
    scr->ewmh.props._NET_SUPPORTING_WM_CHECK = 0;
    scr->ewmh.check = None;
}

/** @} */

/** @name _NET_VIRTUAL_ROOTS, WINDOW[]/32
  *
  * To implement virtual desktops, some Window Managers reparent client windows
  * to a child of the root window.  Window Managers using this technique MUST set
  * this property to a list of IDs for windows that are acting as virtual root
  * windows.  This property allows background setting programs to work with
  * virtual roots and allows clients to figure out the window manager frame
  * windows of their windows.
  *
  * @{ */
static void
Set_NET_VIRTUAL_ROOTS(Window root, Window *windows, int count)
{
    if (count > 0) {
	long data[count];
	int i;

	for (i = 0; i < count; i++)
	    data[i] = (long) windows[i];
	XChangeProperty(dpy, root, _XA_NET_VIRTUAL_ROOTS, XA_WINDOW, 32, PropModeReplace,
			(unsigned char *) data, count);
    } else {
	XDeleteProperty(dpy, root, _XA_NET_VIRTUAL_ROOTS);
    }
}

/** @brief Update the virtual roots.
  * @param scr - screen
  *
  * Update virtual roots.  This procedure should be called once for each screen.
  * Note that the property will only change when the number or order of virtual
  * roots changes.
  *
  * It should be called when the number of desktops changes.
  */
void
Upd_NET_VIRTUAL_ROOTS(ScreenInfo *scr)
{
    Window *vroots = NULL;
    int count = 0;

    TwmGetVirtualRoots(scr, &vroots, &count);
    if (!scr->ewmh.props._NET_VIRTUAL_ROOTS
	|| cmp_window_list(scr->ewmh.vroots, vroots) != 0) {
	Set_NET_VIRTUAL_ROOTS(scr->Root, vroots, count);
	scr->ewmh.props._NET_VIRTUAL_ROOTS = 1;
	free(scr->ewmh.vroots);
	scr->ewmh.vroots = vroots;
    }
}

void
Del_NET_VIRTUAL_ROOTS(ScreenInfo *scr)
{
    XDeleteProperty(dpy, scr->Root, _XA_NET_VIRTUAL_ROOTS);
    scr->ewmh.props._NET_VIRTUAL_ROOTS = 0;
}

/** @} */

/** @name _NET_DESKTOP_LAYOUT, orientation, columns, rows, starting_corner CARDINAL[4]/32
  *
  * This property is set by a Pager, not by the Window Manager. When setting this
  * property, the Pager must own a manager selection (as defined in the ICCCM
  * 2.8).  The manager selection is called _NET_DESKTOP_LAYOUT_Sn where n is the
  * screen number.  The purpose of this property is to allow the Window Manager
  * to know the desktop layout displayed by the Pager.
  *
  * @{ */
static void
Set_NET_DESKTOP_LAYOUT(Window root, struct NetLayout *layout)
{
    XChangeProperty(dpy, root, _XA_NET_DESKTOP_LAYOUT, XA_CARDINAL, 32, PropModeReplace,
		    (unsigned char *) layout, 4);
}

static Bool
Get_NET_DESKTOP_LAYOUT(Window root, struct NetLayout *layout)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, root, _XA_NET_DESKTOP_LAYOUT, 0L, 4L, False, XA_CARDINAL,
			   &actual_type, &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    if (status != Success || actual_type == None || actual_format != 32 || nitems < 3) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    layout->orientation = prop[0];
    layout->columns = prop[1];
    layout->rows = prop[2];
    if (nitems >= 4) {
	layout->starting_corner = prop[3];
    } else {
	layout->starting_corner = _NET_WM_STARTING_CORNER_TOPLEFT;
    }
    XFree(prop);
    return True;
}

/** @brief Update desktop layout.
  * @param scr - screen
  *
  * Updates the desktop layout to the current layout of the workspace manager.
  * We do not change the layout unless we own the _NET_WM_DESKTOP_LAYOUT_Sn
  * selection.
  *
  * Note that this function does not change the property unless the layout
  * actually changes.
  *
  * Should be called when the number of desktops changes.
  */
void
Upd_NET_DESKTOP_LAYOUT(ScreenInfo *scr)
{
    if (scr->ewmh.layout_sn.window != None) {
	struct NetLayout layout = { 1, 1 };

	TwmGetDesktopLayout(scr, &layout);
	if (!scr->ewmh.props._NET_DESKTOP_LAYOUT
	    || cmp_layout(&scr->ewmh.layout, &layout) != 0) {
	    Set_NET_DESKTOP_LAYOUT(scr->Root, &layout);
	    scr->ewmh.props._NET_DESKTOP_LAYOUT = 1;
	    scr->ewmh.layout = layout;
	}
    }
}

/*
 * Retrieves the desktop layout when it has changed or been deleted.  If we are
 * the owner of the _NET_DESKTOP_LAYOUT_Sn manager selection, then ignore any
 * changes.  When we are not, make our desktop layout match that of the
 * controlling pager.
 */
static void
Ret_NET_DESKTOP_LAYOUT(ScreenInfo *scr)
{
    if (scr->ewmh.layout_sn.window != None)
	/* ignore our own changes */
	return;
    else if (scr->ewmh.layout_sn.owner == None)
	/* ignore chages by non-selection owners */
	return;
    else if (Get_NET_DESKTOP_LAYOUT(scr->Root, &scr->ewmh.layout)) {
	scr->ewmh.props._NET_DESKTOP_LAYOUT = 1;
	TwmSetDesktopLayout(scr, &scr->ewmh.layout);
    }
}

/** @} */

/** @name _NET_DESKTOP_LAYOUT_Sn
  *
  * A manager selection in accordance with ICCCM 2.0 section 2.8 that is used to
  * control access to the _NET_DESKTOP_LAYOUT root window property.
  *
  * @{ */
/*
 * A new manager has announced itself as owning the _NET_DESKTOP_LAYOUT_Sn
 * selection.
 */
void
New_NET_DESKTOP_LAYOUT_Sn(ScreenInfo *scr, XClientMessageEvent *event)
{
    scr->ewmh.layout_sn.owner = event->data.l[2];
    XSelectInput(dpy, scr->ewmh.layout_sn.owner, StructureNotifyMask);
}

/*
 * We receive a selection clear event when some other pager has taken over the
 * _NET_DESKTOP_LAYOUT_Sn manager selection.  We need to destroy our window so
 * that the other manager will continue.
 */
void
Clr_NET_DESKTOP_LAYOUT_Sn(ScreenInfo *scr, XSelectionClearEvent * event)
{
    if (scr->ewmh.layout_sn.window != event->window)
	return;			/* not our clear */
    if (scr->ewmh.layout_sn.window != None) {
	XDestroyWindow(dpy, scr->ewmh.layout_sn.window);
	scr->ewmh.layout_sn.window = None;
    }
}

/*
 * The current owner of the _NET_DESKTOP_LAYOUT_Sn selection was destroyed,
 * meaning that there is no owner until we get a MANAGER message indicating a
 * new owner.
 */
void
Rel_NET_DESKTOP_LAYOUT_Sn(ScreenInfo *scr, XDestroyWindowEvent * event)
{
    scr->ewmh.layout_sn.owner = None;
}

/** @} */

/** @name _NET_SHOWING_DESKTOP desktop, CARDINAL/32
  *
  * Some Window Managers have a "showing the desktop" mode in which windows are
  * hidden, and the desktop background is displayed and focused.  If a Window
  * Manager supports the _NET_SHOWING_DESKTOP hint, it MUST set it to a value of
  * 1 when the Window Manager is in "showing the desktop" mode, and a value of
  * zero if the Window Manager is not in this mode.
  *
  * @{ */
static void
Set_NET_SHOWING_DESKTOP(Window root, long value)
{
    long data = value ? True : False;

    XChangeProperty(dpy, root, _XA_NET_SHOWING_DESKTOP, XA_CARDINAL, 32, PropModeReplace,
		    (unsigned char *) &data, 1);
}

static Bool
Get_NET_SHOWING_DESKTOP(Window root, Bool *flag)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, root, _XA_NET_SHOWING_DESKTOP, 0L, 1L, False, XA_CARDINAL,
			   &actual_type, &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 1) {
	if (prop != NULL)
	    XFree(prop);
	*flag = False;
	return False;
    }
    *flag = prop[0];
    XFree(prop);
    return True;
}

/** @brief Update showing desktop.
  * @param scr - screen
  *
  * This function must be called by TWM if it changes the showing desktop mode
  * internally.  Note that this function does not change the property unless the
  * mode actually changes.
  */
void
Upd_NET_SHOWING_DESKTOP(ScreenInfo *scr)
{
    Bool showing = False;

    TwmGetShowingDesktop(scr, &showing);
    if (!scr->ewmh.props._NET_SHOWING_DESKTOP || scr->ewmh.showing != showing) {
	Set_NET_SHOWING_DESKTOP(scr->Root, showing);
	scr->ewmh.props._NET_SHOWING_DESKTOP = 1;
	scr->ewmh.showing = showing;
    }
}

static void
Ret_NET_SHOWING_DESKTOP(ScreenInfo *scr)
{
    Bool showing = False;

    if (Get_NET_SHOWING_DESKTOP(scr->Root, &showing)) {
	TwmSetShowingDesktop(scr, showing);
	scr->ewmh.props._NET_SHOWING_DESKTOP = 1;
	scr->ewmh.showing = showing;
    } else {
	scr->ewmh.props._NET_SHOWING_DESKTOP = 0;
	scr->ewmh.showing = False;
    }
    Upd_NET_SHOWING_DESKTOP(scr);
}

/*
 * If a pager wants to enter or leave the mode, it MUST send a
 * _NET_SHOWING_DESKTOPO message to the root window requesting the change.
 *
 * The Window Manager may choose to ignore this client message.
 *
 * Neither CTWM nor VTWM have a showing desktop mode; however, we can add one
 * later once the code is integrated.
 */
static void
Rcv_NET_SHOWING_DESKTOP(ScreenInfo *scr, XClientMessageEvent *event)
{
    Bool showing = (Bool) event->data.l[0];

    TwmSetShowingDesktop(scr, showing);
    Upd_NET_SHOWING_DESKTOP(scr);
}

/** @} */

/** @} */

/** @name Application Window Properties.
  *
  * @{ */

/** @name WM_CLASS, res_name res_class STRING[2]/8
  *
  * @{ */

static void
Set_WM_CLASS(Window window, XClassHint *class)
{
    XTextProperty text = { NULL, };

    Xutf8TextListToTextProperty(dpy, (char **) class, 2, XStdICCTextStyle, &text);
    XSetTextProperty(dpy, window, &text, XA_WM_CLASS);
    XFree(text.value);
}

static Bool
Get_WM_CLASS(Window window, XClassHint *class)
{
    XTextProperty text = { NULL, };
    int status, count = 0;
    char **list = NULL;

    status = XGetTextProperty(dpy, window, &text, XA_WM_CLASS);
    if (status == Success && text.nitems > 0) {
	XTextPropertyToStringList(&text, &list, &count);
	if (count >= 1)
	    class->res_name = strdup(list[0]);
	if (count >= 2)
	    class->res_class = strdup(list[1]);
	XFreeStringList(list);
	XFree(text.value);
	return True;
    }
    XFree(text.value);
    return False;
}

static void
Ret_WM_CLASS(TwmWindow *twin)
{
    XClassHint class = { NULL, NULL };

    if (Get_WM_CLASS(twin->w, &class)) {
	if (class.res_name == NULL)
	    class.res_name = strdup("");
	if (class.res_class == NULL)
	    class.res_class = strdup("");
	twin->ewmh.props.WM_CLASS = 1;
	twin->ewmh.class = class;
    } else {
	twin->ewmh.props.WM_CLASS = 0;
	free(twin->ewmh.class.res_name);
	twin->ewmh.class.res_name = NULL;
	free(twin->ewmh.class.res_class);
	twin->ewmh.class.res_class = NULL;
    }
}

/** @} */

/** @name WM_PROTOCOLS protocol, protocol, ... ATOM[]/32
  *
  * The ICCCM 2.0 WM_PROTOCOLS property that is set by a client on an application
  * window contains the atoms of supported protocols.  Currently, these are
  * WM_DELETE, WM_SAVEYOURSELF, _NET_WM_PING and _NET_WM_SYNC.
  *
  * @{ */
static const Atom *WmProtocols[] = {
    NET_ATOM_ENTRY(_WM_TAKE_FOCUS),
    NET_ATOM_ENTRY(_WM_SAVE_YOURSELF),
    NET_ATOM_ENTRY(_WM_DELETE_WINDOW),
    NET_ATOM_ENTRY(_NET_WM_PING),
    NET_ATOM_ENTRY(_NET_WM_SYNC_REQUEST),
    [WM_PROTOCOLS_last] = NULL
};

static Bool
Get_WM_PROTOCOLS(Window window, unsigned *protocols)
{
    return Get_NET_WM_atoms(window, _XA_WM_PROTOCOLS, WmProtocols, protocols);
}

static void
Ret_WM_PROTOCOLS(TwmWindow *twin)
{
    unsigned protocols = 0;

    if (Get_WM_PROTOCOLS(twin->w, &protocols)) {
	twin->ewmh.props.WM_PROTOCOLS = 1;
	TwmSetWMProtocols(twin, protocols);
	twin->ewmh.protocols = protocols;
    } else
	twin->ewmh.props.WM_PROTOCOLS = 0;
}

/** @} */

/** @name WM_CLIENT_MACHINE STRING[]/8
  *
  * @{ */

static void
Set_WM_CLIENT_MACHINE(Window window, char *machine)
{
    XTextProperty text = { NULL, };

    Xutf8TextListToTextProperty(dpy, &machine, 1, XStdICCTextStyle, &text);
    XSetTextProperty(dpy, window, &text, XA_WM_CLIENT_MACHINE);
    XFree(text.value);
}

static Bool
Get_WM_CLIENT_MACHINE(Window window, char **machine)
{
    XTextProperty text = { NULL, };
    int status, count = 0;
    char **list = NULL;

    status = XGetTextProperty(dpy, window, &text, XA_WM_CLIENT_MACHINE);
    if (status == Success && text.nitems > 0) {
	XTextPropertyToStringList(&text, &list, &count);
	if (count > 0) {
	    free(*machine);
	    *machine = strdup(list[0]);
	}
	XFreeStringList(list);
	XFree(text.value);
	return True;
    }
    XFree(text.value);
    return False;
}

/** @brief Update the client machine for a window.
  * @param twin - TWM window
  *
  * Should be called when mapping a window and a startup notification sequence
  * has been assigned (after calling Ret_WM_CLIENT_MACHINE).  Note that this
  * function will only set the property when the property is not already set.
  */
void
Upd_WM_CLIENT_MACHINE(TwmWindow *twin)
{
    char *machine;
    EwmhSequence *seq;

    if (!twin->ewmh.props.WM_CLIENT_MACHINE && (seq = twin->ewmh.sequence) != NULL
	&& seq->field.hostname != NULL
	&& (machine = strdup(seq->field.hostname)) != NULL) {
	Set_WM_CLIENT_MACHINE(twin->w, machine);
	twin->ewmh.props.WM_CLIENT_MACHINE = 1;
	free(twin->ewmh.machine);
	twin->ewmh.machine = machine;
    }
}

/** @brief Retrieve or set the client machine for a window.
  * @param twin - TWM window
  *
  * Should be called when mapping a new window.
  */
static void
Ret_WM_CLIENT_MACHINE(TwmWindow *twin)
{
    char *machine = NULL;

    if (Get_WM_CLIENT_MACHINE(twin->w, &machine)) {
	twin->ewmh.props.WM_CLIENT_MACHINE = 1;
	free(twin->ewmh.machine);
	twin->ewmh.machine = machine;
    } else {
	twin->ewmh.props.WM_CLIENT_MACHINE = 0;
	free(twin->ewmh.machine);
	twin->ewmh.machine = NULL;
    }
    Upd_WM_CLIENT_MACHINE(twin);
}

/** @} */

/** @name WM_COMMAND STRING[]/8
  *
  * When there is no startup notification sequence associated with a window that
  * is being mapped and there is a WM_COMMAND property, the list of startup
  * notification sequences can be searched for a matching COMMAND= field and the
  * startup notification sequence associated with the window.  Unfortunately, we
  * cannot set the WM_COMMAND property because the COMMAND= field has not been
  * processed by the shell.
  *
  * @{ */

static void
Set_WM_COMMAND(Window window, char **command, int count)
{
    XTextProperty text = { NULL, };

    Xutf8TextListToTextProperty(dpy, command, count, XStdICCTextStyle, &text);
    XSetTextProperty(dpy, window, &text, XA_WM_COMMAND);
    XFree(text.value);
}

static Bool
Get_WM_COMMAND(Window window, char ***command, int *count)
{
    XTextProperty text = { NULL, };
    int status;

    status = XGetTextProperty(dpy, window, &text, XA_WM_COMMAND);
    if (status == Success && text.nitems > 0) {
	XTextPropertyToStringList(&text, command, count);
	if (*count > 0)
	    return True;
	XFree(text.value);
	return True;
    }
    XFree(text.value);
    return False;
}

static void
Ret_WM_COMMAND(TwmWindow *twin)
{
    char **command = NULL;
    int count = 0;

    twin->ewmh.props.WM_COMMAND = Get_WM_COMMAND(twin->w, &command, &count);
    if (twin->ewmh.command != NULL)
	XFreeStringList(twin->ewmh.command);
    twin->ewmh.command = command;
}

/** @} */

/** @name _NET_WM_NAME, UTF8_STRING/8
  *
  * The Client SHOULD set this to the title of the window in UTF-8 encoding.  If
  * set, the Window Manager should use this in preference to WM_NAME.
  *
  * It is not mentioned in the specifications, but the window manager should
  * monitor for property changes to this property so that the application may
  * change the name at any point providing dynamic text in title bars.
  *
  * @{ */
static void
Set_NET_WM_NAME(Window window, char *name)
{
    XTextProperty text = { NULL, };

    Xutf8TextListToTextProperty(dpy, &name, 1, XUTF8StringStyle, &text);
    XSetTextProperty(dpy, window, &text, _XA_NET_WM_NAME);
    XFree(text.value);
}

static Bool
Get_NET_WM_NAME(Window window, char **name)
{
    XTextProperty text = { NULL, };
    int status, count = 0;
    char **list = NULL;

    status = XGetTextProperty(dpy, window, &text, _XA_NET_WM_NAME);
    if (status == Success && text.nitems > 0) {
	XTextPropertyToStringList(&text, &list, &count);
	if (count > 0) {
	    free(*name);
	    *name = strdup(list[0]);
	}
	XFreeStringList(list);
	return True;
    }
    /* when no _NET_WM_NAME go after WM_NAME */
    status = XGetTextProperty(dpy, window, &text, XA_WM_NAME);
    if (status == Success && text.nitems > 0) {
	XTextPropertyToStringList(&text, &list, &count);
	if (count > 0) {
	    free(*name);
	    *name = strdup(list[0]);
	}
	XFreeStringList(list);
	return True;
    }
    return False;
}

/** @brief Retrieve the window name.
  * @param twin - the TWM window
  */
static void
Ret_NET_WM_NAME(TwmWindow *twin)
{
    if (Get_NET_WM_NAME(twin->w, &twin->ewmh.name))
	twin->ewmh.props._NET_WM_NAME = 1;
    else
	twin->ewmh.props._NET_WM_NAME = 0;
    TwmSetWMName(twin, twin->ewmh.name);

    Upd_NET_WM_VISIBLE_NAME(twin);
}

/** @} */

/** @name _NET_WM_VISIBLE_NAME, UTF8_STRING/8
  *
  * If the Window Manager displays a window name other than _NET_WM_NAME the
  * Window Manager MUST set this to the title displayed in UTF-8 encoding.
  *
  * Rationale: This property is for Window Managers that display a title
  * different from the _NET_WM_NAME or WM_NAME of the window (i.e. xterm <1>,
  * xterm <2>, ... is shown, but _NET_WM_NAME / WM_NAME is still xterm for each
  * window) thereby allowing Pagers to display the same title as the Window
  * Manager.
  *
  * @{ */
static void
Set_NET_WM_VISIBLE_NAME(Window window, char *name)
{
    if (name != NULL) {
	XTextProperty text = { NULL, };

	Xutf8TextListToTextProperty(dpy, &name, 1, XUTF8StringStyle, &text);
	XSetTextProperty(dpy, window, &text, _XA_NET_WM_VISIBLE_NAME);
	XFree(text.value);
    } else {
	XDeleteProperty(dpy, window, _XA_NET_WM_VISIBLE_NAME);
    }
}

void
Del_NET_WM_VISIBLE_NAME(TwmWindow *twin)
{
    XDeleteProperty(dpy, twin->w, _XA_NET_WM_VISIBLE_NAME);
    twin->ewmh.props._NET_WM_VISIBLE_NAME = 0;
    free(twin->ewmh.visible_name);
    twin->ewmh.visible_name = NULL;
}

/** @brief Update the visible name for a window.
  * @param twin - TWM window
  *
  * Should be called by TWM whenever the visible name changes (without a change
  * to the underlying _NET_WM_NAME or WM_NAME properties, which will be updated
  * automatically).  This function only changes the property when the visible
  * name actually changes.
  */
void
Upd_NET_WM_VISIBLE_NAME(TwmWindow *twin)
{
    char *visible_name = NULL;

    TwmGetWMVisibleName(twin, &visible_name);
    if (!twin->ewmh.props._NET_WM_VISIBLE_NAME
	|| strcmp_null(visible_name, twin->ewmh.visible_name) != 0) {
	Set_NET_WM_VISIBLE_NAME(twin->w, visible_name);
	twin->ewmh.props._NET_WM_VISIBLE_NAME = 1;
	free(twin->ewmh.visible_name);
	twin->ewmh.visible_name = visible_name;
    } else if (visible_name != NULL)
	free(visible_name);
}

/** @} */

/** @name _NET_WM_ICON_NAME, UTF8_STRING/8
  *
  * The Client SHOULD set this to the title of the icon for this window in UTF-8
  * encoding. If set, the Window Manager should use this in preference to
  * WM_ICON_NAME.
  *
  * @{ */
static void
Set_NET_WM_ICON_NAME(Window window, char *name)
{
    XTextProperty text = { NULL, };

    Xutf8TextListToTextProperty(dpy, &name, 1, XUTF8StringStyle, &text);
    XSetTextProperty(dpy, window, &text, _XA_NET_WM_ICON_NAME);
    XFree(text.value);
}

Bool
Get_NET_WM_ICON_NAME(Window window, char **name)
{
    XTextProperty text = { NULL, };
    int status, count;
    char **list = NULL;

    status = XGetTextProperty(dpy, window, &text, _XA_NET_WM_ICON_NAME);
    if (status == Success && text.nitems > 0) {
	XTextPropertyToStringList(&text, &list, &count);
	if (count > 0) {
	    free(*name);
	    *name = strdup(list[0]);
	}
	XFreeStringList(list);
	return True;
    }
    /* when no _NET_WM_ICON_NAME go after WM_ICON_NAME */
    status = XGetTextProperty(dpy, window, &text, XA_WM_ICON_NAME);
    if (status == Success && text.nitems > 0) {
	XTextPropertyToStringList(&text, &list, &count);
	if (count > 0) {
	    free(*name);
	    *name = strdup(list[0]);
	}
	XFreeStringList(list);
	return True;
    }
    return False;
}

static void
Ret_NET_WM_ICON_NAME(TwmWindow *twin)
{
    if (Get_NET_WM_ICON_NAME(twin->w, &twin->ewmh.icon_name))
	twin->ewmh.props._NET_WM_ICON_NAME = 1;
    else
	twin->ewmh.props._NET_WM_ICON_NAME = 0;
    TwmSetWMIconName(twin, twin->ewmh.icon_name);

    Upd_NET_WM_VISIBLE_ICON_NAME(twin);
}

/** @} */

/** @name _NET_WM_VISIBLE_ICON_NAME, UTF8_STRING/8
  *
  * If the Window Manager displays an icon name other than _NET_WM_ICON_NAME the
  * Window Manager MUST set this to the title displayed in UTF-8 encoding.
  *
  * @{ */
static void
Set_NET_WM_VISIBLE_ICON_NAME(Window window, char *name)
{
    if (name != NULL) {
	XTextProperty text = { NULL, };

	Xutf8TextListToTextProperty(dpy, &name, 1, XUTF8StringStyle, &text);
	XSetTextProperty(dpy, window, &text, _XA_NET_WM_VISIBLE_ICON_NAME);
	XFree(text.value);
    } else {
	XDeleteProperty(dpy, window, _XA_NET_WM_VISIBLE_ICON_NAME);
    }
}

void
Del_NET_WM_VISIBLE_ICON_NAME(TwmWindow *twin)
{
    XDeleteProperty(dpy, twin->w, _XA_NET_WM_VISIBLE_ICON_NAME);
    twin->ewmh.props._NET_WM_VISIBLE_ICON_NAME = 0;
    free(twin->ewmh.visible_icon_name);
    twin->ewmh.visible_icon_name = NULL;
}

/** @brief Update the visible icon name for a window.
  * @param twin - TWM window.
  *
  * Should be called by TWM whenever the visible icon name changes (without a
  * change to the underlying _NET_WM_ICON_NAME or WM_ICON_NAME properties, which
  * will be updated automatically).  Note that this function will only change
  * the property when the visible icon name actually changes.
  */
void
Upd_NET_WM_VISIBLE_ICON_NAME(TwmWindow *twin)
{
    char *visible_icon_name = NULL;

    TwmGetWMVisibleIconName(twin, &visible_icon_name);
    if (!twin->ewmh.props._NET_WM_VISIBLE_ICON_NAME
	|| strcmp_null(visible_icon_name, twin->ewmh.visible_icon_name) != 0) {
	Set_NET_WM_VISIBLE_ICON_NAME(twin->w, visible_icon_name);
	twin->ewmh.props._NET_WM_VISIBLE_ICON_NAME = 1;
	free(twin->ewmh.visible_icon_name);
	twin->ewmh.visible_icon_name = visible_icon_name;
    }
}

/** @} */

/** @name _NET_WM_DESKTOP desktop, CARDINAL/32
  *
  * Cardinal to determine the desktop the window is in (or wants to be) starting
  * with 0 for the first desktop. A Client MAY choose not to set this property,
  * in which case the Window Manager SHOULD place it as it wishes. 0xFFFFFFFF
  * indicates that the window SHOULD appear on all desktops.
  *
  * The Window Manager should honor _NET_WM_DESKTOP whenever a withdrawn window
  * requests to be mapped.
  *
  * The Window Manager should remove the property whenever a window is withdrawn
  * but it should leave the property in place when it is shutting down, e.g. in
  * response to losing ownership of the WM_Sn manager selection.
  *
  * Rationale: Removing the property upon window withdrawal helps legacy
  * applications which want to reuse withdrawn windows. Not removing the property
  * upon shutdown allows the next Window Manager to restore windows to their
  * previous desktops.
  *
  * @{ */

/** @brief Set the desktop for a window.
  * @param window - window to set
  * @param desktop - desktop to set
  */
static void
Set_NET_WM_DESKTOP(Window window, long desktop)
{
    XChangeProperty(dpy, window, _XA_NET_WM_DESKTOP, XA_CARDINAL, 32, PropModeReplace,
		    (unsigned char *) &desktop, 1);
}

/** @brief Get the desktop for a window.
  * @param window - window to get
  * @param desktop - where to save the desktop
  *
  * Returns True when the property was successfully retrieved.
  */
static Bool
Get_NET_WM_DESKTOP(Window window, int *desktop)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, window, _XA_NET_WM_DESKTOP, 0L, 1L, False, XA_CARDINAL,
			   &actual_type, &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    if (status != Success || actual_type == None) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    *desktop = *prop;
    XFree(prop);
    return True;
}

/** @brief Delete the desktop property from a window.
  * @param twin - TWM Window from which to delete
  */
void
Del_NET_WM_DESKTOP(TwmWindow *twin)
{
    XDeleteProperty(dpy, twin->w, _XA_NET_WM_DESKTOP);
    twin->ewmh.props._NET_WM_DESKTOP = 0;
    twin->ewmh.desktop = -2;
}

/** @brief Update the desktop for a window.
  * @param twin - TWM Window to update
  *
  * Should be called whenever the desktop for a window changes.  This function
  * only changes the property when the desktop for the window actually changes.
  */
void
Upd_NET_WM_DESKTOP(ScreenInfo *scr, TwmWindow *twin)
{
    int desktop = -2;

    TwmGetWMDesktop(scr, twin, &desktop);
    if (desktop != -2) {
	if (!twin->ewmh.props._NET_WM_DESKTOP || desktop != twin->ewmh.desktop) {
	    Set_NET_WM_DESKTOP(twin->w, desktop);
	    twin->ewmh.props._NET_WM_DESKTOP = 1;
	    twin->ewmh.desktop = desktop;
	}
    }
}

/** @brief Retreive the desktop for a window.
  * @param twin - TWM Window from which to retrieve
  *
  * Call this function when adding a window to read or write the current desktop
  * to the window.  When a startup notification sequence is associated with the
  * window that includes the DESKTOP= key field, we use that desktop as the
  * initial desktop when the property does not yet exist.  This is because, when
  * the application does not specify a desktop, the desktop specified by the
  * launcher applies.  The desktop specified by the launcher is normally the
  * desktop on which the application was launched.
  */
static void
Ret_NET_WM_DESKTOP(ScreenInfo *scr, TwmWindow *twin)
{
    int desktop = -2;

    if (Get_NET_WM_DESKTOP(twin->w, &desktop)) {
	TwmSetWMDesktop(scr, twin, desktop, 0);
	twin->ewmh.props._NET_WM_DESKTOP = 1;
	twin->ewmh.desktop = desktop;
    } else {
	EwmhSequence *seq;

	if ((seq = twin->ewmh.sequence) != NULL && seq->field.desktop != NULL
	    && (desktop = seq->numb.desktop) > -2)
	    TwmSetWMDesktop(scr, twin, desktop, 0);
	twin->ewmh.props._NET_WM_DESKTOP = 0;
	twin->ewmh.desktop = -2;
    }
    Upd_NET_WM_DESKTOP(scr, twin);
}

/** @brief Receive a client message changing the desktop for a window.
  *
  * A Client can request a change of desktop for a non-withdrawn window by
  * sending a _NET_WM_DESKTOP client message to the root window:
  *
  *  data.l[0] - desktop number (-1 for occupy all)
  *  data.l[1] - source
  *
  * See the section called "Source indication in requests" for details on the
  * source indication. The Window Manager MUST keep this property updated on all
  * windows.
  */
static void
Rcv_NET_WM_DESKTOP(ScreenInfo *scr, TwmWindow *twin, XClientMessageEvent *event)
{
    int desktop = event->data.l[0];
    enum _NET_SOURCE source = event->data.l[1];

    TwmSetWMDesktop(scr, twin, desktop, source);
    Upd_NET_WM_DESKTOP(scr, twin);
    Upd_NET_WM_DESKTOP_MASK(scr, twin);
    Upd_NET_WM_STATE(twin);	/* for sticky state */
}

/** @} */

/** @name _NET_WM_WINDOW_TYPE, ATOM[]/32
  *
  * This SHOULD be set by the Client before mapping to a list of atoms indicating
  * the functional type of the window. This property SHOULD be used by the window
  * manager in determining the decoration, stacking position and other behavior
  * of the window. The Client SHOULD specify window types in order of preference
  * (the first being most preferable) but MUST include at least one of the basic
  * window type atoms from the list below. This is to allow for extension of the
  * list of types whilst providing default behavior for Window Managers that do
  * not recognize the extensions.
  *
  * This hint SHOULD also be set for override-redirect windows to allow
  * compositing managers to apply consistent decorations to menus, tooltips etc.
  *
  * Rationale: This hint is intended to replace the MOTIF hints. One of the
  * objections to the MOTIF hints is that they are a purely visual description of
  * the window decoration. By describing the function of the window, the Window
  * Manager can apply consistent decoration and behavior to windows of the same
  * type. Possible examples of behavior include keeping dock/panels on top or
  * allowing pinnable menus / toolbars to only be hidden when another window has
  * focus (NextStep style).
  *
  * @{ */
static const Atom *NetWindowTypes[] = {
    NET_ATOM_ENTRY(_NET_WM_WINDOW_TYPE_DESKTOP),
    /* _NET_WM_WINDOW_TYPE_DESKTOP indicates a desktop feature. This can
       include a single window containing desktop icons with the same
       dimensions as the screen, allowing the desktop environment to have full
       control of the desktop, without the need for proxying root window
       clicks. */
    NET_ATOM_ENTRY(_NET_WM_WINDOW_TYPE_DOCK),
    /* _NET_WM_WINDOW_TYPE_DOCK indicates a dock or panel feature. Typically a
       Window Manager would keep such windows on top of all other windows. */
    NET_ATOM_ENTRY(_NET_WM_WINDOW_TYPE_TOOLBAR),
    NET_ATOM_ENTRY(_NET_WM_WINDOW_TYPE_MENU),
    /* _NET_WM_WINDOW_TYPE_TOOLBAR and _NET_WM_WINDOW_TYPE_MENU indicate
       toolbar and pinnable menu windows, respectively (i.e. toolbars and menus 
       "torn off" from the main application). Windows of this type may set the
       WM_TRANSIENT_FOR hint indicating the main application window.  Note that
       the _NET_WM_WINDOW_TYPE_MENU should be set on torn-off managed windows,
       where _NET_WM_WINDOW_TYPE_DROPDOWN_MENU and
       _NET_WM_WINDOW_TYPE_POPUP_MENU are typically used on override-redirect
       windows. */
    NET_ATOM_ENTRY(_NET_WM_WINDOW_TYPE_UTILITY),
    /* _NET_WM_WINDOW_TYPE_UTILITY indicates a small persistent utility window,
       such as a palette or toolbox. It is distinct from type TOOLBAR because it
       does not correspond to a toolbar torn off from the main application. It's
       distinct from type DIALOG because it isn't a transient dialog, the user
       will probably keep it open while they're working. Windows of this type may 
       set the WM_TRANSIENT_FOR hint indicating the main application window. */
    NET_ATOM_ENTRY(_NET_WM_WINDOW_TYPE_SPLASH),
    /* _NET_WM_WINDOW_TYPE_SPLASH indicates that the window is a splash screen
       displayed as an application is starting up. */
    NET_ATOM_ENTRY(_NET_WM_WINDOW_TYPE_DIALOG),
    /* _NET_WM_WINDOW_TYPE_DIALOG indicates that this is a dialog window. If
       _NET_WM_WINDOW_TYPE is not set, then managed windows with
       WM_TRANSIENT_FOR set MUST be taken as this type. Override-redirect
       windows with WM_TRANSIENT_FOR, but without _NET_WM_WINDOW_TYPE must be
       taken as _NET_WM_WINDOW_TYPE_NORMAL. */
    NET_ATOM_ENTRY(_NET_WM_WINDOW_TYPE_MENU),
    /* _NET_WM_WINDOW_TYPE_DROPDOWN_MENU indicates that the window in question
       is a dropdown menu, ie., the kind of menu that typically appears when
       the user clicks on a menubar, as opposed to a popup menu which typically
       appears when the user right-clicks on an object. This property is
       typically used on override-redirect windows. */
    NET_ATOM_ENTRY(_NET_WM_WINDOW_TYPE_POPUP_MENU),
    /* _NET_WM_WINDOW_TYPE_POPUP_MENU indicates that the window in question is
       a popup menu, ie., the kind of menu that typically appears when the user
       right clicks on an object, as opposed to a dropdown menu which typically
       appears when the user clicks on a menubar. This property is typically
       used on override-redirect windows. */
    NET_ATOM_ENTRY(_NET_WM_WINDOW_TYPE_TOOLTIP),
    /* _NET_WM_WINDOW_TYPE_TOOLTIP indicates that the window in question is a
       tooltip, ie., a short piece of explanatory text that typically appear
       after the mouse cursor hovers over an object for a while. This property
       is typically used on override-redirect windows. */
    NET_ATOM_ENTRY(_NET_WM_WINDOW_TYPE_NOTIFICATION),
    /* _NET_WM_WINDOW_TYPE_NOTIFICATION indicates a notification. An example of
       a notification would be a bubble appearing with informative text such as
       "Your laptop is running out of power" etc. This property is typically
       used on override-redirect windows. */
    NET_ATOM_ENTRY(_NET_WM_WINDOW_TYPE_COMBO),
    /* _NET_WM_WINDOW_TYPE_COMBO should be used on the windows that are popped
       up by combo boxes. An example is a window that appears below a text
       field with a list of suggested completions. This property is typically
       used on override-redirect windows. */
    NET_ATOM_ENTRY(_NET_WM_WINDOW_TYPE_DND),
    /* _NET_WM_WINDOW_TYPE_DND indicates that the window is being dragged.
       Clients should set this hint when the window in question contains a
       representation of an object being dragged from one place to another. An
       example would be a window containing an icon that is being dragged from
       one file manager window to another. This property is typically used on
       override-redirect windows. */
    NET_ATOM_ENTRY(_NET_WM_WINDOW_TYPE_NORMAL),
    /* _NET_WM_WINDOW_TYPE_NORMAL indicates that this is a normal, top-level
       window, either managed or override-redirect. Managed windows with
       neither _NET_WM_WINDOW_TYPE nor WM_TRANSIENT_FOR set MUST be taken as
       this type. Override-redirect windows without _NET_WM_WINDOW_TYPE, must
       be taken as this type, whether or not they have WM_TRANSIENT_FOR set. */
    [_NET_WM_WINDOW_TYPE_last] = NULL
};

static void
Set_NET_WM_WINDOW_TYPE(Window window, unsigned type)
{
    return Set_NET_WM_atoms(window, _XA_NET_WM_WINDOW_TYPE, NetWindowTypes, type);
}

static Bool
Get_NET_WM_WINDOW_TYPE(Window window, unsigned *type)
{
    return Get_NET_WM_atoms(window, _XA_NET_WM_WINDOW_TYPE, NetWindowTypes, type);
}

static void
Ret_NET_WM_WINDOW_TYPE(TwmWindow *twin)
{
    unsigned type = 0;

    if (Get_NET_WM_WINDOW_TYPE(twin->w, &type)) {
	twin->ewmh.props._NET_WM_WINDOW_TYPE = 1;
	TwmSetWMWindowType(twin, type);
	twin->ewmh.type = type;
    } else {
	twin->ewmh.props._NET_WM_WINDOW_TYPE = 0;
	twin->ewmh.type = 0;
    }
}

/** @} */

/** @name _NET_WM_STATE, ATOM[]
  *
  * A list of hints describing the window state. Atoms present in the list MUST
  * be considered set, atoms not present in the list MUST be considered not set.
  * The Window Manager SHOULD honor _NET_WM_STATE whenever a withdrawn window
  * requests to be mapped. A Client wishing to change the state of a window MUST
  * send a _NET_WM_STATE client message to the root window (see below). The
  * Window Manager MUST keep this property updated to reflect the current state
  * of the window.
  *
  * The Window Manager should remove the property whenever a window is withdrawn,
  * but it should leave the property in place when it is shutting down, e.g. in
  * response to losing ownership of the WM_Sn manager selection.
  *
  * Rationale: Removing the property upon window withdrawal helps legacy
  * applications which want to reuse withdrawn windows. Not removing the property
  * upon shutdown allows the next Window Manager to restore windows to their
  * previous state.
  *
  * @{ */
static const Atom *NetStates[] = {
    NET_ATOM_ENTRY(_NET_WM_STATE_MODAL),
    /* _NET_WM_STATE_MODAL indicates that this is a modal dialog box. If the
       WM_TRANSIENT_FOR hint is set to another toplevel window, the dialog is
       modal for that window; if WM_TRANSIENT_FOR is not set or set to the root
       window the dialog is modal for its window group. */
    NET_ATOM_ENTRY(_NET_WM_STATE_STICKY),
    /* _NET_WM_STATE_STICKY indicates that the Window Manager SHOULD keep the
       window's position fixed on the screen, even when the virtual desktop
       scrolls. */
    NET_ATOM_ENTRY(_NET_WM_STATE_MAXIMIZED_HORZ),
    NET_ATOM_ENTRY(_NET_WM_STATE_MAXIMIZED_VERT),
    /* _NET_WM_STATE_MAXIMIZED_{VERT,HORZ} indicates that the window is
       {vertically,horizontally} maximized. */
    NET_ATOM_ENTRY(_NET_WM_STATE_SHADED),
    /* _NET_WM_STATE_SHADED indicates that the window is shaded. */
    NET_ATOM_ENTRY(_NET_WM_STATE_SKIP_TASKBAR),
    /* _NET_WM_STATE_SKIP_TASKBAR indicates that the window should not be
       included on a taskbar. This hint should be requested by the application,
       i.e. it indicates that the window by nature is never in the taskbar.
       Applications should not set this hint if _NET_WM_WINDOW_TYPE already
       conveys the exact nature of the window. */
    NET_ATOM_ENTRY(_NET_WM_STATE_SKIP_PAGER),
    /* _NET_WM_STATE_SKIP_PAGER indicates that the window should not be
       included on a Pager. This hint should be requested by the application,
       i.e. it indicates that the window by nature is never in the Pager.
       Applications should not set this hint if _NET_WM_WINDOW_TYPE already
       conveys the exact nature of the window. */
    NET_ATOM_ENTRY(_NET_WM_STATE_HIDDEN),
    /* _NET_WM_STATE_HIDDEN should be set by the Window Manager to indicate
       that a window would not be visible on the screen if its desktop/viewport 
       were active and its coordinates were within the screen bounds. The
       canonical example is that minimized windows should be in the
       _NET_WM_STATE_HIDDEN state. Pagers and similar applications should use
       _NET_WM_STATE_HIDDEN instead of WM_STATE to decide whether to display a
       window in miniature representations of the windows on a desktop. */
    NET_ATOM_ENTRY(_NET_WM_STATE_FULLSCREEN),
    /* _NET_WM_STATE_FULLSCREEN indicates that the window should fill the
       entire screen and have no window decorations. Additionally the Window
       Manager is responsible for restoring the original geometry after a
       switch from fullscreen back to normal window. For example, a
       presentation program would use this hint. */
    NET_ATOM_ENTRY(_NET_WM_STATE_ABOVE),
    /* _NET_WM_STATE_ABOVE indicates that the window should be on top of most
       windows (see the section called "Stacking order" for details). */
    NET_ATOM_ENTRY(_NET_WM_STATE_BELOW),
    /* _NET_WM_STATE_BELOW indicates that the window should be below most
       windows (see the section called "Stacking order" for details). */
    /* _NET_WM_STATE_ABOVE and _NET_WM_STATE_BELOW are mainly meant for user
       preferences and should not be used by applications e.g. for drawing
       attention to their dialogs (the Urgency hint should be used in that
       case, see the section called "Urgency").' */
    NET_ATOM_ENTRY(_NET_WM_STATE_DEMANDS_ATTENTION),
    /* _NET_WM_STATE_DEMANDS_ATTENTION indicates that some action in or with
       the window happened. For example, it may be set by the Window Manager if 
       the window requested activation but the Window Manager refused it, or
       the application may set it if it finished some work. This state may be
       set by both the Client and the Window Manager. It should be unset by the 
       Window Manager when it decides the window got the required attention
       (usually, that it got activated). */
    NET_ATOM_ENTRY(_NET_WM_STATE_FOCUSED),
    /* _NET_WM_STATE_FOCUSED indicates whether the window's decorations are
       drawn in an active state. Clients MUST regard it as a read-only hint. It 
       cannot be set at map time or changed via a _NET_WM_STATE client message. 
       The window given by _NET_ACTIVE_WINDOW will usually have this hint, but
       at times other windows may as well, if they have a strong association
       with the active window and will be considered as a unit with it by the
       user. Clients that modify the appearance of internal elements when a
       toplevel has keyboard focus SHOULD check for the availability of this
       state in _NET_SUPPORTED and, if it is available, use it in preference to 
       tracking focus via FocusIn events. By doing so they will match the
       window decorations and accurately reflect the intentions of the Window
       Manager. */

    NET_ATOM_ENTRY(_NET_WM_STATE_DECOR_BORDER),
    /* 
     */
    NET_ATOM_ENTRY(_NET_WM_STATE_DECOR_HANDLE),
    /* 
     */
    NET_ATOM_ENTRY(_NET_WM_STATE_DECOR_TITLE),
    /* 
     */
    NET_ATOM_ENTRY(_NET_WM_STATE_DECOR),
    /* 
     */
    NET_ATOM_ENTRY(_NET_WM_STATE_PARENTRELATIVE_BACKGROUND),
    /* 
     */
    NET_ATOM_ENTRY(_NET_WM_STATE_STAYS_AT_BOTTOM),
    /* 
     */
    NET_ATOM_ENTRY(_NET_WM_STATE_STAYS_ON_TOP),
    /* 
     */

    NET_ATOM_ENTRY(_NET_WM_STATE_MAXIMUS_BOTTOM),
    /* 
     */
    NET_ATOM_ENTRY(_NET_WM_STATE_MAXIMUS_LEFT),
    /* 
     */
    NET_ATOM_ENTRY(_NET_WM_STATE_MAXIMUS_RIGHT),
    /* 
     */
    NET_ATOM_ENTRY(_NET_WM_STATE_MAXIMUS_TOP),
    /* 
     */
    NET_ATOM_ENTRY(_NET_WM_STATE_AUTORAISE),
    /* 
     */
    NET_ATOM_ENTRY(_NET_WM_STATE_AUTOLOWER),
    /* 
     */
    [_NET_WM_STATE_last] = NULL
};

/*
 * An implementation MAY add new atoms to this list. Implementations without
 * extensions MUST ignore any unknown atoms, effectively removing them from the
 * list. These extension atoms MUST NOT start with the prefix _NET.
 */

static void
Set_NET_WM_STATE(Window window, unsigned flags)
{
    return Set_NET_WM_atoms(window, _XA_NET_WM_STATE, NetStates, flags);
}

static Bool
Get_NET_WM_STATE(Window window, unsigned *flags)
{
    return Get_NET_WM_atoms(window, _XA_NET_WM_STATE, NetStates, flags);
}

/** @brief Update the window state.
  * @param twin - TWM window
  *
  * Should be called whenever the window state changes.  Note that this function
  * will not change the property unless the actual state changes.
  */
void
Upd_NET_WM_STATE(TwmWindow *twin)
{
    unsigned state = 0;

    TwmGetWMState(twin, &twin->ewmh.state);
    if (!twin->ewmh.props._NET_WM_STATE || twin->ewmh.state != state) {
	Set_NET_WM_STATE(twin->w, twin->ewmh.state);
	twin->ewmh.props._NET_WM_STATE = 1;
	twin->ewmh.state = state;
    }
}

/** @brief Retrieve the window state.
  * @param twin - TWM window
  *
  * Should be called when a window is intially mapped or restored.
  */
static void
Ret_NET_WM_STATE(TwmWindow *twin)
{
    unsigned state = 0;

    if (Get_NET_WM_STATE(twin->w, &state)) {
	TwmSetWMState(twin, state);
	twin->ewmh.props._NET_WM_STATE = 1;
	twin->ewmh.state = state;
    } else {
	twin->ewmh.props._NET_WM_STATE = 0;
	twin->ewmh.state = 0;
    }
    Upd_NET_WM_STATE(twin);
}

void
Del_NET_WM_STATE(TwmWindow *twin)
{
    XDeleteProperty(dpy, twin->w, _XA_NET_WM_STATE);
    twin->ewmh.props._NET_WM_STATE = 0;
    twin->ewmh.state = 0;
}

/** @brief Handle state change client requests for a window.
  * @param scr - screen
  * @param twin - TWM window
  * @param event - client message event
  *
  * To change the state of a mapped window, a Client MUST send a _NET_WM_STATE
  * client message to the root window.
  *
  * This message allows two properties to be changed simultaneously,
  * specifically to allow both horizontal and vertical maximization to be
  * altered together.  l[2] MUST be set to zero if only one property is to be
  * changed. See the section called "Source indication in requests" for details
  * on the source indication. l[0], the action, MUST be one of:
  */
static void
Rcv_NET_WM_STATE(ScreenInfo *scr, TwmWindow *twin, XClientMessageEvent *event)
{
    enum _NET_WM_STATE_ACTION action = (int) event->data.l[0];
    enum _NET_SOURCE source = (int) event->data.l[3];
    int bits, bits1, bits2;
    int action1, action2;

    action1 = atom_to_index(event->data.l[1], NetStates);
    action2 = atom_to_index(event->data.l[2], NetStates);
    bits1 = atoms_to_bits(&event->data.l[1], 1, NetStates);
    bits2 = atoms_to_bits(&event->data.l[2], 1, NetStates);
    if (!(bits = bits1 | bits2))
	return;
    TwmChgWMState(scr, twin, action1, action2, action, source);
    Upd_NET_WM_STATE(twin);
}

/** @} */

/** @name _NET_WM_ALLOWED_ACTIONS, ATOM[]
  *
  * A list of atoms indicating user operations that the Window Manager supports
  * for this window. Atoms present in the list indicate allowed actions, atoms
  * not present in the list indicate actions that are not supported for this
  * window.  The Window Manager MUST keep this property updated to reflect the
  * actions which are currently "active" or "sensitive" for a window. Taskbars,
  * Pagers, and other tools use _NET_WM_ALLOWED_ACTIONS to decide which actions
  * should be made available to the user.
  *
  * @{ */
static const Atom *NetAllowedActions[] = {
    NET_ATOM_ENTRY(_NET_WM_ACTION_MOVE),
    /* _NET_WM_ACTION_MOVE indicates that the window may be moved around the
       screen. */
    NET_ATOM_ENTRY(_NET_WM_ACTION_RESIZE),
    /* _NET_WM_ACTION_RESIZE indicates that the window may be resized.
       (Implementation note: Window Managers can identify a non-resizable
       window because its minimum and maximum size in WM_NORMAL_HINTS will be
       the same.) */
    NET_ATOM_ENTRY(_NET_WM_ACTION_MINIMIZE),
    /* _NET_WM_ACTION_MINIMIZE indicates that the window may be iconified. */
    NET_ATOM_ENTRY(_NET_WM_ACTION_SHADE),
    /* _NET_WM_ACTION_SHADE indicates that the window may be shaded. */
    NET_ATOM_ENTRY(_NET_WM_ACTION_STICK),
    /* _NET_WM_ACTION_STICK indicates that the window may have its sticky state
       toggled (as for _NET_WM_STATE_STICKY). Note that this state has to do
       with viewports, not desktops. */
    NET_ATOM_ENTRY(_NET_WM_ACTION_MAXIMIZE_HORZ),
    /* _NET_WM_ACTION_MAXIMIZE_HORZ indicates that the window may be maximized
       horizontally. */
    NET_ATOM_ENTRY(_NET_WM_ACTION_MAXIMIZE_VERT),
    /* _NET_WM_ACTION_MAXIMIZE_VERT indicates that the window may be maximized
       vertically. */
    NET_ATOM_ENTRY(_NET_WM_ACTION_FULLSCREEN),
    /* _NET_WM_ACTION_FULLSCREEN indicates that the window may be brought to
       fullscreen state. */
    NET_ATOM_ENTRY(_NET_WM_ACTION_CHANGE_DESKTOP),
    /* _NET_WM_ACTION_CHANGE_DESKTOP indicates that the window may be moved
       between desktops. */
    NET_ATOM_ENTRY(_NET_WM_ACTION_CLOSE),
    /* _NET_WM_ACTION_CLOSE indicates that the window may be closed (i.e. a
       _NET_CLOSE_WINDOW message may be sent). */
    NET_ATOM_ENTRY(_NET_WM_ACTION_ABOVE),
    /* _NET_WM_ACTION_ABOVE indicates that the window may placed in the "above"
       layer of windows (i.e. will respond to _NET_WM_STATE_ABOVE changes; see
       also the section called "Stacking order" for details). */
    NET_ATOM_ENTRY(_NET_WM_ACTION_BELOW),
    /* _NET_WM_ACTION_BELOW indicates that the window may placed in the "below"
       layer of windows (i.e. will respond to _NET_WM_STATE_BELOW changes; see
       also the section called "Stacking order" for details)). */
    [_NET_WM_ACTION_last] = NULL
};

/*
 * An implementation MAY add new atoms to this list. Implementations without
 * extensions MUST ignore any unknown atoms, effectively removing them from the
 * list. These extension atoms MUST NOT start with the prefix _NET.
 *
 * Note that the actions listed here are those that the Window Manager will
 * honor for this window. The operations must still be requested through the
 * normal mechanisms outlined in this specification. For example,
 * _NET_WM_ACTION_CLOSE does not mean that clients can send a WM_DELETE_WINDOW
 * message to this window; it means that clients can use a _NET_CLOSE_WINDOW
 * message to ask the Window Manager to do so.
 *
 * Window Managers SHOULD ignore the value of _NET_WM_ALLOWED_ACTIONS when they
 * initially manage a window. This value may be left over from a previous Window
 * Manager with different policies.
 */

static void
Set_NET_WM_ALLOWED_ACTIONS(Window window, unsigned flags)
{
    return Set_NET_WM_atoms(window, _XA_NET_WM_ALLOWED_ACTIONS, NetAllowedActions, flags);
}

static Bool
Get_NET_WM_ALLOWED_ACTIONS(Window window, unsigned *flags)
{
    return Get_NET_WM_atoms(window, _XA_NET_WM_ALLOWED_ACTIONS, NetAllowedActions, flags);
}

void
Del_NET_WM_ALLOWED_ACTIONS(TwmWindow *twin)
{
    XDeleteProperty(dpy, twin->w, _XA_NET_WM_ALLOWED_ACTIONS);
    twin->ewmh.props._NET_WM_ALLOWED_ACTIONS = 0;
    twin->ewmh.allowed = 0;
}

/** @brief Update the allowed actions for a window.
  * @param twin - TWM window
  *
  * Should be called whenver the allowed actions change.  Note that this
  * function will not change the property unless the actions actually change.
  */
void
Upd_NET_WM_ALLOWED_ACTIONS(TwmWindow *twin)
{
    unsigned allowed = 0;

    TwmGetWMAllowedActions(twin, &allowed);
    if (!twin->ewmh.props._NET_WM_ALLOWED_ACTIONS || twin->ewmh.allowed != allowed) {
	Set_NET_WM_ALLOWED_ACTIONS(twin->w, allowed);
	twin->ewmh.props._NET_WM_ALLOWED_ACTIONS = 1;
	twin->ewmh.allowed = allowed;
    }
}

/** @} */

/** @name _NET_WM_STRUT, left, right, top, bottom, CARDINAL[4]/32
  *
  * This property is equivalent to a _NET_WM_STRUT_PARTIAL property where all
  * start values are 0 and all end values are the height or width of the logical
  * screen. _NET_WM_STRUT_PARTIAL was introduced later than _NET_WM_STRUT,
  * however, so clients MAY set this property in addition to
  * _NET_WM_STRUT_PARTIAL to ensure backward compatibility with Window Managers
  * supporting older versions of the Specification.
  *
  * @{ */

static void
Set_NET_WM_STRUT(Window window, struct NetStrut *strut)
{
    XChangeProperty(dpy, window, _XA_NET_WM_STRUT, XA_CARDINAL, 32, PropModeReplace,
		    (unsigned char *) strut, 4);
}

static Bool
Get_NET_WM_STRUT(Window window, struct NetStrut *strut)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, window, _XA_NET_WM_STRUT, 0L, 4L, False, XA_CARDINAL,
			   &actual_type, &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    memset(strut, 0, sizeof(*strut));
    if (status != Success || actual_type == None || nitems < 4UL) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    *strut = *(struct NetStrut *) prop;
    XFree(prop);
    return True;
}

/** @} */

/** @name _NET_WM_STRUT_PARTIAL, left, right, top, bottom, left_start_y, left_end_y, right_start_y, right_end_y, top_start_x, top_end_x, bottom_start_x, bottom_end_x,CARDINAL[12]/32
  *
  * This property MUST be set by the Client if the window is to reserve space at
  * the edge of the screen. The property contains 4 cardinals specifying the
  * width of the reserved area at each border of the screen, and an additional 8
  * cardinals specifying the beginning and end corresponding to each of the four
  * struts. The order of the values is left, right, top, bottom, left_start_y,
  * left_end_y, right_start_y, right_end_y, top_start_x, top_end_x,
  * bottom_start_x, bottom_end_x. All coordinates are root window coordinates.
  * The client MAY change this property at any time, therefore the Window
  * Manager MUST watch for property notify events if the Window Manager uses
  * this property to assign special semantics to the window.
  *
  * If both this property and the _NET_WM_STRUT property are set, the Window
  * Manager MUST ignore the _NET_WM_STRUT property values and use instead the
  * values for _NET_WM_STRUT_PARTIAL. This will ensure that Clients can safely
  * set both properties without giving up the improved semantics of the new
  * property.
  *
  * The purpose of struts is to reserve space at the borders of the desktop.
  * This is very useful for a docking area, a taskbar or a panel, for instance.
  * The Window Manager should take this reserved area into account when
  * constraining window positions - maximized windows, for example, should not
  * cover that area.
  *
  * The start and end values associated with each strut allow areas to be
  * reserved which do not span the entire width or height of the screen.  Struts
  * MUST be specified in root window coordinates, that is, they are not relative
  * to the edges of any view port or Xinerama monitor.
  *
  * For example, for a panel-style Client appearing at the bottom of the screen,
  * 50 pixels tall, and occupying the space from 200-600 pixels from the left of
  * the screen edge would set a bottom strut of 50, and set bottom_start_x to
  * 200 and bottom_end_x to 600. Another example is a panel on a screen using
  * the Xinerama extension. Assume that the set up uses two monitors, one
  * running at 1280x1024 and the other to the right running at 1024x768, with
  * the top edge of the two physical displays aligned. If the panel wants to
  * fill the entire bottom edge of the smaller display with a panel 50 pixels
  * tall, it should set a bottom strut of 306, with bottom_start_x of 1280, and
  * bottom_end_x of 2303.  Note that the strut is relative to the screen edge,
  * and not the edge of the xinerama monitor.
  *
  * Rationale: A simple "do not cover" hint is not enough for dealing with e.g.
  * auto-hide panels.
  *
  * Notes: An auto-hide panel SHOULD set the strut to be its minimum, hidden
  * size. A "corner" panel that does not extend for the full length of a screen
  * border SHOULD only set one strut.
  *
  * @{ */
static void
Set_NET_WM_STRUT_PARTIAL(Window window, struct NetStrut *strut)
{
    XChangeProperty(dpy, window, _XA_NET_WM_STRUT_PARTIAL, XA_CARDINAL, 32,
		    PropModeReplace, (unsigned char *) strut, 12);
}

static Bool
Get_NET_WM_STRUT_PARTIAL(Window window, struct NetStrut *strut)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, window, _XA_NET_WM_STRUT_PARTIAL, 0L, 12L, False,
			   XA_CARDINAL, &actual_type, &actual_format, &nitems,
			   &bytes_after, (unsigned char **) &prop);
    memset(strut, 0, sizeof(*strut));
    if (status != Success || actual_type == None || nitems < 12UL) {
	if (prop != NULL)
	    XFree(prop);
	return False;
	return Get_NET_WM_STRUT(window, strut);
    }
    *strut = *(struct NetStrut *) prop;
    XFree(prop);
    return True;
}

static void
Ret_NET_WM_STRUT_PARTIAL(ScreenInfo *scr, TwmWindow *twin)
{
    struct NetStrut strut = { 0, };

    if (Get_NET_WM_STRUT_PARTIAL(twin->w, &strut)) {
	twin->ewmh.props._NET_WM_STRUT_PARTIAL = 1;
	twin->ewmh.strut = strut;
    } else if (Get_NET_WM_STRUT(twin->w, &strut)) {
	twin->ewmh.props._NET_WM_STRUT = 1;
	twin->ewmh.strut = strut;
    } else {
	twin->ewmh.props._NET_WM_STRUT_PARTIAL = 0;
	twin->ewmh.props._NET_WM_STRUT = 0;
	twin->ewmh.strut = strut;
    }
    Upd_NET_WORKAREA(scr);
}

/** @} */

/** @name _NET_WM_ICON_GEOMETRY, x, y, width, height, CARDINAL[4]/32
  *
  * This optional property MAY be set by stand alone tools like a taskbar or an
  * iconbox. It specifies the geometry of a possible icon in case the window is
  * iconified.
  *
  * Rationale: This makes it possible for a Window Manager to display a nice
  * animation like morphing the window into its icon.
  *
  * @{ */
static void
Set_NET_WM_ICON_GEOMETRY(Window window, struct NetGeometry *geom)
{
    XChangeProperty(dpy, window, _XA_NET_WM_ICON_GEOMETRY, XA_CARDINAL, 32,
		    PropModeReplace, (unsigned char *) geom, 4);
}

static Bool
Get_NET_WM_ICON_GEOMETRY(Window window, struct NetGeometry *geom)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, window, _XA_NET_WM_ICON_GEOMETRY, 0L, 4L, False,
			   XA_CARDINAL, &actual_type, &actual_format, &nitems,
			   &bytes_after, (unsigned char **) &prop);
    memset(geom, 0, sizeof(*geom));
    if (status != Success || actual_type == None || nitems < 4UL) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    *geom = *(struct NetGeometry *) prop;
    XFree(prop);
    return True;
}

/*
 * Should be called directly when first managing a window.  Otherwise, property
 * changes will update automagically.  TWM is not notified when this property
 * changes.  When performing zoom animations, TWM should check the presence of
 * this property and use it as the destination of the animation when present.
 */
void
Ret_NET_WM_ICON_GEOMETRY(TwmWindow *twin)
{
    if (Get_NET_WM_ICON_GEOMETRY(twin->w, &twin->ewmh.icon_geometry))
	twin->ewmh.props._NET_WM_ICON_GEOMETRY = 1;
    else
	twin->ewmh.props._NET_WM_ICON_GEOMETRY = 0;
}

/** @} */

/** @name _NET_WM_ICON CARDINAL[][2+n]/32
  *
  * This is an array of possible icons for the client. This specification does
  * not stipulate what size these icons should be, but individual desktop
  * environments or toolkits may do so. The Window Manager MAY scale any of these
  * icons to an appropriate size.
  *
  * This is an array of 32bit packed CARDINAL ARGB with high byte being A, low
  * byte being B. The first two cardinals are width, height. Data is in rows,
  * left to right and top to bottom.
  *
  * @{ */
static Bool
Get_NET_WM_ICON(Window window, struct NetIcon *icon)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0, n;
    long *prop = NULL;

    free(icon->data);
    icon->data = NULL;
    icon->length = 0;
    status =
	XGetWindowProperty(dpy, window, _XA_NET_WM_ICON, 0L, 1L, False, XA_CARDINAL,
			   &actual_type, &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 1UL || bytes_after <= 0) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    XFree(prop);
    prop = NULL;
    status =
	XGetWindowProperty(dpy, window, _XA_NET_WM_ICON, 0L, ((bytes_after + 3) >> 2) + 1,
			   False, XA_CARDINAL, &actual_type, &actual_format, &nitems,
			   &bytes_after, (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 2UL || bytes_after > 0) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    if ((icon->data = calloc(nitems, sizeof(*icon->data))) == NULL) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    for (n = 0; n < nitems; n++)
	icon->data[n] = prop[n];
    XFree(prop);
    return True;
}

/** @brief Reterieve the icon for a window.
  * @param twin - TWM window
  *
  * Should be called once when a window is first managed.  It will be updated
  * automatically after that.  When displaying an icon, TWM should check for the
  * presence of this property and use this icon.  Note that if an ICON= field
  * exists is an associated startup notification, we can do an XDG lookup of the
  * icon.
  */
static void
Ret_NET_WM_ICON(TwmWindow *twin)
{
    if (Get_NET_WM_ICON(twin->w, &twin->ewmh.icon))
	twin->ewmh.props._NET_WM_ICON = 1;
    else
	twin->ewmh.props._NET_WM_ICON = 0;
}

/** @} */

/** @name _NET_WM_PID CARDINAL/32
  *
  * If set, this property MUST contain the process ID of the client owning this
  * window. This MAY be used by the Window Manager to kill windows which do not
  * respond to the _NET_WM_PING protocol.
  *
  * If _NET_WM_PID is set, the ICCCM-specified property WM_CLIENT_MACHINE MUST
  * also be set. While the ICCCM only requests that WM_CLIENT_MACHINE is set " to
  * a string that forms the name of the machine running the client as seen from
  * the machine running the server" conformance to this specification requires
  * that WM_CLIENT_MACHINE be set to the fully-qualified domain name of the
  * client's host.
  *
  * See also the implementation notes on killing hung processes.
  *
  * @{ */
static void
Set_NET_WM_PID(Window window, pid_t pid)
{
    long data = (long) pid;

    XChangeProperty(dpy, window, _XA_NET_WM_PID, XA_CARDINAL, 32, PropModeReplace,
		    (unsigned char *) &data, 1);
}

static Bool
Get_NET_WM_PID(Window window, pid_t *pid)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, window, _XA_NET_WM_PID, 0L, 1L, False, XA_CARDINAL,
			   &actual_type, &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 1UL) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    *pid = *prop;
    XFree(prop);
    return True;
}

/** @brief Update the client pid for a window.
  * @param twin - TWM window
  *
  * Should be called when mapping a window and a startup notification sequence
  * has been assigned (after calling Ret_NET_WM_PID()).  When there is a startup
  * notification sequence associated with the window with a PID= field, we set
  * that when there is no property.
  */
void
Upd_NET_WM_PID(TwmWindow *twin)
{
    pid_t pid = 0;
    EwmhSequence *seq;

    if (!twin->ewmh.props._NET_WM_PID && (seq = twin->ewmh.sequence) != NULL
	&& seq->field.pid != NULL && (pid = seq->numb.pid) != 0) {
	Set_NET_WM_PID(twin->w, pid);
	twin->ewmh.props._NET_WM_PID = 1;
	twin->ewmh.pid = pid;
    }
}

/** @brief Retreive the pid for a window.
  * @param twin - TWM window
  *
  * This function should be called when a window is first managed.
  */
static void
Ret_NET_WM_PID(ScreenInfo *scr, TwmWindow *twin)
{
    pid_t pid = 0;

    if (Get_NET_WM_PID(twin->w, &pid)) {
	TwmSetWMPid(scr, twin, pid);
	twin->ewmh.props._NET_WM_PID = 1;
	twin->ewmh.pid = pid;
    } else {
	twin->ewmh.props._NET_WM_PID = 0;
	twin->ewmh.pid = pid;
	Upd_NET_WM_PID(twin);
    }
}

/** @} */

/** @name _NET_WM_HANDLED_ICONS
  *
  * This property can be set by a Pager on one of its own toplevel windows to
  * indicate that the Window Manager need not provide icons for iconified
  * windows, for example if it is a taskbar and provides buttons for iconified
  * windows.
  *
  * @{ */
static Bool
Get_NET_WM_HANDLED_ICONS(Window window)
{
    Atom actual_type = 0;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, window, _XA_NET_WM_HANDLED_ICONS, 0L, 1L, False, None,
			   &actual_type, &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    if (status != Success || actual_format == 0) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    XFree(prop);
    return True;
}

static void
Ret_NET_WM_HANDLED_ICONS(ScreenInfo *scr, TwmWindow *twin)
{
    if (Get_NET_WM_HANDLED_ICONS(twin->w))
	twin->ewmh.props._NET_WM_HANDLED_ICONS = 1;
    else
	twin->ewmh.props._NET_WM_HANDLED_ICONS = 0;
    TwmUpdWMHandledIcons(scr);
}

/** @} */

/** @name _NET_WM_USER_TIME_WINDOW WINDOW/32
  *
  * This property contains the XID of a window on which the client sets the
  * _NET_WM_USER_TIME property. Clients should check whether the window manager
  * supports _NET_WM_USER_TIME_WINDOW and fall back to setting the
  * _NET_WM_USER_TIME property on the toplevel window if it doesn't.
  *
  * Rationale: Storing the frequently changing _NET_WM_USER_TIME property on the
  * toplevel window itself causes every application that is interested in any of
  * the properties of that window to be woken up on every keypress, which is
  * particularly bad for laptops running on battery power.
  *
  * @{ */
static void
Set_NET_WM_USER_TIME_WINDOW(Window window, Window time_window)
{
    long data = time_window;

    XChangeProperty(dpy, window, _XA_NET_WM_USER_TIME_WINDOW, XA_CARDINAL, 32,
		    PropModeReplace, (unsigned char *) &data, 1);
}

static Bool
Get_NET_WM_USER_TIME_WINDOW(Window window, Window *time_window)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;
    Window check;

    status =
	XGetWindowProperty(dpy, window, _XA_NET_WM_USER_TIME_WINDOW, 0L, 1L, False,
			   XA_WINDOW, &actual_type, &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 1) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    check = prop[0];
    XFree(prop);
    prop = NULL;
    status =
	XGetWindowProperty(dpy, check, _XA_NET_WM_USER_TIME_WINDOW, 0L, 1L, False,
			   XA_WINDOW, &actual_type, &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 1) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    if (check != (Window) prop[0]) {
	XFree(prop);
	return False;
    }
    *time_window = prop[0];
    XFree(prop);
    return True;
}

static void
Ret_NET_WM_USER_TIME_WINDOW(ScreenInfo *scr, TwmWindow *twin)
{
    Window time_window = None;

    if (Get_NET_WM_USER_TIME_WINDOW(twin->w, &time_window)) {
	twin->ewmh.props._NET_WM_USER_TIME_WINDOW = 1;
	TwmSetWMUserTimeWindow(scr, twin, time_window);
	twin->ewmh.user_time_window = time_window;
    } else {
	twin->ewmh.props._NET_WM_USER_TIME_WINDOW = 0;
	twin->ewmh.user_time_window = None;
    }
}

static void
Del_NET_WM_USER_TIME_WINDOW(ScreenInfo *scr, TwmWindow *twin)
{
    if (twin->ewmh.props._NET_WM_USER_TIME_WINDOW) {
	Window time_window;

	time_window = twin->ewmh.user_time_window;
	TwmDelWMUserTimeWindow(scr, twin, time_window);
	twin->ewmh.props._NET_WM_USER_TIME_WINDOW = 0;
	twin->ewmh.user_time_window = None;
    }
}

/** @} */

/** @name _NET_WM_USER_TIME CARDINAL/32
  *
  * This property contains the XServer time at which last user activity in this
  * window took place.
  *
  * Clients should set this property on every new toplevel window (or on the
  * window pointed out by the _NET_WM_USER_TIME_WINDOW property), before mapping
  * the window, to the timestamp of the user interaction that caused the window
  * to appear. A client that only deals with core events, might, for example, use
  * the timestamp of the last KeyPress or ButtonPress event. ButtonRelease and
  * KeyRelease events should not generally be considered to be user interaction,
  * because an application may receive KeyRelease events from global keybindings,
  * and generally release events may have later timestamp than actions that were
  * triggered by the matching press events. Clients can obtain the timestamp that
  * caused its first window to appear from the DESKTOP_STARTUP_ID environment
  * variable, if the app was launched with startup notification. If the client
  * does not know the timestamp of the user interaction that caused the first
  * window to appear (e.g. because it was not launched with startup
  * notification), then it should not set the property for that window. The
  * special value of zero on a newly mapped window can be used to request that
  * the window not be initially focused when it is mapped.
  *
  * If the client has the active window, it should also update this property on
  * the window whenever there's user activity.
  *
  * Rationale: This property allows a Window Manager to alter the focus,
  * stacking, and/or placement behavior of windows when they are mapped depending
  * on whether the new window was created by a user action or is a "pop-up"
  * window activated by a timer or some other event.
  *
  * @{ */

/** @brief Set the user time.
  * @param window - window on which to set
  * @param time - time to set
  */
static void
Set_NET_WM_USER_TIME(Window window, Time time)
{
    long data = time;

    XChangeProperty(dpy, window, _XA_NET_WM_USER_TIME, XA_CARDINAL, 32, PropModeReplace,
		    (unsigned char *) &data, 1);
}

/** @brief Get the user time.
  * @param window - window from which to fetch
  * @param time - where to store the time
  */
static Bool
Get_NET_WM_USER_TIME(Window window, Time *time)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, window, _XA_NET_WM_USER_TIME, 0L, 1L, False, XA_CARDINAL,
			   &actual_type, &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 1) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    *time = prop[0];
    XFree(prop);
    return True;
}

/** @brief update the user time for a window.
  * @param twin - TWM window
  *
  * Should be called when mapping a window and a startup notification sequence
  * has been assigned (after calling Ret_NET_WM_USER_TIME()).  When there is a
  * startup notification sequence associated with the window with a TIMESTAMP=
  * field (or a timestamp embedded in the ID= field), we set that when there is
  * no property.
  */
void
Upd_NET_WM_USER_TIME(TwmWindow *twin)
{
    Time time = CurrentTime;
    EwmhSequence *seq;

    if (!twin->ewmh.props._NET_WM_USER_TIME && (seq = twin->ewmh.sequence) != NULL
	&& seq->field.timestamp != NULL
	&& (time = (Time) seq->numb.timestamp) != CurrentTime) {
	Window time_window = twin->w;

	if (twin->ewmh.props._NET_WM_USER_TIME_WINDOW
	    && twin->ewmh.user_time_window != None)
	    time_window = twin->ewmh.user_time_window;
	TwmSetWMUserTime(time);
	Set_NET_WM_USER_TIME(time_window, time);
	twin->ewmh.props._NET_WM_USER_TIME = 1;
	twin->ewmh.user_time = time;
    }
}

/** @brief Retrieve the user time.
  * @param twin - TWM window
  *
  * This function should be called when a window is first managed.  Note that
  * wehn a user time window has been established, this function uses that window
  * instead of the one associated with twin.
  */
static void
Ret_NET_WM_USER_TIME(TwmWindow *twin)
{
    Time time = CurrentTime;
    Window time_window = twin->w;

    if (twin->ewmh.props._NET_WM_USER_TIME_WINDOW && twin->ewmh.user_time_window != None)
	time_window = twin->ewmh.user_time_window;

    if (Get_NET_WM_USER_TIME(time_window, &time)) {
	TwmSetWMUserTime(time);
	twin->ewmh.props._NET_WM_USER_TIME = 1;
	twin->ewmh.user_time = time;
    } else {
	twin->ewmh.props._NET_WM_USER_TIME = 0;
	twin->ewmh.user_time = CurrentTime;
    }
    Upd_NET_WM_USER_TIME(twin);
}

/** @} */

/** @name _NET_FRAME_EXTENTS, left, right, top, bottom, CARDINAL[4]/32
  *
  * The Window Manager MUST set _NET_FRAME_EXTENTS to the extents of the window's
  * frame. left, right, top and bottom are widths of the respective borders added
  * by the Window Manager.
  *
  * @{ */
static void
Set_NET_FRAME_EXTENTS(Window window, struct NetExtents *extents)
{
    XChangeProperty(dpy, window, _XA_NET_FRAME_EXTENTS, XA_CARDINAL, 32, PropModeReplace,
		    (unsigned char *) extents, 4);
    XChangeProperty(dpy, window, _XA_KDE_NET_WM_FRAME_STRUT, XA_CARDINAL, 32,
		    PropModeReplace, (unsigned char *) extents, 4);
}

/** @brief Update the frame extents for a window.
  * @param twin - TWM window
  *
  * Should be called whenever the borders and title for a window changes.  Note
  * that this function will only set the property when the extents actually
  * change.
  */
void
Upd_NET_FRAME_EXTENTS(TwmWindow *twin)
{
    struct NetExtents extents = { 0, 0, 0, 0 };

    TwmGetWMFrameExtents(twin, &extents);
    if (!twin->ewmh.props._NET_FRAME_EXTENTS
	|| cmp_extents(&twin->ewmh.extents, &extents) != 0) {
	Set_NET_FRAME_EXTENTS(twin->w, &extents);
	twin->ewmh.props._NET_FRAME_EXTENTS = 1;
	twin->ewmh.props._KDE_NET_WM_FRAME_STRUT = 1;
	twin->ewmh.extents = extents;
    }
}

void
Del_NET_FRAME_EXTENTS(TwmWindow *twin)
{
    XDeleteProperty(dpy, twin->w, _XA_NET_FRAME_EXTENTS);
    XDeleteProperty(dpy, twin->w, _XA_KDE_NET_WM_FRAME_STRUT);
}

/** @} */

/** @name _NET_WM_OPAQUE_REGION, x, y, width, height, CARDINAL[][4]/32
  *
  * The Client MAY set this property to a list of 4-tuples [x, y, width, height],
  * each representing a rectangle in window coordinates where the pixels of the
  * window's contents have a fully opaque alpha value. If the window is drawn by
  * the compositor without adding any transparency, then such a rectangle will
  * occlude whatever is drawn behind it. When the window has an RGB visual rather
  * than an ARGB visual, this property is not typically useful, since the
  * effective opaque region of a window is exactly the bounding region of the
  * window as set via the shape extension. For windows with an ARGB visual and
  * also a bounding region set via the shape extension, the effective opaque
  * region is given by the intersection of the region set by this property and
  * the bounding region set via the shape extension. The compositing manager MAY
  * ignore this hint.
  *
  * Rationale: This gives the compositing manager more room for optimizations.
  * For example, it can avoid drawing occluded portions behind the window.
  *
  * @{ */
static void
Set_NET_WM_OPAQUE_REGION(Window window)
{
}

/** @} */

/** @name _NET_WM_BYPASS_COMPOSITOR, CARDINAL/32
  *
  * The Client MAY set this property to hint the compositor that the window would
  * benefit from running uncomposited (i.e not redirected offscreen) or that the
  * window might be hurt from being uncomposited. A value of 0 indicates no
  * preference. A value of 1 hints the compositor to disabling compositing of
  * this window. A value of 2 hints the compositor to not disabling compositing
  * of this window. All other values are reserved and should be treated the same
  * as a value of 0. The compositing manager MAY bypass compositing for both
  * fullscreen and non-fullscreen windows if bypassing is requested, but MUST NOT
  * bypass if it would cause differences from the composited appearance.
  *
  * Rationale: Some applications like fullscreen games might want run without the
  * overhead of being redirected offscreen (to avoid extra copies) and thus
  * perform better. An application which creates pop-up windows might always want
  * to run composited to avoid exposes.
  *
  * @{ */
static void
Set_NET_WM_BYPASS_COMPOSITOR(Window window, enum _NET_COMPOSITOR_HINT hint)
{
    long data = (long) hint;

    XChangeProperty(dpy, window, _XA_NET_WM_BYPASS_COMPOSITOR, XA_CARDINAL, 32,
		    PropModeReplace, (unsigned char *) &data, 1);
}

/** @} */

/** @} */

/** @name Other Root Window Messages.
  *
  * @{ */

/** @name _NET_CLOSE_WINDOW
  *
  * Pagers wanting to close a window MUST send a _NET_CLOSE_WINDOW client message
  * request to the root window.
  *
  * The Window Manager MUST then attempt to close the window specified.  See the
  * section called "Source indication in requests" for details on the source
  * indication.
  *
  * Rationale: A Window Manager might be more clever than the usual method (send
  * WM_DELETE message if the protocol is selected, XKillClient otherwise).  It
  * might introduce a timeout, for example. Instead of duplicating the code, the
  * Window Manager can easily do the job.
  *
  * @{ */
static void
Rcv_NET_CLOSE_WINDOW(TwmWindow *twin, XClientMessageEvent *event)
{
    Time timestamp = (Time) event->data.l[0];
    enum _NET_SOURCE source = (int) event->data.l[1];

    switch (source) {
    case _NET_SOURCE_UNSPECIFIED:
    case _NET_SOURCE_APPLICATION:
    case _NET_SOURCE_PAGER:
	break;
    default:
#ifdef DEBUG_EWMH
	fprintf(stderr, "Received _NET_CLOSE_WINDOW message with invalid source %d\n", source);
	fflush(stderr);
#endif
	return;
    }
    TwmCloseWindow(twin, timestamp, source);
}

/* _NET_MOVERESIZE_WINDOW
 *
 * The low byte of data.l[0] contains the gravity to use; it may contain any
 * value allowed for the WM_SIZE_HINTS.win_gravity property: NorthWest (1),
 * North (2), NorthEast (3), West (4), Center (5), East (6), SouthWest (7),
 * South (8), SouthEast (9) and Static (10).  A gravity of 0 indicates that the
 * Window Manager should use the gravity specified in WM_SIZE_HINTS.win_gravity.
 * The bits 8 to 11 indicate the presence of x, y, width and height.  The bits
 * 12 to 15 indicate the source (see the section called "Source indication in
 * requests"), so 0001 indicates the application and 0010 indicates a Pager or a
 * Taskbar.  The remaining bits should be set to zero.
 *
 * Pagers wanting to move or resize a window may send a _NET_MOVERESIZE_WINDOW
 * client message request to the root window instead of using a
 * ConfigureRequest.
 *
 * Window Managers should treat a _NET_MOVERESIZE_WINDOW message exactly like a
 * ConfigureRequest (in particular, adhering to the ICCCM rules about synthetic
 * ConfigureNotify events), except that they should use the gravity specified in
 * the message.
 *
 * Rationale: Using a _NET_MOVERESIZE_WINDOW message with StaticGravity allows
 * Pagers to exactly position and resize a window including its decorations
 * without knowing the size of the decorations.
 */
static void
Rcv_NET_MOVERESIZE_WINDOW(TwmWindow *twin, XClientMessageEvent *event)
{
    unsigned mask = 0;
    XWindowChanges changes = { 0, };
    unsigned flags = (unsigned) event->data.l[0];
    unsigned gravity = flags & 0xff;
    enum _NET_SOURCE source = (int) ((flags >> 12) & 0x0f);

    if (flags & (1 << 8)) {
	mask |= CWX;
	changes.x = event->data.l[1];
    }
    if (flags & (1 << 9)) {
	mask |= CWY;
	changes.y = event->data.l[2];
    }
    if (flags & (1 << 10)) {
	mask |= CWWidth;
	changes.width = event->data.l[3];
    }
    if (flags & (1 << 11)) {
	mask |= CWHeight;
	changes.height = event->data.l[4];
    }

    switch (source) {
    case _NET_SOURCE_UNSPECIFIED:
	/* might be using oder version of specification */
    case _NET_SOURCE_PAGER:
	break;
    case _NET_SOURCE_APPLICATION:
#ifdef DEBUG_EWMH
	fprintf(stderr, "Applications should use XConfigureWindow instead of _NET_MOVERESIZE_WINDOW\n");
	fflush(stderr);
#endif
	break;
    default:
#ifdef DEBUG_EWMH
	fprintf(stderr, "Received _NET_MOVERESIZE_WINDOW message with invalid source %d\n", source);
	fflush(stderr);
#endif
	return;
    }
    switch (gravity) {
    case 0:			/* use WM_SIZE_HINTS.win_gravity */
    case 1:			/* NorthWest */
    case 2:			/* North */
    case 3:			/* NorthEast */
    case 4:			/* West */
    case 5:			/* Center */
    case 6:			/* East */
    case 7:			/* SouthWest */
    case 8:			/* South */
    case 9:			/* SouthEast */
    case 10:			/* Static */
	break;
    default:
	return;
    }
    TwmMoveResizeWindow(twin, gravity, mask, &changes, source);
}

/** @} */

/** @name _NET_WM_MOVERESIZE
  *
  * This message allows Clients to initiate window movement or resizing.  They
  * can define their own move and size "grips", whilst letting the Window Manager
  * control the actual operation.  This means that all moves/resizes can happen
  * in a consistent manner as defined by the Window Manager.  See the section
  * called "Source indication in requests" for details on the source indication.
  *
  * When sending this message in response to a button press event, button SHOULD
  * indicate the button which was pressed, x_root and y_root MUST indicate the
  * position of the button press with respect to the root window and direction
  * MUST indicate whether this is a move or resize event, and if it is a resize
  * event, which edges of the window the size grip applies to.  When sending this
  * message in response to a key event, the direction MUST indicate whether this
  * this is a move or resize event and the other fields are unused.
  *
  * The Client MUST release all grabs prior to sending such message (except for
  * the _NET_WM_MOVERESIZE_CANCEL message).
  *
  * The Window Manager can use the button field to determine the events on which
  * it terminates the operation initiated by the _NET_WM_MOVERESIZE message.
  * Since there is a race condition between a client sending the
  * _NET_WM_MOVERESIZE message and the user releasing the button, Window Managers
  * are advised to offer some other means to terminate the operation, e.g. by
  * pressing the ESC key.  The special value _NET_WM_MOVERESIZE_CANCEL also
  * allows clients to cancel the operation by sending such message if they detect
  * the release themselves (clients should send it if they get the button release
  * after sending the move resize message, indicating that the WM did not get a
  * grab in time to get the release).
  *
  * @{ */
static void
Rcv_NET_WM_MOVERESIZE(TwmWindow *twin, XClientMessageEvent *event)
{
    int x_root = (int) event->data.l[0];
    int y_root = (int) event->data.l[1];
    enum _NET_WM_MOVERESIZE direction = (int) event->data.l[2];
    int button = (int) event->data.l[3];
    enum _NET_SOURCE source = (int) event->data.l[4];

    switch (source) {
    case _NET_SOURCE_UNSPECIFIED:
    case _NET_SOURCE_APPLICATION:
    case _NET_SOURCE_PAGER:
	break;
    default:
#ifdef DEBUG_EWMH
	fprintf(stderr, "Received _NET_WM_MOVERESIZE message with invalid source %d\n", source);
	fflush(stderr);
#endif
	return;
    }
    if (_NET_WM_MOVERESIZE_first > direction || direction > _NET_WM_MOVERESIZE_last) {
#ifdef DEBUG_EWMH
	fprintf(stderr, "Received _NET_WM_MOVERESIZE message with invalid direction %d\n", direction);
	fflush(stderr);
#endif
	return;
    }
    TwmStartMoveResize(twin, x_root, y_root, direction, button, source);
}

/** @} */

/** @name _NET_RESTACK_WINDOW
  *
  * Pagers wanting to restack a window SHOULD send a _NET_RESTACK_WINDOW client
  * message request to the root window.
  *
  * This request is similar to ConfigureRequest with CWSibling and CWStackMode
  * flags.  It should be used only by pagers, applications can use normal
  * ConfigureRequests.  The source indication field should be therefore set to 2,
  * see the section called "Source indication in requests" for details.
  *
  * Rationale: A Window Manager may put restrictions on configure requests from
  * applications, for example it may under some conditions refuse to raise a
  * window.  This request makes it clear it comes from a pager or similar tool,
  * and therefore the Window Manager should always obey it.
  *
  * @{ */
static void
Rcv_NET_RESTACK_WINDOW(TwmWindow *twin, XClientMessageEvent *event)
{
    unsigned mask = 0;
    XWindowChanges changes = { 0, };
    enum _NET_SOURCE source = (int) event->data.l[0];
    enum _NET_RESTACK_DETAIL detail = (int) event->data.l[2];

    changes.sibling = (Window) event->data.l[1];
    if (changes.sibling != None)
	mask |= CWSibling;

    switch (source) {
    case _NET_SOURCE_UNSPECIFIED:
	/* might be using an older version of the specification */
    case _NET_SOURCE_PAGER:
	break;
    case _NET_SOURCE_APPLICATION:
#ifdef DEBUG_EWMH
	fprintf(stderr, "Applications should use XConfigureWindow instead of _NET_RESTACK_WINDOW\n");
	fflush(stderr);
#endif
	break;
    default:
#ifdef DEBUG_EWMH
	fprintf(stderr, "Received _NET_RESTACK_WINDOW with invalid source %d\n", source);
	fflush(stderr);
#endif
	return;
    }
    switch (detail) {
    case _NET_RESTACK_DETAIL_ABOVE:
	changes.stack_mode = Above;
	mask |= CWStackMode;
	break;
    case _NET_RESTACK_DETAIL_BELOW:
	changes.stack_mode = Below;
	mask |= CWStackMode;
	break;
    case _NET_RESTACK_DETAIL_TOPIF:
	changes.stack_mode = TopIf;
	mask |= CWStackMode;
	break;
    case _NET_RESTACK_DETAIL_BOTTOMIF:
	changes.stack_mode = BottomIf;
	mask |= CWStackMode;
	break;
    case _NET_RESTACK_DETAIL_OPPOSITE:
	changes.stack_mode = Opposite;
	mask |= CWStackMode;
	break;
    default:
#ifdef DEBUG_EWMH
	fprintf(stderr, "Received _NET_RESTACK_WINDOW with invalid detail %d\n", detail);
	fflush(stderr);
#endif
	return;
    }
    TwmRestackWindow(twin, mask, &changes, source);
}

/** @} */

/** @name _NET_REQUEST_FRAME_EXTENTS
  *
  * A Client whose window has not yet been mapped can request of the Window
  * Manager an estimate of the frame extents it will be given upon mapping.  To
  * retrieve such an estimate, the Client MUST send a _NET_REQUEST_FRAME_EXTENTS
  * message to the root window.  The Window Manager MUST respond by estimating
  * the prospective frame extents and setting the window's _NET_FRAME_EXTENTS
  * property accordingly.  The Client MUST handle the resulting
  * _NET_FRAME_EXTENTS PropertyNotify event.  So that the Window Manager has a
  * good basis for estimation, the Client MUST set any window properties it
  * intends to set before sending this message.  The Client MUST be able to cope
  * with imperfect estimates.
  *
  * Rationale: A client cannot calculate the dimensions of its window's frame
  * before the window is mapped, but some toolkits need this information.  Asking
  * the window manager for an estimate of the extents is a workable solution.
  * The estimate may depend on the current theme, font sizes or other window
  * properties.  The client can track changes to the frame's dimensions by
  * listening for _NET_FRAME_EXTENTS PropertyNotify events.
  *
  * @{ */
static void
Rcv_NET_REQUEST_FRAME_EXTENTS(ScreenInfo *scr, XClientMessageEvent *event)
{
    struct NetExtents extents = { 0, };
    Window window = event->window;

    TwmEstFrameExtents(window, &extents);
    Set_NET_FRAME_EXTENTS(window, &extents);
}

/** @} */

/** @} */

/** @name Window Manager Protocols.
  *
  * @{ */

/** @name _NET_WM_PING
  *
  * This protocol allows the Window Manager to determine if the Client is still
  * processing X events. This can be used by the Window Manager to determine if a
  * window which fails to close after being sent WM_DELETE_WINDOW has stopped
  * responding or has stalled for some other reason, such as waiting for user
  * confirmation. A Client SHOULD indicate that it is willing to participate in
  * this protocol by listing _NET_WM_PING in the WM_PROTOCOLS property of the
  * client window.
  *
  * A Window Manager can use this protocol at any time by sending a client
  * message.
  *
  * A participating Client receiving this message MUST send it back to the root
  * window immediately, by setting window = root, and calling XSendEvent with the
  * same event mask like all other root window messages in this specification
  * use. The Client MUST NOT alter any field in the event other than the window.
  * This includes all 5 longs in the data.l[5] array. The Window Manager can
  * uniquely identify the ping by the timestamp and the data.l[2] field if
  * necessary. Note that some older clients may not preserve data.l[2] through
  * data.l[4].
  *
  * The Window Manager MAY kill the Client (using _NET_WM_PID) if it fails to
  * respond to this protocol within a reasonable time.
  *
  * See also the implementation notes on killing hung processes.
  *
  * @{ */
static void
Rcv_NET_WM_PING(TwmWindow *twin, XClientMessageEvent *event)
{
    Time timestamp = (Time) event->data.l[1];
    Window client = (Window) event->data.l[2];

    TwmGotWMPing(twin, timestamp, client);
}

void
Snd_NET_WM_PING(TwmWindow *twin, Time timestamp)
{
    XClientMessageEvent event;

    event.type = ClientMessage;
    event.window = twin->w;
    event.message_type = _XA_WM_PROTOCOLS;
    event.format = 32;
    event.data.l[0] = _XA_NET_WM_PING;
    event.data.l[1] = timestamp;
    event.data.l[2] = twin->w;
    event.data.l[3] = event.data.l[4] = 0;
    XSendEvent(dpy, twin->w, False, SubstructureRedirectMask | SubstructureNotifyMask,
	       (XEvent *) &event);
}

/** @} */

/** @name _NET_WM_SYNC_REQUEST
  *
  * This protocol uses the XSync extension (see the protocol specification and
  * the library documentation) to let client and window manager synchronize the
  * repaint of the window manager frame and the client window. A client indicates
  * that it is willing to participate in the protocol by listing
  * _NET_WM_SYNC_REQUEST in the WM_PROTOCOLS property of the client window and
  * storing the XID of an XSync counter in the property
  * _NET_WM_SYNC_REQUEST_COUNTER. The initial value of this counter is not
  * defined by this specification.
  *
  * A window manager uses this protocol by preceding a ConfigureNotify event sent
  * to a client by a client message.
  *
  * After receiving one or more such message/ConfigureNotify pairs, and having
  * handled all repainting associated with the ConfigureNotify events, the client
  * MUST set the _NET_WM_SYNC_REQUEST_COUNTER to the 64 bit number indicated by
  * the data.l[2] and data.l[3] fields of the last client message received.
  *
  * By using either the Alarm or the Await mechanisms of the XSync extension, the
  * window manager can know when the client has finished handling the
  * ConfigureNotify events. The window manager SHOULD not resize the window
  * faster than the client can keep up.
  *
  * The update request number in the client message is determined by the window
  * manager subject to the restriction that it MUST NOT be 0. The number is
  * generally intended to be incremented by one for each message sent. Since the
  * initial value of the XSync counter is not defined by this specification, the
  * window manager MAY set the value of the XSync counter at any time, and MUST
  * do so when it first manages a new window.
  *
  * NOTE: This message should not be sent (and the WM can expect no XSync
  * response) unless the client has indicated support for _NET_WM_SYNC_REQUEST in
  * its WM_PROTOCOLS property.
  *
  * @{ */
void
Snd_NET_WM_SYNC_REQUEST(TwmWindow *twin, Time timestamp, struct NetCounter *counter)
{
    XClientMessageEvent event;

    event.type = ClientMessage;
    event.window = twin->w;
    event.message_type = _XA_WM_PROTOCOLS;
    event.format = 32;
    event.data.l[0] = _XA_NET_WM_SYNC_REQUEST;
    event.data.l[1] = timestamp;
    event.data.l[2] = counter->low;
    event.data.l[3] = counter->high;
    event.data.l[4] = 0;
    XSendEvent(dpy, twin->w, False, NoEventMask, (XEvent *) &event);
}

/** @} */

/** @name _NET_WM_SYNC_REQUEST_COUNTER counter_low, counter_high [2]/CARDINAL
  *
  * The value of the last _NET_WM_SYNC_REQUEST client message received in a
  * synchronized ConfigureNotify sequence.
  *
  * @{ */
static Bool
Get_NET_WM_SYNC_REQUEST_COUNTER(Window window, struct NetCounter *counter)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, window, _XA_NET_WM_SYNC_REQUEST_COUNTER, 0L, 2L, False,
			   XA_CARDINAL, &actual_type, &actual_format, &nitems,
			   &bytes_after, (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 2) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    counter->low = prop[0];
    counter->high = prop[1];
    XFree(prop);
    return True;
}

static void
Ret_NET_WM_SYNC_REQUEST_COUNTER(TwmWindow *twin)
{
    struct NetCounter counter = { 0, 0 };

    if (Get_NET_WM_SYNC_REQUEST_COUNTER(twin->w, &counter)) {
	twin->ewmh.props._NET_WM_SYNC_REQUEST_COUNTER = 1;
	TwmChgWMSyncRequestCounter(twin, &counter);
    } else
	twin->ewmh.props._NET_WM_SYNC_REQUEST_COUNTER = 0;
}

/** @} */

/** @name _NET_WM_FULLSCREEN_MONITORS, CARDINAL[4]/32
  *
  * A read-only list of 4 monitor indices indicating the top, bottom, left, and
  * right edges of the window when the fullscreen state is enabled. The indices
  * are from the set returned by the Xinerama extension.
  *
  * Windows transient for the window with _NET_WM_FULLSCREEN_MONITORS set, such
  * as those with type _NET_WM_WINDOW_TYPE_DIALOG, are generally expected to be
  * positioned (e.g. centered) with respect to only one of the monitors. This
  * might be the monitor containing the mouse pointer or the monitor containing
  * the non-full-screen window.
  *
  * @{ */
static void
Set_NET_WM_FULLSCREEN_MONITORS(Window window, struct NetMonitors *monitors)
{
    XChangeProperty(dpy, window, _XA_NET_WM_FULLSCREEN_MONITORS, XA_CARDINAL, 32,
		    PropModeReplace, (unsigned char *) monitors, 4);
}

static Bool
Get_NET_WM_FULLSCREEN_MONITORS(Window window, struct NetMonitors *monitors)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, window, _XA_NET_WM_FULLSCREEN_MONITORS, 0L, 4L, False,
			   XA_CARDINAL, &actual_type, &actual_format, &nitems,
			   &bytes_after, (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 4) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    monitors->top = prop[0];
    monitors->bottom = prop[1];
    monitors->left = prop[2];
    monitors->right = prop[3];
    XFree(prop);
    return True;

}

void
Upd_NET_WM_FULLSCREEN_MONITORS(ScreenInfo *scr, TwmWindow *twin)
{
    struct NetMonitors monitors = { 0, 0, 0, 0 };

    TwmGetWMFullscreenMonitors(scr, twin, &monitors);
    if (!twin->ewmh.props._NET_WM_FULLSCREEN_MONITORS
	|| cmp_mons(&twin->ewmh.monitors, &monitors) != 0) {
	Set_NET_WM_FULLSCREEN_MONITORS(twin->w, &monitors);
	twin->ewmh.props._NET_WM_FULLSCREEN_MONITORS = 1;
	twin->ewmh.monitors = monitors;
    }
}

static void
Ret_NET_WM_FULLSCREEN_MONITORS(ScreenInfo *scr, TwmWindow *twin)
{
    struct NetMonitors monitors = { 0, 0, 0, 0 };
    if (Get_NET_WM_FULLSCREEN_MONITORS(twin->w, &monitors)) {
	TwmSetWMFullscreenMonitors(scr, twin, &monitors);
	twin->ewmh.props._NET_WM_FULLSCREEN_MONITORS = 1;
	twin->ewmh.monitors = monitors;
    } else {
	twin->ewmh.props._NET_WM_FULLSCREEN_MONITORS = 0;
	twin->ewmh.monitors = monitors;
    }
    Upd_NET_WM_FULLSCREEN_MONITORS(scr, twin);
}

/** @brief Handle fullscreen montors change request.
  * @param twin - TWM window
  * @param event - client message
  *
  * A Client wishing to change this list MUST send a _NET_WM_FULLSCREEN_MONITORS
  * client message to the root window. The Window Manager MUST keep this list
  * updated to reflect the current state of the window.
  *
  * See the section called "Source indication in requests" for details on the
  * source indication.
  *
  * Virtual machine software may use this hint to have a virtual operating
  * system instance that sees multiple monitors. The application window
  * stretches over several monitors, giving the appearance that these monitors
  * have been taken over by the guest virtual machine.
  *
  * This hint might also be used by a movie or presentation application allowing
  * users to display the media spanned over several monitors.
  *
  * In both cases, the application would have some user interface allowing users
  * to configure which monitors the application fullscreens to. The window
  * manager need not provide such an interface, though it could.
  *
  * In the event of a change in monitor configuration, the application is
  * responsible for re-computing the monitors on which it wants to appear.  The
  * window manager may continue using the same monitor indices as before or
  * simply clear the list, returning to "normal" fullscreen.
  */
static void
Rcv_NET_WM_FULLSCREEN_MONITORS(ScreenInfo *scr, TwmWindow *twin,
			       XClientMessageEvent *event)
{
    struct NetMonitors monitors;

    monitors.top = event->data.l[0];
    monitors.bottom = event->data.l[1];
    monitors.left = event->data.l[2];
    monitors.right = event->data.l[3];
    TwmSetWMFullscreenMonitors(scr, twin, &monitors);
    Upd_NET_WM_FULLSCREEN_MONITORS(scr, twin);
}

/** @} */

/** @} */

/** @name Other properties.
 *
 * @{ */

/** @name _NET_WM_FULL_PLACEMENT
  *
  * By including this hint in _NET_SUPPORTED the Window Manager announces that it
  * performs reasonable window placement for all window types it supports (for
  * example centering dialogs on the mainwindow or whatever handling the Window
  * Manager considers reasonable). This in turn means that Clients, when they
  * detect that this hint is supported, SHOULD NOT abuse or often even use
  * PPosition and USPosition hints for requesting placement. In particular:
  *
  * - USPosition is reserved to be used only to indicate that the position was
  *   specified by the user and MUST NOT be used for anything else (see ICCCM
  *   section 4.1.2.3 for details)
  *
  * - PPosition SHOULD be used for for specifying position only if a specific
  *   position should be used. Position SHOULD NOT be specified for "default"
  *   placement such as centering dialog windows on their mainwindow.
  *
  * Rationale: Window managers can often perform better placement (that may be
  * even configurable) for windows than the application. However at the time of
  * writing this it is problematic for Window managers to decide when to use them
  * because many applications abuse positioning flags and/or provide unnecessary
  * default positions.
  *
  * Note: The property is not used anywhere else besides being listed in
  * _NET_SUPPORTED.
  *
  * @{ */

/*
 * None of the TWM variants support full placement and this property is not
 * included in _NET_SUPPORTED.
 */

/** @} */

/** @} */

/** @name Compositing Managers.
  *
  * @{ */

/** @name _NET_WM_WINDOW_OPACITY
  *
  * @{ */
static void
Set_NET_WM_WINDOW_OPACITY(Window window, unsigned char opacity)
{
    long data = (long) opacity;

    /* TODO: set error handler */
    XChangeProperty(dpy, window, _XA_NET_WM_WINDOW_OPACITY, XA_CARDINAL, 32,
		    PropModeReplace, (unsigned char *) &data, 1);
    XSync(dpy, False);
}

/** @} */

/** @name WM_TRANSIENT_FOR
  *
  * The WM_TRANSIENT_FOR property is defined by the [ICCCM] for managed windows.
  * This specification extends the use of the property to override-redirect
  * windows.  If an override-redirect is a pop-up on behalf of another window,
  * then the Client SHOULD set WM_TRANSIENT_FOR on the override-redirect to this
  * other window.
  *
  * As an example, a Client should set WM_TRANSIENT_FOR on dropdown menus to the
  * toplevel application window that contains the menubar.
  *
  * @{ */

/** @} */

/** @} */

/** @name Startup Notification.
  *
  * @{ */

/** @name _NET_STARTUP_ID, UTF8_STRING
  *
  * The ID used for the startup sequence for the window.  If set on a group
  * leader window, applies to all application windows in that group that do not
  * set their own _NET_STARTUP_ID.
  *
  * @{ */

/** @brief Set the startup notification identifier.
  * @param window - window to set the identifier
  * @param startup_id - the startup identifier to set
  */
static void
Set_NET_STARTUP_ID(Window window, char *startup_id)
{
    XTextProperty text = { NULL, };

    Xutf8TextListToTextProperty(dpy, &startup_id, 1, XUTF8StringStyle, &text);
    XSetTextProperty(dpy, window, &text, _XA_NET_STARTUP_ID);
    XFree(text.value);
}

/** @brief Get the startup notification identifier.
  * @param window - window from which to get
  * @param startup_id - where to store the startup identifier
  */
static Bool
Get_NET_STARTUP_ID(Window window, char **startup_id)
{
    Atom actual_type = None;
    XTextProperty text = { NULL, };
    int status, count = 0, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;
    char **list = NULL;

    status =
	XGetWindowProperty(dpy, window, _XA_WM_CLIENT_LEADER, 0L, 1L, False, XA_WINDOW,
			   &actual_type, &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    if (status == Success && actual_type != None && nitems >= 1) {
	window = prop[0];
	XFree(prop);
    } else {
	if (prop != NULL)
	    XFree(prop);
    }

    status = XGetTextProperty(dpy, window, &text, _XA_NET_STARTUP_ID);
    if (status == Success && text.nitems > 0) {
	XTextPropertyToStringList(&text, &list, &count);
	if (count > 0) {
	    free(*startup_id);
	    *startup_id = strdup(list[0]);
	}
	XFreeStringList(list);
	return True;
    }
    XFreeStringList(list);
    return False;
}

/** @brief Is startup notification sequence available for capture?
  * @param seq - startup notification sequence
  * @return Bool - truth value
  *
  * The sequence must be incomplete, not belong to another window, not have an
  * WMCLASS field, not be marked SILENT=1.
  */
static Bool
available_seq(EwmhSequence *seq)
{
    if (seq == NULL)
	return False;
    if (seq->state == _NET_NOTIFY_COMPLETE)
	return False;
    if (seq->twm != NULL)
	return False;
    if (seq->field.wmclass != NULL)
	return False;
    if (seq->field.silent != NULL && seq->numb.silent != 0)
	return True;
    return False;
}

static Bool
usable_seq(EwmhSequence *seq)
{
    if (seq == NULL)
	return False;
    if (seq->state == _NET_NOTIFY_COMPLETE)
	return False;
    if (seq->twm != NULL)
	return False;
    if (seq->field.wmclass != NULL)
	return True;
    return False;
}

/** @brief Find a startup sequence by identifier.
  * @param scr - screen
  * @param id - identifier to look up
  * @return EwmhSequence * - pointer to found sequence or NULL
  */
static EwmhSequence *
find_seq_by_id(ScreenInfo *scr, const char *id)
{
    EwmhSequence *seq = NULL;

    if (id == NULL || scr->ewmh.sequence == NULL)
	return (seq);
    for (seq = scr->ewmh.sequence; seq != NULL; seq = seq->next)
	if (strcmp(seq->field.id, id) == 0)
	    break;
    return (seq);
}

/** @brief Find a startup sequence by resource name or class.
  * @param scr - screen
  * @param res - resource to look up
  * @return EwmhSequence * - pointer to found sequence or NULL
  */
static EwmhSequence *
find_seq_by_res(ScreenInfo *scr, const XClassHint *res)
{
    EwmhSequence *seq = NULL;

    if (res == NULL || res->res_name == NULL || res->res_class == NULL
	|| scr->ewmh.sequence == NULL)
	return (seq);
    for (seq = scr->ewmh.sequence; seq != NULL; seq = seq->next)
	if (usable_seq(seq) && seq->field.wmclass != NULL
	    && (strcmp(seq->field.wmclass, res->res_name) == 0
		|| strcmp(seq->field.wmclass, res->res_class) == 0))
	    break;
    return (seq);
}

/** @brief Find a startup sequence by pid and machine.
  * @param scr - screen
  * @param pid - pid to find
  * @param machine - machine to find
  * @return EwmhSequence * - pointer to found sequence or NULL
  */
static EwmhSequence *
find_seq_by_pid(ScreenInfo *scr, const pid_t pid, const char *machine)
{
    EwmhSequence *seq = NULL;

    if (pid == 0 || machine == NULL || scr->ewmh.sequence == NULL)
	return (seq);
    for (seq = scr->ewmh.sequence; seq != NULL; seq = seq->next)
	if (available_seq(seq) && seq->field.pid != NULL && seq->field.hostname != NULL
	    && seq->numb.pid == pid && strcmp(seq->field.hostname, machine) == 0)
	    break;
    return (seq);
}

/** @brief Make a command string from an argv list.
  * @param argv - argv[] string list
  *
  * This uses the same approach that we use to create COMMAND= field in the
  * launcher: if the argv element contains '"' or ' ', quote the string with
  * '"', escape existing '"' with '\'.  Joint all arguments with a single space.
  */
static char *
make_command_from_argv(char **argv)
{
    char **p;
    char *command, *q;
    int index = 0, argl;

    if ((command = malloc(1)) == NULL)
	return (command);
    q = command;
    q[index] = '\0';

    for (p = argv; *p != NULL; p++) {
	if (index != 0) {
	    q[index++] = ' ';
	    q[index] = '\0';
	}
	argl = strlen(*p);
	if (argl == 0 || strchr(*p, ' ') != NULL || strchr(*p, '"') != NULL) {
	    const char *c = *p;

	    if ((command = realloc(command, index + 1 + 2 * argl + 2 + 1)) == NULL) {
		free(q);
		return (NULL);
	    } else
		q = command;
	    q[index++] = '"';
	    while (*c != '\0') {
		if (*c == '"')
		    q[index++] = '\\';
		q[index++] = *c;
	    }
	    q[index++] = '"';
	    q[index] = '\0';
	} else {
	    if ((command = realloc(command, index + 1 + argl + 1)) == NULL) {
		free(q);
		return (NULL);
	    } else
		q = command;
	    strcat(&q[index], *p);
	    index += argl;
	    q[index] = '\0';
	}
    }
    return (command);
}

/** @brief Find a startup sequence by command.
  * @param scr - screen
  * @param argv - string list
  * @return EwmhSequence * - pointer to the found sequence
  *
  */
static EwmhSequence *
find_seq_by_cmd(ScreenInfo *scr, char **argv)
{
    EwmhSequence *seq = NULL;
    char *command;

    if (argv == NULL || *argv == NULL || scr->ewmh.sequence == NULL)
	return (seq);
    if ((command = make_command_from_argv(argv)) == NULL)
	return (seq);
    for (seq = scr->ewmh.sequence; seq != NULL; seq = seq->next)
	if (available_seq(seq) && seq->field.command != NULL
	    && strcmp(seq->field.command, command) == 0)
	    break;
    free(command);
    return (seq);
}

/** @brief Find a startup sequence by timestamp.
  * @param scr - screen
  * @param timestamp - timestamp to find
  * @return EwmhSequence * - pointer to found sequence or NULL
  */
static EwmhSequence *
find_seq_by_time(ScreenInfo *scr, unsigned timestamp)
{
    EwmhSequence *seq = NULL;

    if (timestamp == 0 || scr->ewmh.sequence == NULL)
	return (seq);
    for (seq = scr->ewmh.sequence; seq != NULL; seq = seq->next)
	if (available_seq(seq) && seq->field.timestamp != NULL
	    && seq->numb.timestamp == timestamp)
	    break;
    return (seq);
}

static EwmhSequence *
find_seq(ScreenInfo *scr, TwmWindow *twin)
{
    EwmhSequence *seq = NULL;

    if (scr->ewmh.sequence == NULL)
	return (seq);
    if ((seq = find_seq_by_id(scr, twin->ewmh.startup_id)) != NULL)
	return (seq);
    if ((seq = find_seq_by_res(scr, &twin->ewmh.class)) != NULL)
	return (seq);
    if ((seq = find_seq_by_pid(scr, twin->ewmh.pid, twin->ewmh.machine)) != NULL)
	return (seq);
    if ((seq = find_seq_by_cmd(scr, twin->ewmh.command)) != NULL)
	return (seq);
    if ((seq = find_seq_by_time(scr, twin->ewmh.user_time)) != NULL)
	return (seq);
    return (seq);
}

static void
free_sequence_fields(EwmhSequence *seq)
{
    int i;

    for (i = 0; i < sizeof(seq->fields) / sizeof(char *); i++)
	free(seq->fields[i]);
}

/** @brief Remove a startup notification sequence from the screen list.
  * @param seq - startup notification sequence
  *
  * Removes a startup notification sequence from the screen list and when it is
  * not referred to by a window, frees the structure itself.  The pointer passed
  * to this function may be invalidated.
  *
  */
static void
remove_seq(EwmhSequence *seq)
{
    if (seq != NULL) {
	if (seq->prev != NULL) {
	    if ((*(seq->prev) = seq->next) != NULL)
		seq->next->prev = seq->prev;
	    seq->prev = NULL;
	    seq->next = NULL;
	}
	if (seq->twm == NULL) {
	    free_sequence_fields(seq);
	    free(seq);
	}
    }
}

/** @brief Releases a startup notification sequence.
  * @param seq - startup notification sequence
  *
  * Removes a startup notifications sequence from a window and when it is not on
  * a screen list, frees the structure itself.  The pointer passed to this
  * function may be invalidated.
  */
static void
release_seq(EwmhSequence *seq)
{
    if (seq != NULL) {
	if (seq->twm != NULL) {
	    seq->twm->ewmh.sequence = NULL;
	    seq->twm = NULL;
	}
	if (seq->prev == NULL) {
	    free_sequence_fields(seq);
	    free(seq);
	}
    }
}

/** @brief Frees a startup notification sequence.
  * @param seq - startup notification sequence
  *
  * Removes a startup notifcation sequence from its screen list (if any) and
  * associated window (if any) and frees the structure.  This invalidates all
  * references to seq.
  */
static void
free_seq(EwmhSequence *seq)
{
    if (seq != NULL) {
	if (seq->twm != NULL) {
	    seq->twm->ewmh.sequence = NULL;
	    seq->twm = NULL;
	}
	if (seq->prev != NULL) {
	    if ((*(seq->prev) = seq->next) != NULL)
		seq->next->prev = seq->prev;
	    seq->prev = NULL;
	    seq->next = NULL;
	}
	free_sequence_fields(seq);
	free(seq);
    }
}

/** @brief Is startup notification sequence removeable by WM?
  * @param seq - startup notification sequence
  * @return Bool - truth value
  */
static Bool
removable_seq(EwmhSequence *seq)
{
    if (seq == NULL)
	return False;
    if (seq->field.wmclass != NULL)
	return True;
    if (seq->field.silent != NULL && seq->numb.silent != 0)
	return True;
    return False;
}

EwmhSequence *
Seq_NET_STARTUP_ID(ScreenInfo *scr, TwmWindow *twin)
{
    EwmhSequence *seq;

    if ((seq = twin->ewmh.sequence) != NULL)
	return (seq);

    /* ensure that these were called before searching */
    Ret_WM_CLASS(twin);
    Ret_WM_CLIENT_MACHINE(twin);
    Ret_WM_COMMAND(twin);
    Ret_NET_WM_PID(scr, twin);
    Ret_NET_WM_USER_TIME_WINDOW(scr, twin);
    Ret_NET_WM_USER_TIME(twin);

    if ((seq = find_seq(scr, twin)) != NULL) {
	Bool force = False;

	if (seq->twm != NULL) {
	    seq->twm->ewmh.sequence = NULL;
	    force = True;
	}
	twin->ewmh.sequence = seq;
	seq->twm = twin;
	Upd_NET_STARTUP_INFO(scr, twin, force);
    }
    return (seq);
}

void
Upd_NET_STARTUP_ID(ScreenInfo *scr, TwmWindow *twin)
{
    if (!twin->ewmh.props._NET_STARTUP_ID) {
	char *startup_id = NULL;
	EwmhSequence *seq;

	if ((seq = twin->ewmh.sequence) == NULL)
	    seq = Seq_NET_STARTUP_ID(scr, twin);
	if (seq != NULL && (startup_id = strdup(seq->field.id)) != NULL) {
	    Set_NET_STARTUP_ID(twin->w, startup_id);
	    twin->ewmh.props._NET_STARTUP_ID = 1;
	    free(twin->ewmh.startup_id);
	    twin->ewmh.startup_id = startup_id;

	    /* update these from sequence if not done already or otherwise set */
	    Upd_NET_WM_FULLSCREEN_MONITORS(scr, twin);
	    Upd_NET_WM_DESKTOP(scr, twin);
	    Upd_NET_WM_USER_TIME(twin);
	    Upd_WM_CLIENT_MACHINE(twin);
	    Upd_NET_WM_PID(twin);
	}
    }
}

/** @brief Retrieve the startup identifier and sequence.
  * @param scr - screen
  * @param twin - TWM window
  *
  * This function looks for the _NET_STARTUP_ID property first on the window
  * itself, and then any group leader window that is associated with the window.
  */
static void
Ret_NET_STARTUP_ID(ScreenInfo *scr, TwmWindow *twin)
{
    char *startup_id = NULL;
    Window group = None;

    if (Get_NET_STARTUP_ID(twin->w, &startup_id)
	|| (twin->wmhints != NULL && (group = twin->wmhints->window_group) != None
	    && group != twin->w && Get_NET_STARTUP_ID(group, &startup_id))) {
	TwmSetStartupId(scr, twin, startup_id);
	twin->ewmh.props._NET_STARTUP_ID = 1;
	free(twin->ewmh.startup_id);
	twin->ewmh.startup_id = startup_id;
    } else {
	twin->ewmh.props._NET_STARTUP_ID = 0;
	free(twin->ewmh.startup_id);
	twin->ewmh.startup_id = NULL;
    }
    Upd_NET_STARTUP_ID(scr, twin);
}

static void
Del_NET_STARTUP_ID(TwmWindow *twin)
{
    if (twin->ewmh.props._NET_STARTUP_ID) {
	XDeleteProperty(dpy, twin->w, _XA_NET_STARTUP_ID);
	free(twin->ewmh.startup_id);
	twin->ewmh.startup_id = NULL;
	twin->ewmh.props._NET_STARTUP_ID = 0;
    }
}

/** @} */

/** @name _NET_STARTUP_INFO_BEGIN
  *
  * The _NET_STARTUP_INFO_BEGIN client message is a client message that starts
  * the first segment of a multiple segment startup notification message.  The
  * event->window describes the source of the message partitions.  The last
  * message contains a nul byte (0) in one of the 20 data.b elements.
  *
  * @{ */

static const char *NetFields[] = {
    "ID",
    "NAME",
    "SCREEN",
    "BIN",
    "ICON",
    "DESKTOP",
    "TIMESTAMP",
    "DESCRIPTION",
    "WMCLASS",
    "SILENT",
    "APPLICATION_ID",
    "LAUNCHER",
    "LAUNCHEE",
    "HOSTNAME",
    "PID",
    "COMMAND",
    "FILE",
    "URL",
    NULL
};

static void
copy_sequence_fields(EwmhSequence *old, EwmhSequence *new)
{
    int i;

    for (i = 0; i < sizeof(old->fields) / sizeof(char *); i++) {
	if (new->fields[i] != NULL) {
	    free(old->fields[i]);
	    old->fields[i] = new->fields[i];
	}
    }
    if (new->field.screen != NULL)
	old->numb.screen = atoi(old->field.screen);
    if (new->field.desktop != NULL)
	old->numb.desktop = atoi(old->field.desktop);
    if (new->field.timestamp != NULL)
	old->numb.timestamp = atoi(old->field.timestamp);
    if (new->field.silent != NULL)
	old->numb.silent = atoi(old->field.silent);
    if (new->field.pid != NULL)
	old->numb.pid = atoi(old->field.pid);
}

/** @brief End a startup notification message sequence
  * @param scr - screen
  * @param notify - complete notification message sequence
  */
void
End_NET_STARTUP_INFO(ScreenInfo *scr, EwmhNotify *notify)
{
    EwmhSequence command = { NULL, }, *seq;
    char *p = notify->message, *k, *v, *q;
    const char **f;
    int i;
    int escaped, quoted;

    if (0) {
    } else if (p == strstr(p, "new:")) {
	command.state = _NET_NOTIFY_NEW;
	p += 4;
    } else if (p == strstr(p, "change:")) {
	command.state = _NET_NOTIFY_CHANGED;
	p += 7;
    } else if (p == strstr(p, "remove:")) {
	command.state = _NET_NOTIFY_COMPLETE;
	p += 7;
    } else
	return;			/* wrong command */
    while (*p != '\0') {
	while (*p == ' ')
	    p++;
	k = p;
	if ((v = strchr(k, '=')) == NULL) {
	    free_sequence_fields(&command);
	    return;		/* mangled message */
	} else {
	    *v++ = '\0';
	    p = q = v;
	    escaped = quoted = 0;
	    for (;;) {
		if (!escaped) {
		    if (*p == '"') {
			p++;
			quoted ^= 1;
			continue;
		    } else if (*p == '\\') {
			p++;
			escaped = 1;
			continue;
		    } else if (*p == '\0' || (*p == ' ' && !quoted)) {
			if (quoted) {
			    free_sequence_fields(&command);
			    return;	/* mangled message */
			}
			if (*p == ' ')
			    p++;
			*q = '\0';
			break;
		    }
		}
		*q++ = *p++;
		escaped = 0;
	    }
	    for (i = 0, f = NetFields; f[i] != NULL; i++)
		if (strcmp(f[i], k) == 0)
		    break;
	    if (f[i] != NULL)
		command.fields[i] = strdup(v);
	}
    }
    if (command.field.id == NULL) {
	free_sequence_fields(&command);
	return;			/* all messages require ID= field */
    }
    /* get timestamp from ID if necessary */
    if (command.field.timestamp == NULL
	&& (p = strstr(command.field.id, "_TIME")) != NULL)
	command.field.timestamp = strdup(p + 5);
    if (command.field.screen != NULL)
	command.numb.screen = atoi(command.field.screen);
    if (command.field.desktop != NULL)
	command.numb.desktop = atoi(command.field.desktop);
    if (command.field.timestamp != NULL)
	command.numb.timestamp = atoi(command.field.timestamp);
    if (command.field.silent != NULL)
	command.numb.silent = atoi(command.field.silent);
    if (command.field.pid != NULL)
	command.numb.pid = atoi(command.field.pid);
    if ((seq = find_seq_by_id(scr, command.field.id)) == NULL) {
	if (command.state != _NET_NOTIFY_NEW) {
	    free_sequence_fields(&command);
	    return;		/* out of sequence message */
	}
	if ((seq = calloc(1, sizeof(*seq))) == NULL) {
	    free_sequence_fields(&command);
	    return;		/* no memory */
	}
	*seq = command;
	if ((seq->next = scr->ewmh.sequence) != NULL)
	    seq->next->prev = &seq->next;
	seq->prev = &scr->ewmh.sequence;
	seq->twm = NULL;
	seq->changed = False;
	return;
    }
    switch (seq->state) {
    case _NET_NOTIFY_IDLE:
	switch (command.state) {
	case _NET_NOTIFY_IDLE:
	    /* error */
	    return;
	case _NET_NOTIFY_COMPLETE:
	    seq->state = _NET_NOTIFY_COMPLETE;
	    /* remove sequence */
	    break;
	case _NET_NOTIFY_NEW:
	    seq->state = _NET_NOTIFY_NEW;
	    copy_sequence_fields(seq, &command);
	    if (seq->twm != NULL) {
		Upd_NET_STARTUP_ID(scr, seq->twm);
		Upd_NET_STARTUP_INFO(scr, seq->twm, False);
	    }
	    return;
	case _NET_NOTIFY_CHANGED:
	    seq->state = _NET_NOTIFY_CHANGED;
	    copy_sequence_fields(seq, &command);
	    if (seq->twm != NULL) {
		Upd_NET_STARTUP_ID(scr, seq->twm);
		Upd_NET_STARTUP_INFO(scr, seq->twm, False);
	    }
	    return;
	}
	break;
    case _NET_NOTIFY_NEW:
	switch (command.state) {
	case _NET_NOTIFY_IDLE:
	    /* error */
	    return;
	case _NET_NOTIFY_COMPLETE:
	    seq->state = _NET_NOTIFY_COMPLETE;
	    /* remove sequence */
	    break;
	case _NET_NOTIFY_NEW:
	case _NET_NOTIFY_CHANGED:
	    seq->state = _NET_NOTIFY_CHANGED;
	    copy_sequence_fields(seq, &command);
	    if (seq->twm != NULL)
		Upd_NET_STARTUP_ID(scr, seq->twm);
	    return;
	}
	break;
    case _NET_NOTIFY_CHANGED:
	switch (command.state) {
	case _NET_NOTIFY_IDLE:
	    /* error */
	    return;
	case _NET_NOTIFY_COMPLETE:
	    seq->state = _NET_NOTIFY_COMPLETE;
	    /* remove sequence */
	    break;
	case _NET_NOTIFY_NEW:
	case _NET_NOTIFY_CHANGED:
	    seq->state = _NET_NOTIFY_CHANGED;
	    copy_sequence_fields(seq, &command);
	    if (seq->twm != NULL)
		Upd_NET_STARTUP_ID(scr, seq->twm);
	    return;
	}
	break;
    case _NET_NOTIFY_COMPLETE:
	/* remove sequence */
	break;
    }
    /* remove sequence */
    remove_seq(seq);
}

/** @brief Begin a startup notification message sequence.
  * @param scr - screen
  * @param event - client message event
  * @param notify - existing notification structure or NULL
  */
static void
Rcv_NET_STARTUP_INFO_BEGIN(ScreenInfo *scr, XClientMessageEvent *event,
			   EwmhNotify *notify)
{
    int len;

    if (notify == NULL) {
	Window source = event->window;

	if ((notify = calloc(1, sizeof(*notify))) == NULL)
	    return;
	if ((notify->next = scr->ewmh.notify) != NULL)
	    notify->next->prev = &notify->next;
	notify->prev = &scr->ewmh.notify;
	scr->ewmh.notify = notify;
	notify->source = source;
	XSaveContext(dpy, source, NotifyContext, (XPointer) notify);
	XSaveContext(dpy, source, ScreenContext, (XPointer) scr);
	XSelectInput(dpy, source, StructureNotifyMask);
    }
    if (notify->message != NULL) {
	free(notify->message);
	notify->message = NULL;
    }
    len = strnlen(event->data.b, 20);
    if ((notify->message = strndup(event->data.b, 20)) == NULL)
	return;
    if (len < 20)
	End_NET_STARTUP_INFO(scr, notify);
}

/** @} */

/** @name _NET_STARTUP_INFO
  *
  * @{ */
static void
Rcv_NET_STARTUP_INFO(ScreenInfo *scr, XClientMessageEvent *event, EwmhNotify *notify)
{
    int len;

    if (notify == NULL)		/* out of sequence */
	return;
    if (notify->message == NULL)	/* no begin */
	return;
    len = strnlen(event->data.b, 20);
    if ((notify->message =
	 realloc(notify->message, strlen(notify->message) + len + 1)) == NULL)
	return;
    strncat(notify->message, event->data.b, 20);
    if (len < 20)
	End_NET_STARTUP_INFO(scr, notify);
}

/** @brief Release startup notification messages.
  * @param scr - screen
  * @param notify - corresponding notify structure
  *
  * This function is called whenever the window associated with the source of a
  * startup notification message sequence is destroyed.  We release the
  * structure and deallocate the memory associated with the structure.
  */
static void
Rel_NET_STARTUP_INFO(ScreenInfo *scr, EwmhNotify *notify)
{
    XDeleteContext(dpy, notify->source, NotifyContext);
    XDeleteContext(dpy, notify->source, ScreenContext);

    if ((*(notify->prev) = notify->next) != NULL)
	notify->next->prev = notify->prev;

    free(notify);
}

static void
Snd_NET_STARTUP_INFO(Window from, Window to, char *msg)
{
    XClientMessageEvent xev;
    int len;

    xev.type = ClientMessage;
    xev.message_type = _XA_NET_STARTUP_INFO_BEGIN;
    xev.display = dpy;
    xev.window = from;
    xev.format = 8;

    len = strlen(msg) + 1;

    while (len > 0) {
	strncpy(xev.data.b, msg, 20);
	msg += 20;
	len -= 20;
	XSendEvent(dpy, to, False, PropertyChangeMask, (XEvent *) &xev);
	xev.message_type = _XA_NET_STARTUP_INFO;
    }
}

static void
Snd_NET_STARTUP_INFO_MSG(ScreenInfo *scr, EwmhSequence *seq, enum _NET_NOTIFY_MSG type)
{
    char *msg, *cmd;
    int f, fend, index = 0;

    switch (type) {
    case _NET_NOTIFY_MSG_NEW:
	cmd = "new:";
	fend = sizeof(NetFields) / sizeof(char *);
	break;
    case _NET_NOTIFY_MSG_CHANGE:
	cmd = "change:";
	fend = sizeof(NetFields) / sizeof(char *);
	break;
    case _NET_NOTIFY_MSG_REMOVE:
	cmd = "remove:";
	fend = 1;
	break;
    }
    if ((msg = calloc(strlen(cmd) + 1, 1)) == NULL)
	return;
    msg[index] = '\0';
    strcat(msg, cmd);
    index += strlen(cmd);
    msg[index] = '\0';

    for (f = 0; f < fend; f++) {
	const char *k, *p;

	if ((k = NetFields[f]) == NULL)
	    continue;
	if ((p = seq->fields[f]) == NULL)
	    continue;
	if ((cmd = realloc(msg, strlen(msg) + strlen(k) + 1 + 2 * strlen(p) + 2)) == NULL) {
	    free(msg);
	    return;
	}
	msg = cmd;
	msg[index++] = ' ';
	msg[index] = '\0';
	while (*k != '\0')
	    msg[index++] = *k++;
	msg[index++] = '=';
	msg[index] = '\0';
	while (*p != '\0') {
	    if (*p == ' ' || *p == '"')
		msg[index++] = '\\';
	    msg[index++] = *p++;
	}
	msg[index] = '\0';
    }
    Snd_NET_STARTUP_INFO(scr->ManagerWindow, scr->Root, msg);
}

/** @brief Update startup notification sequence information from window.
  * @param scr - screen
  * @param twin - TWM window
  * 
  * This function should be called when a startup notification sequence is
  * initially associated with a window.  The window manager copies what
  * information it knows about that was not included in the "new:" or subsequent
  * "change:" message and generates a "change:" message of its own to update the
  * information.
  *
  * When the sequence is has a WMCLASS field or it has a SILENT field that is
  * non-zero, a "remove:" message will also be sent and the sequence will be
  * removed from the screen list.
  */
void
Upd_NET_STARTUP_INFO(ScreenInfo *scr, TwmWindow *twin, Bool forced)
{
    char *str;
    Bool changed = False;
    EwmhSequence *seq;

    if ((seq = twin->ewmh.sequence) == NULL)
	return;

    /* update SCREEN field */
    if (forced || seq->field.screen == NULL) {
	/* shouldn't be necessary unless "new:" message was missed */
	if ((str = calloc(1, 32)) != NULL) {
	    snprintf(str, 32, "%d", scr->screen);
	    free(seq->field.screen);
	    seq->field.screen = str;
	    seq->numb.screen = scr->screen;
	    changed = True;
	}
    }
    /* update DESKTOP field */
    if (forced || seq->field.desktop == NULL) {
	if (twin->ewmh.props._NET_WM_DESKTOP && twin->ewmh.desktop != -2) {
	    if ((str = calloc(1, 32)) != NULL) {
		snprintf(str, 31, "%d", twin->ewmh.desktop);
		free(seq->field.desktop);
		seq->field.desktop = str;
		seq->numb.desktop = (unsigned) twin->ewmh.desktop;
		changed = True;
	    }
	}
    }
    /* update TIMESTAMP field */
    if (forced || seq->field.timestamp == NULL) {
	if ((str = strstr(seq->field.id, "_TIME")) != NULL) {
	    free(seq->field.timestamp);
	    seq->field.timestamp = strdup(str + 5);
	    changed = True;
	}
    }
#if 0
    /* update WMCLASS (and SILENT) field */
    if (seq->field.wmclass == NULL) {
	if (twin->class.res_name != NULL || twin->class.res_class != NULL) {
	    if (seq->field.silent == NULL) {
		if ((str = calloc(1, 8)) != NULL) {
		    snprintf(str, 7, "%d", 0);
		    seq->field.silent = str;
		    seq->numb.silent = 0;
		}
	    }
	    if (twin->class.res_name != NULL)
		seq->field.wmclass = strdup(twin->class.res_name);
	    else if (twin->class.res_class != NULL)
		seq->field.wmclass = strdup(twin->class.res_name);
	    changed = True;
	}
    }
#endif
    /* update HOSTNAME field */
    if (forced || seq->field.hostname == NULL) {
	if (twin->ewmh.props.WM_CLIENT_MACHINE && twin->ewmh.machine) {
	    free(seq->field.hostname);
	    seq->field.hostname = strdup(twin->ewmh.machine);
	    changed = True;
	}
    }
    /* update PID field */
    if (forced || seq->field.pid == NULL) {
	if (twin->ewmh.props._NET_WM_PID && twin->ewmh.pid != 0) {
	    if ((str = calloc(1, 32)) != NULL) {
		snprintf(str, 31, "%d", twin->ewmh.pid);
		free(seq->field.pid);
		seq->field.pid = str;
		seq->numb.pid = twin->ewmh.pid;
		changed = True;
	    }
	}
    }
    /* update COMMAND field */
    if (forced || seq->field.command == NULL) {
	if (twin->ewmh.props.WM_COMMAND && twin->ewmh.command != NULL) {
	    free(seq->field.command);
	    seq->field.command = make_command_from_argv(twin->ewmh.command);
	    changed = True;
	}
    }
    switch (seq->state) {
    case _NET_NOTIFY_NEW:
    case _NET_NOTIFY_CHANGED:
	if (changed)
	    Snd_NET_STARTUP_INFO_MSG(scr, seq, _NET_NOTIFY_MSG_CHANGE);
	if (seq->field.wmclass != NULL
	    && (seq->field.silent == NULL || seq->numb.silent != 0)) {
	    Snd_NET_STARTUP_INFO_MSG(scr, seq, _NET_NOTIFY_MSG_REMOVE);
	    remove_seq(seq);
	}
	break;
    case _NET_NOTIFY_IDLE:
    case _NET_NOTIFY_COMPLETE:
	/* should not send messages in these states */
	break;
    }
}

/** @} */

/** @} */

/** @name System Tray.
  *
  * None of the TWM variants directly support a system tray.  Therefore, it is
  * not necessary to support any of the system tray properties or protocols.
  *
  * @{ */

/** @name _NET_SYSTEM_TRAY_MESSAGE_DATA
  *
  * @{ */

/** @} */

/** @name _NET_SYSTEM_TRAY_OPCODE
  *
  * @{ */

/** @} */

/** @name _NET_SYSTEM_TRAY_ORIENTATION
  *
  * @{ */
static void
Set_NET_SYSTEM_TRAY_ORIENTATION(Window root,
				enum _NET_SYSTEM_TRAY_ORIENTATION orientation)
{
    long data = (long) orientation;

    XChangeProperty(dpy, root, _XA_NET_SYSTEM_TRAY_ORIENTATION, XA_CARDINAL, 32,
		    PropModeReplace, (unsigned char *) &data, 1);
}

/** @} */

/** @name _NET_SYSTEM_TRAY_VISUAL
  *
  * @{ */
static void
Set_NET_SYSTEM_TRAY_VISUAL(Visual visual)
{
}

/** @} */

/** @} */

/** @name KDE properties
  *
  * @{ */

/** @name _KDE_NET_SYSTEM_TRAY_WINDOWS  windows WINDOW[]/32
  *
  * Provides a list of KDE dock application windows.  These are windows that
  * had the _KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR property set on them when they
  * were initially mapped.  The window manager does not manage these windows and
  * they are treated as though they were override-redirect windows.
  *
  * @{ */
static void
Set_KDE_NET_SYSTEM_TRAY_WINDOWS(Window root, Window *systray, int count)
{
    int n;
    long windows[count];

    for (n = 0; n < count; n++)
	windows[n] = systray[n];

    XChangeProperty(dpy, root, _XA_KDE_NET_SYSTEM_TRAY_WINDOWS, XA_WINDOW, 32,
		    PropModeReplace, (unsigned char *) windows, count);
}

/** @brief Update the KDE system tray windows list.
  * @param scr - screen
  *
  * Should be called whenever the KDE system tray window list changes.  Note
  * that this function only changes the property when the list actually changes.
  */
void
Upd_KDE_NET_SYSTEM_TRAY_WINDOWS(ScreenInfo *scr)
{
    Window *systray = NULL;
    int count = 0;

    TwmGetKdeSystemTrayWindows(scr, &systray, &count);
    if (!scr->ewmh.props._KDE_NET_SYSTEM_TRAY_WINDOWS
	|| cmp_window_list(scr->ewmh.systray, systray) != 0) {
	Set_KDE_NET_SYSTEM_TRAY_WINDOWS(scr->Root, systray, count);
	scr->ewmh.props._KDE_NET_SYSTEM_TRAY_WINDOWS = 1;
	free(scr->ewmh.systray);
	scr->ewmh.systray = systray;
    }
}

/** @} */

/** @name _KDE_NET_WM_FRAME_STRUT
  *
  * Not to be confused with _NET_WM_STRUT, this property is in fact identical to
  * _NET_FRAME_EXTENTS and is handled by the functions associated with the
  * _NET_FRAME_EXTENTS property.
  *
  * @{ */
/** @} */

/** @name _KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR
  *
  * This property is set by KDE dock applications to mark that they are intended
  * for the KDE dock.  When the window manager supports docking KDE dock
  * applications (such as fluxbox), this is an indication to "swallow" that
  * application into the dock/harbour/slit.  When the window manager does not
  * directly support KDE dock applications, the window manager should add its
  * window id to the _KDE_NET_SYSTEM_TRAY_WINDOWS property array on the root
  * window.
  *
  * @{ */
static Bool
Get_KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR(Window window)
{
    return False;
}

static void
Ret_KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR(TwmWindow *twin)
{
}

/** @} */

/** @name _KDE_NET_WM_WINDOW_TYPE_OVERRIDE
  *
  * This property prevents the window that has it set from getting any
  * decorations or functionality.  In a sense, it "overrides" the normal
  * _NET_WM_WINDOW_TYPE and the decor and function settings that would normally
  * be applied.
  *
  * @{ */
/** @} */

/** @} */

/** @name Additional non-standard properties.
  *
  * @{ */

/** @name _NET_MAXIMIZED_RESTORE x, y, width, height, vx, vy [6]/CARDINAL
  *
  * This property is used by waimea to save information for restoring maximized
  * windows.  x, y, width, height is the geometry of the unmaximized window.  vx
  * and vy is the maximized position on the virtual root.
  *
  * For TWM variants, we use this property to save the restored size of a
  * maximized or fullscreened window for restart behaviour or in the event of a
  * crash.
  *
  * @{ */
static void
Set_NET_MAXIMIZED_RESTORE(Window window, struct NetRestore *restore)
{
    XChangeProperty(dpy, window, _XA_NET_MAXIMIZED_RESTORE, XA_CARDINAL, 32,
		    PropModeReplace, (unsigned char *) restore, 6);
}

static Bool
Get_NET_MAXIMIZED_RESTORE(Window window, struct NetRestore *restore)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, window, _XA_NET_MAXIMIZED_RESTORE, 0L, 6L, False,
			   XA_CARDINAL, &actual_type, &actual_format, &nitems,
			   &bytes_after, (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 6) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    *restore = *(struct NetRestore *) prop;
    return True;
}

/** @brief Update the maximized restore configuration for a window.
  * @param twin - TWM window
  *
  * Should be called whenever a window is maximized and the configuration for
  * restoration after maximization changes.  Note that this property is only
  * changed when the restored configuration acutal changes.
  */
void
Upd_NET_MAXIMIZED_RESTORE(TwmWindow *twin)
{
    struct NetRestore restore = { 0, };

    TwmGetMaximizedRestore(twin, &restore);
    if (!twin->ewmh.props._NET_MAXIMIZED_RESTORE
	|| cmp_restore(&twin->ewmh.restore, &restore) != 0) {
	Set_NET_MAXIMIZED_RESTORE(twin->w, &restore);
	twin->ewmh.props._NET_MAXIMIZED_RESTORE = 1;
	twin->ewmh.restore = restore;
    }
}

/** @brief Retrieve the maimixzed restore configuration for a window.
  * @param twin - TWM window
  *
  * Should be called when a window is intially managed to retrieve or set the
  * maximized restore configuration.
  */
static void
Ret_NET_MAXIMIZED_RESTORE(TwmWindow *twin)
{
    struct NetRestore restore = { 0, };

    if (Get_NET_MAXIMIZED_RESTORE(twin->w, &restore)) {
	TwmSetMaximizedRestore(twin, &restore);
	twin->ewmh.props._NET_MAXIMIZED_RESTORE = 1;
	twin->ewmh.restore = restore;
    } else {
	twin->ewmh.props._NET_MAXIMIZED_RESTORE = 0;
	twin->ewmh.restore = restore;
    }
    Upd_NET_MAXIMIZED_RESTORE(twin);
}

void
Del_NET_MAXIMIZED_RESTORE(TwmWindow *twin)
{
    XDeleteProperty(dpy, twin->w, _XA_NET_MAXIMIZED_RESTORE);
}

/** @} */

/** @name  _NET_PROPERTIES
  *
  * @{ */
static void
Set_NET_PROPERTIES(void)
{
}

/** @} */

/** @name _NET_WM_DESKTOP_MASK mask1 mask2 ... CARDINAL[]/32
  *
  * Some window managers try to compensate for the ability to place windows on
  * mutliple desktops somewhere between 1 desktop and all desktops by providing a
  * bit-mask of the desktops in the _NET_WM_DESKTOP_MASK property.  The bit-mask
  * contains as many CARD32s as are necessary to represent all of the desktops on
  * which the client window appears.  The least significant bit of the first
  * CARD32 represents the first desktop and the most significant bit of the last
  * CARD32 represents the last desktop.  A 0 in a bit position indicates that the
  * application window does not exist on that desktop; a 1 indicates that the
  * window does appear on that desktop.
  *
  * @{ */

/** @brief Set the desktop masks of a window.
  * @param window - window to set
  * @param mask - bit mask of desktops
  * @param count - number of words in bit mask
  */
static void
Set_NET_WM_DESKTOP_MASK(Window window, long *mask, int count)
{
    XChangeProperty(dpy, window, _XA_NET_WM_DESKTOP_MASK, XA_CARDINAL, 32,
		    PropModeReplace, (unsigned char *) mask, count);
}

/** @brief Get the desktop masks of a window.
  * @param window - window from which to fetch
  * @param mask - where to return the mask
  * @param count - where to returnt the count of words in the mask
  */
static Bool
Get_NET_WM_DESKTOP_MASK(Window window, long **mask, int *count)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0, len, n;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, window, _XA_NET_WM_DESKTOP_MASK, 0L, 1L, False,
			   XA_CARDINAL, &actual_type, &actual_format, &nitems,
			   &bytes_after, (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 1) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    len = ((bytes_after + 3) >> 2) + 1;
    XFree(prop);
    prop = NULL;

    status =
	XGetWindowProperty(dpy, window, _XA_NET_WM_DESKTOP_MASK, 0L, len, False,
			   XA_CARDINAL, &actual_type, &actual_format, &nitems,
			   &bytes_after, (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < len) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    if ((*mask = calloc(len + 1, sizeof(unsigned))) == NULL) {
	XFree(prop);
	return False;
    }
    for (n = 0; n < len; n++)
	(*mask)[n] = prop[n];
    (*mask)[n] = 0;
    *count = len;
    XFree(prop);
    return True;
}

/** @brief Update the desktop mask for a window.
  * @param twin - TWM window to update
  */
void
Upd_NET_WM_DESKTOP_MASK(ScreenInfo *scr, TwmWindow *twin)
{
    long *mask = NULL;
    int masks = 0;

    TwmGetWMDesktopMask(scr, twin, &mask, &masks);
    if (!twin->ewmh.props._NET_WM_DESKTOP_MASK
	|| cmp_mask_list(twin->ewmh.mask, twin->ewmh.masks, mask, masks) != 0) {
	Set_NET_WM_DESKTOP_MASK(twin->w, mask, masks);
	twin->ewmh.props._NET_WM_DESKTOP_MASK = 1;
	free(twin->ewmh.mask);
	twin->ewmh.mask = mask;
	twin->ewmh.masks = masks;
    }
}

/** @brief Retrieve the desktop mask for a window.
  * @param twin - TWM window
  *
  * Sets the desktop mask to the property or derives it from the desktop
  * property.
  */
static void
Ret_NET_WM_DESKTOP_MASK(ScreenInfo *scr, TwmWindow *twin)
{
    long *mask = NULL;
    int masks = 0;

    if (Get_NET_WM_DESKTOP_MASK(twin->w, &mask, &masks)) {
	TwmSetWMDesktopMask(scr, twin, mask, masks);
	twin->ewmh.props._NET_WM_DESKTOP_MASK = 1;
	free(twin->ewmh.mask);
	twin->ewmh.mask = mask;
	twin->ewmh.masks = masks;
    } else {
	twin->ewmh.props._NET_WM_DESKTOP_MASK = 0;
	free(twin->ewmh.mask);
	twin->ewmh.mask = NULL;
	twin->ewmh.masks = 0;
    }
    Upd_NET_WM_DESKTOP_MASK(scr, twin);
}

/** @brief Delete the desktop mask from a window.
  * @param twin - TWM window
  */
void
Del_NET_WM_DESKTOP_MASK(TwmWindow *twin)
{
    XDeleteProperty(dpy, twin->w, _XA_NET_WM_DESKTOP_MASK);
    twin->ewmh.props._NET_WM_DESKTOP_MASK = 0;
    free(twin->ewmh.mask);
    twin->ewmh.mask = NULL;
    twin->ewmh.masks = 0;
}

/** @brief Change the desktop mask for a window.
  * @param scr - screen
  * @param twin - TWM window
  * @param event - client message changing the desktop mask
  */
static void
Rcv_NET_WM_DESKTOP_MASK(ScreenInfo *scr, TwmWindow *twin, XClientMessageEvent *event)
{
    unsigned index = event->data.l[0];
    unsigned mask = event->data.l[1];

    TwmChgWMDesktopMask(scr, twin, index, mask);
    Upd_NET_WM_DESKTOP_MASK(scr, twin);
    Upd_NET_WM_DESKTOP(scr, twin);
    Upd_NET_WM_STATE(twin);	/* for sticky state */
}

/** @} */

/** @name _NET_VIRTUAL_POS
  *
  * @{ */
static void
Set_NET_VIRTUAL_POS(Window window, struct NetPosition *virtual_pos)
{
    XChangeProperty(dpy, window, _XA_NET_VIRTUAL_POS, XA_CARDINAL, 32, PropModeReplace,
		    (unsigned char *) virtual_pos, 2);
}

static Bool
Get_NET_VIRTUAL_POS(Window window, struct NetPosition *virtual_pos)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, window, _XA_NET_VIRTUAL_POS, 0L, 2L, False, XA_CARDINAL,
			   &actual_type, &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 2) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    *virtual_pos = *(struct NetPosition *) prop;
    XFree(prop);
    return True;
}

/** @brief Update the virtual position of a window.
  * @param twin - TWM window
  *
  * The virtual position of a window is its position on the desktop rather than
  * its position on the root window of the current screen.  Waimea uses this for
  * some reason.  The property is only updated when the position actually
  * changes.
  */
void
Upd_NET_VIRTUAL_POS(TwmWindow *twin)
{
    struct NetPosition virtual_pos = { 0, 0 };

    TwmGetWMVirtualPos(twin, &virtual_pos);
    if (!twin->ewmh.props._NET_VIRTUAL_POS
	|| cmp_pos(&twin->ewmh.virtual_pos, &virtual_pos) != 0) {
	Set_NET_VIRTUAL_POS(twin->w, &virtual_pos);
	twin->ewmh.props._NET_VIRTUAL_POS = 1;
	twin->ewmh.virtual_pos = virtual_pos;
    }
}

static void
Ret_NET_VIRTUAL_POS(TwmWindow *twin)
{
    struct NetPosition virtual_pos = { 0, 0 };

    if (Get_NET_VIRTUAL_POS(twin->w, &virtual_pos)) {
	TwmSetWMVirtualPos(twin, &virtual_pos);
	twin->ewmh.props._NET_VIRTUAL_POS = 1;
	twin->ewmh.virtual_pos = virtual_pos;
    } else {
	twin->ewmh.props._NET_VIRTUAL_POS = 0;
	twin->ewmh.virtual_pos.x = 0;
	twin->ewmh.virtual_pos.y = 0;
    }
    Upd_NET_VIRTUAL_POS(twin);
}

static void
Del_NET_VIRTUAL_POS(TwmWindow *twin)
{
    XDeleteProperty(dpy, twin->w, _XA_NET_VIRTUAL_POS);
    twin->ewmh.props._NET_VIRTUAL_POS = 0;
    twin->ewmh.virtual_pos.x = 0;
    twin->ewmh.virtual_pos.y = 0;
}

/** @} */

/*
 * Also provided is a client message for applications to request that the WM
 * alter the desktops on which the application window appears.  Two arguments
 * are provided: the index of the bit mask to replace and the CARD32 of the
 * bit-mask to replace at the index.  Typically the index is 0 and the bit-mask
 * represents the occupancy of the application window on the first 32 desktops
 * (numbered 0 through 31).
 */

SIGNAL_T Restart(int signum);
SIGNAL_T Done(int signum);

/** @name _NET_RESTART command UTF8_STRING[]/8
  *
  * Client message that asks the window manager to restart.   Coupled with a
  * property on the root window that conntains the command an arguments of a
  * replacement window mwnager when present.  To simply restart the window
  * manager, the property should be absent.
  *
  * @{ */
static void
Set_NET_RESTART(void)
{
}

static Bool
Get_NET_RESTART(void)
{
    return False;
}

static void
Ret_NET_RESTART(void)
{
}

static void
Rcv_NET_RESTART(ScreenInfo *scr, XClientMessageEvent *event)
{
    Restart(0);
}

/** @} */

/** @name _NET_SHUTDOWN
  *
  * Client message that asks the window manager to shut down.
  *
  * @{ */
static void
Rcv_NET_SHUTDOWN(ScreenInfo *scr, XClientMessageEvent *event)
{
    Done(0);
}

/** @} */

/** @} */

/** @name Initialization and termination of the window manager.
  *
  * @{ */

/** @brief Initialize the window manager in the NetWM/EWMH sense.
  *
  * This function is called when TWM starts to initialize root window properties
  * and screen information on initialization of the window manager.
  */
void
InitEwmh(ScreenInfo *scr)
{
    char *cp, *name = Argv[0];
    char layout[32];

    InternKdeAtoms();
    InternNetAtoms();

#ifdef DEBUG_EWMH
    fprintf(stderr, "Initializing NetWM/EWMH support.\n");
    fflush(stderr);
#endif

#ifdef DEBUG_EWMH
    fprintf(stderr, "Updating _NET_SUPPORTED\n");
    fflush(stderr);
#endif
    Upd_NET_SUPPORTED(scr);

#if 0
    if ((cp = strrchr(name, '/')))
	name = cp + 1;
#else
    name = PACKAGE_STRING;
#endif

#ifdef DEBUG_EWMH
    fprintf(stderr, "Setting _NET_WM_NAME on check window\n");
    fflush(stderr);
#endif
    Set_NET_WM_NAME(scr->ManagerWindow, name);
#ifdef DEBUG_EWMH
    fprintf(stderr, "Setting _NET_WM_PID on check window\n");
    fflush(stderr);
#endif
    Set_NET_WM_PID(scr->ManagerWindow, getpid());

#ifdef DEBUG_EWMH
    fprintf(stderr, "Retrieve or set _NET_NUMBER_OF_DESKTOPS on root\n");
    fflush(stderr);
#endif
    Ret_NET_NUMBER_OF_DESKTOPS(scr);
#ifdef DEBUG_EWMH
    fprintf(stderr, "Retrieve or set _NET_DESKTOP_GEOMETRY on root\n");
    fflush(stderr);
#endif
    Ret_NET_DESKTOP_GEOMETRY(scr);
#ifdef DEBUG_EWMH
    fprintf(stderr, "Retrieve or set _NET_DESKTOP_VIEWPORT on root\n");
    fflush(stderr);
#endif
    Ret_NET_DESKTOP_VIEWPORT(scr);
#ifdef DEBUG_EWMH
    fprintf(stderr, "Retrieve or set _NET_CURRENT_DESKTOP on root\n");
    fflush(stderr);
#endif
    Ret_NET_CURRENT_DESKTOP(scr);
#if 0
#ifdef DEBUG_EWMH
    fprintf(stderr, "Retrieve or set _NET_DESKTOP_NAMES on root\n");
    fflush(stderr);
#endif
    Ret_NET_DESKTOP_NAMES(scr);
#endif
#ifdef DEBUG_EWMH
    fprintf(stderr, "Retrieve or set _NET_ACTIVE_WINDOW on root\n");
    fflush(stderr);
#endif
    Ret_NET_ACTIVE_WINDOW(scr);
#ifdef DEBUG_EWMH
    fprintf(stderr, "Retrieve or set _NET_DESKTOP_LAYOUT on root\n");
    fflush(stderr);
#endif
    Ret_NET_DESKTOP_LAYOUT(scr);
#ifdef DEBUG_EWMH
    fprintf(stderr, "Retrieve or set _NET_SHOWING_DESKTOP on root\n");
    fflush(stderr);
#endif
    Ret_NET_SHOWING_DESKTOP(scr);

    Set_NET_SUPPORTING_WM_CHECK(scr->Root, scr->ManagerWindow);

    /* get the _NET_WM_DESKTOP_LAYOUT_Sn selection */
    snprintf(layout, 32, "_NET_DESKTOP_LAYOUT_S%d", scr->screen);
    scr->ewmh.layout_sn.atom = XInternAtom(dpy, layout, False);
    XGrabServer(dpy);
    scr->ewmh.layout_sn.owner = XGetSelectionOwner(dpy, scr->ewmh.layout_sn.atom);
    if (scr->ewmh.layout_sn.owner == None) {
	scr->ewmh.layout_sn.window =
	    XCreateSimpleWindow(dpy, scr->Root, scr->rootw, scr->rooth, 1, 1, 0, 0L, 0L);
	XSetSelectionOwner(dpy, scr->ewmh.layout_sn.atom, scr->ewmh.layout_sn.window,
			   CurrentTime);
    } else {
	scr->ewmh.layout_sn.window = None;
	XSelectInput(dpy, scr->ewmh.layout_sn.owner, StructureNotifyMask);
	XSaveContext(dpy, scr->ewmh.layout_sn.owner, ScreenContext, (XPointer) scr);
    }
    XSync(dpy, False);
    XUngrabServer(dpy);
}

/** @brief Update the window manager in the NetWM/WMH sense.
  */
void
UpdateEwmh(ScreenInfo *scr)
{
#ifdef DEBUG_EWMH
    fprintf(stderr, "Updating _NET_CLIENT_LIST on root\n");
    fflush(stderr);
#endif
    Upd_NET_CLIENT_LIST(scr);
#ifdef DEBUG_EWMH
    fprintf(stderr, "Updating _NET_CLIENT_LIST_STACKING on root\n");
    fflush(stderr);
#endif
    Upd_NET_CLIENT_LIST_STACKING(scr);
#ifdef DEBUG_EWMH
    fprintf(stderr, "Updating _NET_NUMBER_OF_DESKTOPS on root\n");
    fflush(stderr);
#endif
    Upd_NET_NUMBER_OF_DESKTOPS(scr);
#ifdef DEBUG_EWMH
    fprintf(stderr, "Updating _NET_DESKTOP_GEOMETRY on root\n");
    fflush(stderr);
#endif
    Upd_NET_DESKTOP_GEOMETRY(scr);
#ifdef DEBUG_EWMH
    fprintf(stderr, "Updating _NET_DESKTOP_VIEWPORT on root\n");
    fflush(stderr);
#endif
    Upd_NET_DESKTOP_VIEWPORT(scr);
#ifdef DEBUG_EWMH
    fprintf(stderr, "Updating _NET_CURRENT_DESKTOP on root\n");
    fflush(stderr);
#endif
    Upd_NET_CURRENT_DESKTOP(scr);
#if 0
#ifdef DEBUG_EWMH
    fprintf(stderr, "Updating _NET_DESKTOP_NAMES on root\n");
    fflush(stderr);
#endif
    Upd_NET_DESKTOP_NAMES(scr);
#endif
#ifdef DEBUG_EWMH
    fprintf(stderr, "Updating _NET_ACTIVE_WINDOW on root\n");
    fflush(stderr);
#endif
    Upd_NET_ACTIVE_WINDOW(scr);
#ifdef DEBUG_EWMH
    fprintf(stderr, "Updating _NET_WORKAREA on root\n");
    fflush(stderr);
#endif
    Upd_NET_WORKAREA(scr);
#ifdef DEBUG_EWMH
    fprintf(stderr, "Updating _NET_VIRTUAL_ROOTS on root\n");
    fflush(stderr);
#endif
    Upd_NET_VIRTUAL_ROOTS(scr);
#ifdef DEBUG_EWMH
    fprintf(stderr, "Updating _NET_DESKTOP_LAYOUT on root\n");
    fflush(stderr);
#endif
    Upd_NET_DESKTOP_LAYOUT(scr);
#ifdef DEBUG_EWMH
    fprintf(stderr, "Updating _NET_SHOWING_DESKTOP on root\n");
    fflush(stderr);
#endif
    Upd_NET_SHOWING_DESKTOP(scr);
}

/** @brief Terminate the window manager in the NetWM/WMH sense.
  *
  * Note that this is for exiting or starting another window manager.
  * WM controlled properties are removed so that if the next window manager is
  * not NetWM/EWMH compliant, taskbars, pagers and desktops are not confused,
  * even though _NET_WM_SUPPORTING_WM_CHECK handles this nicely.
  */
void
TermEwmh(ScreenInfo *scr)
{
    Del_NET_SUPPORTING_WM_CHECK(scr);	/* do this first */
    Del_NET_SUPPORTED(scr);
    Del_NET_CLIENT_LIST(scr);
    Del_NET_CLIENT_LIST_STACKING(scr);
    // Del_NET_NUMBER_OF_DESKTOPS(scr);
    // Del_NET_DESKTOP_GEOMETRY(scr);
    // Del_NET_DESKTOP_VIEWPORT(scr);
    // Del_NET_CURRENT_DESKTOP(scr);
    // Del_NET_DESKTOP_NAMES(scr);
    // Del_NET_ACTIVE_WINDOW(scr);
    Del_NET_WORKAREA(scr);
    Del_NET_VIRTUAL_ROOTS(scr);
    // Del_NET_DESKTOP_LAYOUT(scr);
    // Del_NET_SHOWING_DESKTOP(scr);
    // Del_NET_VIRTUAL_POS(scr);
}

/** @} */

/** @name Addition and Withdrawal of windows.
  *
  * CTWM treats withdrawal and deletion of windows identically.
  *
  * @{ */

/** @brief Add a window in the EWMH sense.
  * @param twin - the window to add
  *
  * This funciton is used to add a newly managed window in the NetWM/EWMH sens
  * in reponse to a MapRequest of a managed window.  The entire TwmWindow
  * structure has been completed and we only need to add NetWM/WMH stuff.
  */
void
AddWindowEwmh(ScreenInfo *scr, TwmWindow *twin)
{
    Ret_WM_CLASS(twin);
    Ret_WM_PROTOCOLS(twin);
    Ret_WM_CLIENT_MACHINE(twin);
    Ret_WM_COMMAND(twin);
    Ret_NET_WM_PID(scr, twin);
    Ret_NET_WM_USER_TIME_WINDOW(scr, twin);
    Ret_NET_WM_USER_TIME(twin);
    Ret_NET_STARTUP_ID(scr, twin);

    Ret_NET_WM_DESKTOP(scr, twin);
    Ret_NET_WM_DESKTOP_MASK(scr, twin);

    Ret_NET_WM_NAME(twin);
    Upd_NET_WM_VISIBLE_NAME(twin);
    Ret_NET_WM_ICON_NAME(twin);
    Upd_NET_WM_VISIBLE_ICON_NAME(twin);
    Ret_NET_WM_WINDOW_TYPE(twin);
    Ret_NET_WM_STATE(twin);
    Upd_NET_WM_ALLOWED_ACTIONS(twin);
    Ret_NET_WM_STRUT_PARTIAL(scr, twin);
    Ret_NET_WM_ICON_GEOMETRY(twin);
    Ret_NET_WM_ICON(twin);
    Ret_NET_WM_HANDLED_ICONS(scr, twin);
    Upd_NET_FRAME_EXTENTS(twin);
    // Ret_NET_WM_OPAQUE_REGION(twin);
    // Ret_NET_WM_BYPASS_COMPOSITOR(twin);
    Ret_NET_WM_FULLSCREEN_MONITORS(scr, twin);
    // Ret_NET_WM_WINDOW_OPACITY(twin);
    Ret_NET_VIRTUAL_POS(twin);

    Upd_NET_CLIENT_LIST(scr);
    Upd_NET_CLIENT_LIST_STACKING(scr);
}

/** @brief Update a window in the EWMH sense.
  * @param twin - the window to update
  *
  * This function is used when a change may have occured in TWM that affects
  * this window's properties but it is not known which properties are affected,
  * nor whether a change has actually occured.
  */
void
UpdateWindowEwmh(ScreenInfo *scr, TwmWindow *twin)
{
    Upd_NET_WM_VISIBLE_NAME(twin);
    Upd_NET_WM_VISIBLE_ICON_NAME(twin);
    Upd_NET_WM_DESKTOP(scr, twin);
    Upd_NET_WM_STATE(twin);
    Upd_NET_WM_ALLOWED_ACTIONS(twin);
    Upd_NET_FRAME_EXTENTS(twin);
    Upd_NET_WM_DESKTOP_MASK(scr, twin);
    Upd_NET_VIRTUAL_POS(twin);
    Upd_NET_MAXIMIZED_RESTORE(twin);

    Upd_NET_CLIENT_LIST(scr);
    Upd_NET_CLIENT_LIST_STACKING(scr);
}

/** @brief Withdraw a window in the EWMH sense.
  * @param twin - the window to delete
  *
  * This function is used to delete a managed window in the NetWM/EWMH sense in
  * reponse to an UnmapRequest or DestroyNotify for a managed window.  The
  * TwmWindow structure has not yet been removed or prepared for deallocation:
  * we need to delete the NetWM/EWMH stuff first.
  */
void
DelWindowEwmh(ScreenInfo *scr, TwmWindow *twin)
{
    EwmhWindow *ewin;

    /* When a window is being withdrawn, remove a rash of NetWM/EWMH hints that
       are supposed to be removed on withdrawal. */
    Del_NET_WM_VISIBLE_NAME(twin);
    Del_NET_WM_VISIBLE_ICON_NAME(twin);
    Del_NET_WM_DESKTOP(twin);
    Del_NET_WM_STATE(twin);
    Del_NET_WM_ALLOWED_ACTIONS(twin);
    Del_NET_FRAME_EXTENTS(twin);
    Del_NET_WM_DESKTOP_MASK(twin);
    Del_NET_VIRTUAL_POS(twin);
    Del_NET_MAXIMIZED_RESTORE(twin);
    Del_NET_WM_USER_TIME_WINDOW(scr, twin);
    Del_NET_STARTUP_ID(twin);

    Upd_NET_CLIENT_LIST(scr);
    Upd_NET_CLIENT_LIST_STACKING(scr);

    /* free various EwmhWindow allocations */
    ewin = &twin->ewmh;

    free(ewin->class.res_name);
    free(ewin->class.res_class);
    free(ewin->machine);
    if (ewin->command != NULL)
	XFreeStringList(ewin->command);
    free(ewin->name);
    free(ewin->visible_name);
    free(ewin->icon_name);
    free(ewin->visible_icon_name);
    free(ewin->startup_id);
    release_seq(ewin->sequence);
    free(ewin->mask);
}

/** @} */

/** @name Handling of NetWM/EWMH events.
  *
  * These event handlers must be hooked into the corresponing 'Handle*' event
  * handler in the TWM variant.
  *
  * @{ */

/*
 * Handle the receipt of client messages.  Returns True when the event has
 * been handled, False otherwise.
 *
 * Client messages to which NetWM/EWMH window managers repond are normally of a
 * message type atom specified by the NetWM/EWMH specification.  There are two
 * exceptions: WM_PROTOCOLS, which is used for the _NET_WM_PING protocol; and
 * MANAGER, which is used for indicating the new owner of a
 * _NET_DESKTOP_LAYOUT_Sn selection.
 */
Bool
HandleNetClientMessage(ScreenInfo *scr, TwmWindow *twin, XEvent *xev)
{
    XClientMessageEvent *event = &xev->xclient;
    Atom message_type = event->message_type;

    if (scr == NULL)
	return False;
    if (event->format != 32)
	return False;
    if (twin != NULL) {
	/* these messages have event->window == twin->w */
	if (0) {
	} else if (message_type == _XA_NET_CLOSE_WINDOW) {
	    Rcv_NET_CLOSE_WINDOW(twin, event);
	} else if (message_type == _XA_NET_MOVERESIZE_WINDOW) {
	    Rcv_NET_MOVERESIZE_WINDOW(twin, event);
	} else if (message_type == _XA_NET_WM_MOVERESIZE) {
	    Rcv_NET_WM_MOVERESIZE(twin, event);
	} else if (message_type == _XA_NET_RESTACK_WINDOW) {
	    Rcv_NET_RESTACK_WINDOW(twin, event);
	} else if (message_type == _XA_NET_WM_DESKTOP) {
	    Rcv_NET_WM_DESKTOP(scr, twin, event);
	} else if (message_type == _XA_NET_WM_DESKTOP_MASK) {
	    Rcv_NET_WM_DESKTOP_MASK(scr, twin, event);
	} else if (message_type == _XA_NET_WM_STATE) {
	    Rcv_NET_WM_STATE(scr, twin, event);
	} else if (message_type == _XA_WM_PROTOCOLS) {
	    if (event->data.l[0] == _XA_NET_WM_PING)
		Rcv_NET_WM_PING(twin, event);
	    else
		return False;
	} else if (message_type == _XA_NET_WM_FULLSCREEN_MONITORS) {
	    Rcv_NET_WM_FULLSCREEN_MONITORS(scr, twin, event);
	} else
	    return False;
    } else {
	if (0) {
	} else if (message_type == _XA_NET_NUMBER_OF_DESKTOPS) {
	    Rcv_NET_NUMBER_OF_DESKTOPS(scr, event);
	} else if (message_type == _XA_NET_DESKTOP_GEOMETRY) {
	    Rcv_NET_DESKTOP_GEOMETRY(scr, event);
	} else if (message_type == _XA_NET_DESKTOP_VIEWPORT) {
	    Rcv_NET_DESKTOP_VIEWPORT(scr, event);
	} else if (message_type == _XA_NET_CURRENT_DESKTOP) {
	    Rcv_NET_CURRENT_DESKTOP(scr, event);
	} else if (message_type == _XA_NET_ACTIVE_WINDOW) {
	    Rcv_NET_ACTIVE_WINDOW(scr, event);
	} else if (message_type == _XA_NET_SHOWING_DESKTOP) {
	    Rcv_NET_SHOWING_DESKTOP(scr, event);
	} else if (message_type == _XA_NET_RESTART) {
	    Rcv_NET_RESTART(scr, event);
	} else if (message_type == _XA_NET_SHUTDOWN) {
	    Rcv_NET_SHUTDOWN(scr, event);
	} else if (message_type == _XA_MANAGER) {
	    if (event->data.l[1] == scr->ewmh.layout_sn.atom) {
		New_NET_DESKTOP_LAYOUT_Sn(scr, event);
	    } else
		return False;
	} else if (message_type == _XA_NET_REQUEST_FRAME_EXTENTS) {
	    Rcv_NET_REQUEST_FRAME_EXTENTS(scr, event);
	} else if (message_type == _XA_NET_STARTUP_INFO_BEGIN
		   || message_type == _XA_NET_STARTUP_INFO) {
	    EwmhNotify *notify = NULL;

	    XFindContext(dpy, event->window, NotifyContext, (XPointer *) &notify);
	    if (0) {
	    } else if (message_type == _XA_NET_STARTUP_INFO_BEGIN) {
		Rcv_NET_STARTUP_INFO_BEGIN(scr, event, notify);
	    } else if (message_type == _XA_NET_STARTUP_INFO) {
		Rcv_NET_STARTUP_INFO(scr, event, notify);
	    } else
		return False;
	} else
	    return False;
    }
    return True;
}

/** @brief Handle property change notifications.
  *
  * Returns True when the event has been handled, False otherwise.
  *
  * NetWM/EWMH specifications normally do not respond to property change
  * notifications on any of the properties that it uses: client messages are
  * normally defined instead.  There are; however, several exceptions:
  * _NET_DESKTOP_NAMES, _NET_WM_NAME, _NET_WM_ICON_NAME, because client messages
  * are too short to convey long string information; _NET_DESKTOP_LAYOUT,
  * because only the owner of the _NET_DESKTOP_LAYOUT_Sn selection can change
  * this property.  All other property changes are ignored.
  */
Bool
HandleNetPropertyNotify(ScreenInfo *scr, TwmWindow *twin, XEvent *xev)
{
    XPropertyEvent *event = &xev->xproperty;
    Atom atom = event->atom;

    if (scr == NULL)
	return False;
    if (twin != NULL) {
	if (0) {
	} else if (atom == _XA_NET_WM_NAME || atom == XA_WM_NAME) {
	    Ret_NET_WM_NAME(twin);
	} else if (atom == _XA_NET_WM_ICON_NAME || atom == XA_WM_ICON_NAME) {
	    Ret_NET_WM_ICON_NAME(twin);
	} else if (atom == _XA_NET_WM_STRUT || atom == _XA_NET_WM_STRUT_PARTIAL) {
	    Ret_NET_WM_STRUT_PARTIAL(scr, twin);
	} else if (atom == _XA_NET_WM_ICON) {
	    Ret_NET_WM_ICON(twin);
	} else if (atom == _XA_NET_WM_ICON_GEOMETRY) {
	    Ret_NET_WM_ICON_GEOMETRY(twin);
	} else if (atom == _XA_NET_WM_USER_TIME) {
	    Ret_NET_WM_USER_TIME(twin);	/* do we really want to do this? */
	} else if (atom == _XA_NET_WM_SYNC_REQUEST_COUNTER) {
	    Ret_NET_WM_SYNC_REQUEST_COUNTER(twin);
	} else
	    return False;
    } else {
	if (0) {
	} else if (atom == _XA_NET_DESKTOP_NAMES) {
	    Ret_NET_DESKTOP_NAMES(scr);
	} else if (atom == _XA_NET_DESKTOP_LAYOUT) {
	    Ret_NET_DESKTOP_LAYOUT(scr);
	} else
	    return False;
    }
    return True;
}

/** @brief Handle selection clear events.
  *
  * Returns True when the event has been handled, False otherwise.
  *
  * The only selection clear event that we need to handle is the clearing of a
  * desktop layout selection owner (which is us losing the selection).
  */
Bool
HandleNetSelectionClear(ScreenInfo *scr, TwmWindow *twin, XEvent *xev)
{
    XSelectionClearEvent *event = &xev->xselectionclear;
    Atom atom = event->selection;

    if (scr == NULL)
	return False;
    if (0) {
    } else if (atom == scr->ewmh.layout_sn.atom) {
	Clr_NET_DESKTOP_LAYOUT_Sn(scr, event);
    } else
	return False;
    return True;
}

/** @brief Handle destroy window events.
  *
  * Returns True when the event has been handled, False otherwise.
  */
Bool
HandleNetDestroyNotify(ScreenInfo *scr, TwmWindow *twin, XEvent *xev)
{
    XDestroyWindowEvent *event = &xev->xdestroywindow;
    Window window = event->window;

    if (scr == NULL)
	return False;
    if (0) {
    } else if (window == scr->ewmh.layout_sn.owner) {
	Rel_NET_DESKTOP_LAYOUT_Sn(scr, event);
    } else {
	EwmhNotify *notify = NULL;

	if (XFindContext(dpy, window, NotifyContext, (XPointer *) &notify) == XCSUCCESS)
	    Rel_NET_STARTUP_INFO(scr, notify);
	else
	    return False;
    }
    return True;
}

/** @} */

// vim: set sw=4 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
