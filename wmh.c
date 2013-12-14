#include <stdio.h>
#include "wmh.h"
#include "screen.h"
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/Xmu/CharSet.h>

/** @file
  * WinWM/WMH Implementation File
  *
  * This file contains a generic implementation of the GNOME WinWM/WMH
  * specification.
  *
  * Function naming conventions are as follows:
  *
  * - Set_*	sets the root or window property
  * - Get_*	gets the root or window property
  * - Upd_*	updates the root or window property by setting it to its current
  *		value according to the window manager
  * - Ret_*	retrieves the root or window property and synchronizes the
  *		setting with the window manager
  * - Del_*	deltes the property from the root or window
  * - Rcv_*	processes a client message
  * - Snd_*	sends a client message
  */

/* 1.1 Detection of GNOME compliant window manager */
Atom _XA_WIN_SUPPORTING_WM_CHECK;

/* 1.2 Listing GNOME window manager compliance */
Atom _XA_WIN_PROTOCOLS;

/* 1.3 Providing shortcuts managed clients */
Atom _XA_WIN_CLIENT_LIST;

/* 1.4 Providing multiple/virtual desktop information */
Atom _XA_WIN_WORKSPACE_COUNT;
Atom _XA_WIN_WORKSPACE;
Atom _XA_WIN_WORKSPACE_NAMES;

/* 2.1 Initial properties set on client window */
Atom _XA_WIN_LAYER;
Atom _XA_WIN_STATE;
extern Atom _XA_WIN_WORKSPACE;
Atom _XA_WIN_EXPANDED_SIZE;
Atom _XA_WIN_HINTS;

/* 2.2 State change requests */
extern Atom _XA_WIN_LAYER;
extern Atom _XA_WIN_STATE;
extern Atom _XA_WIN_WORKSPACE;

/* 3.1 Button press and release forwarding for the desktop window. */
Atom _XA_WIN_DESKTOP_BUTTON_PROXY;

/* 3.2 Desktop areas as opposed to multiple desktops. */
Atom _XA_WIN_AREA_COUNT;
Atom _XA_WIN_AREA;

/* mentioned by not defined by wm-comp */
Atom _XA_WIN_APP_STATE;
Atom _XA_WIN_ICONS;

/* not mentioned by wm-comp but used by window managers, e.g. icewm */
Atom _XA_WIN_WORKAREA;
Atom _XA_WIN_WORKSPACES;
Atom _XA_WIN_CLIENT_MOVING;

/* others */
Atom _XA_WIN_MAXIMIZED_GEOMETRY;
Atom _XA_WIN_RESIZE;
Atom _XA_WIN_FOCUS;

typedef struct WinAtom {
    Atom *atom_p;
    const char *name;
} WinAtom;

static const struct WinAtom WinAtoms[] = {
    {&_XA_WIN_SUPPORTING_WM_CHECK, "_WIN_SUPPORTING_WM_CHECK"},
    {&_XA_WIN_PROTOCOLS, "_WIN_PROTOCOLS"},
    {&_XA_WIN_CLIENT_LIST, "_WIN_CLIENT_LIST"},
    {&_XA_WIN_WORKSPACE_COUNT, "_WIN_WORKSPACE_COUNT"},
    {&_XA_WIN_WORKSPACE, "_WIN_WORKSPACE"},
    {&_XA_WIN_WORKSPACE_NAMES, "_WIN_WORKSPACE_NAMES"},
    {&_XA_WIN_LAYER, "_WIN_LAYER"},
    {&_XA_WIN_STATE, "_WIN_STATE"},
    {&_XA_WIN_EXPANDED_SIZE, "_WIN_EXPANDED_SIZE"},
    {&_XA_WIN_HINTS, "_WIN_HINTS"},
    {&_XA_WIN_DESKTOP_BUTTON_PROXY, "_WIN_DESKTOP_BUTTON_PROXY"},
    {&_XA_WIN_AREA_COUNT, "_WIN_AREA_COUNT"},
    {&_XA_WIN_AREA, "_WIN_AREA"},
    {&_XA_WIN_APP_STATE, "_WIN_APP_STATE"},
    {&_XA_WIN_ICONS, "_WIN_ICONS"},
    {&_XA_WIN_WORKAREA, "_WIN_WORKAREA"},
    {&_XA_WIN_WORKSPACES, "_WIN_WORKSPACES"},
    {&_XA_WIN_CLIENT_MOVING, "_WIN_CLIENT_MOVING"},
    {&_XA_WIN_MAXIMIZED_GEOMETRY, "_WIN_MAXIMIZED_GEOMETRY"},
    {&_XA_WIN_RESIZE, "_WIN_RESIZE"},
    {&_XA_WIN_FOCUS, "_WIN_FOCUS"},
    {NULL, NULL}
};

void
InternWinAtoms(void)
{
    const struct WinAtom *p;

    for (p = WinAtoms; p->atom_p != NULL; p++)
	*(p->atom_p) = XInternAtom(dpy, p->name, False);
}

#if 0
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
#endif

#if 0
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
#endif

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
cmp_geo(struct WinGeometry *g1, struct WinGeometry *g2)
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
cmp_layout(struct WinLayout *l1, struct WinLayout *l2)
{
    if (l1->cols < l2->cols)
	return (-1);
    if (l1->cols > l2->cols)
	return (1);
    if (l1->rows < l2->rows)
	return (-1);
    if (l1->rows > l2->rows)
	return (1);
    return (0);
}

static inline int
cmp_area(struct WinArea *a1, struct WinArea *a2)
{
    if (a1->col < a2->col)
	return (-1);
    if (a1->col > a2->col)
	return (1);
    if (a1->row < a2->row)
	return (-1);
    if (a1->row > a2->row)
	return (1);
    return (0);
}

static inline int
cmp_workarea(struct WinWorkarea *w1, struct WinWorkarea *w2)
{
    if (w1->x_min < w2->x_min)
	return (-1);
    if (w1->x_min > w2->x_min)
	return (1);
    if (w1->y_min < w2->y_min)
	return (-1);
    if (w1->y_min > w2->y_min)
	return (1);
    if (w1->x_max < w2->x_max)
	return (-1);
    if (w1->x_max > w2->x_max)
	return (1);
    if (w1->y_max < w2->y_max)
	return (-1);
    if (w1->y_max > w2->y_max)
	return (1);
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

/** @name _WIN_SUPPORTING_WM_CHECK, CARDINAL/32
  *
  * There is a single unambiguous way to detect if there currently is a GNOME
  * compliant Window Manager running.  It is the job of the Window Manager to
  * set up a few things to make this possible.  Using the following method it is
  * also possible for applications to detect compliance by receiving an event
  * when the Window Manager exits.
  *
  * To do this the Window Manager should create a Window, that is a child of the
  * root window.  There is no need to map it, just create it.  The Window
  * Manager may reuse ANY window it has for this purpose - even if it is mapped,
  * just as long as the window is never destroyed while the Window Manager is
  * running.
  *
  * Once the Window is created the Window Manager should set a property on the
  * root window of the name _WIN_SUPPORTING_WM_CHECK, and type CARDINAL.  The
  * atom's data would be a CARDINAL that is the Window ID of the window that was
  * created above.  The window that was created would ALSO have this property
  * set on it with the same values and type.
  *
  * @{ */

/** @brief Set the supporting window manager check window.
  * @param window - the check window
  *
  * Sets the _WIN_SUPPORTING_WM_CHECK property on the check window, #window, and
  * then sets the property on the root window.
  */
static void
Set_WIN_SUPPORTING_WM_CHECK(Window root, Window window)
{
    long data = window;

    XChangeProperty(dpy, window, _XA_WIN_SUPPORTING_WM_CHECK, XA_CARDINAL, 32,
		    PropModeReplace, (unsigned char *) &data, 1);
    XChangeProperty(dpy, root, _XA_WIN_SUPPORTING_WM_CHECK, XA_CARDINAL, 32,
		    PropModeReplace, (unsigned char *) &data, 1);
}

static void
Ini_WIN_SUPPORTING_WM_CHECK(ScreenInfo *scr)
{
    Window check = TwmWinManager(scr);

    Set_WIN_SUPPORTING_WM_CHECK(TwmWinRoot(scr), check);
    scr->wmh.props._WIN_SUPPORTING_WM_CHECK = 1;
    scr->wmh.check = check;
}

/** @brief Update the supporting window manager check window property.
  * @param scr - screen
  *
  * The property is not updated unless the check window actually changes.
  */
void
Upd_WIN_SUPPORTING_WM_CHECK(ScreenInfo *scr)
{
    Window check = TwmWinManager(scr);

    if (!scr->wmh.props._WIN_SUPPORTING_WM_CHECK || scr->wmh.check != check) {
	Set_WIN_SUPPORTING_WM_CHECK(TwmWinRoot(scr), check);
	scr->wmh.props._WIN_SUPPORTING_WM_CHECK = 1;
	scr->wmh.check = check;
    }
}

/** @brief Delete the supporting window manager check window property.
  */
static void
Del_WIN_SUPPORTING_WM_CHECK(ScreenInfo *scr)
{
    XDeleteProperty(dpy, TwmWinRoot(scr), _XA_WIN_SUPPORTING_WM_CHECK);
    scr->wmh.props._WIN_SUPPORTING_WM_CHECK = 0;
    scr->wmh.check = None;
}

/** @} */

/** @name _WIN_PROTOCOLS, ATOM[]/32
  *
  * Create a property on the root window of the atom name _WIN_PROTOCOLS.
  * This property contains a list (array) of atoms that are all the properties
  * the Window Manager supports.  These atoms are any number of the following:
  *
  * _WIN_LAYER
  * _WIN_STATE
  * _WIN_HINTS
  * _WIN_APP_STATE
  * _WIN_EXPANDED_SIZE
  * _WIN_ICONS
  * _WIN_WORKSPACE
  * _WIN_WORKSPACE_COUNT
  * _WIN_WORKSPACE_NAMES
  * _WIN_CLIENT_LIST
  *
  * If you list one of these properties then you support it and applications can
  * expect information provided by, or accepted by the Window Manager to work.
  *
  * IceWM says that this property contains ALL of the protocols/hints supported
  * by the window manager, including WM_HINTS, MWM_HINTS, WIN_*, etc.
  *
  * @{ */
/** @brief Set the supported WinWM/WMH protocols.
  */
static void
Set_WIN_PROTOCOLS(Window root)
{
    long data[] = {
	_XA_WIN_SUPPORTING_WM_CHECK,
	_XA_WIN_PROTOCOLS,
	_XA_WIN_CLIENT_LIST,
	_XA_WIN_WORKSPACE_COUNT,
	_XA_WIN_WORKSPACE,
	_XA_WIN_WORKSPACE_NAMES,
	_XA_WIN_LAYER,
	_XA_WIN_STATE,
	_XA_WIN_EXPANDED_SIZE,
	_XA_WIN_HINTS,
	_XA_WIN_DESKTOP_BUTTON_PROXY,
	_XA_WIN_AREA_COUNT,
	_XA_WIN_AREA,
	_XA_WIN_APP_STATE,
	_XA_WIN_ICONS,
	_XA_WIN_WORKAREA,
	_XA_WIN_WORKSPACES,
	_XA_KDE_WM_CHANGE_STATE,
	_XA_KDE_SPLASH_PROGRESS,
	_XA_KWM_WIN_ICON,
	_XA_KWM_DOCKWINDOW,
	_XA_MOTIF_WM_HINTS,
	_XA_MOTIF_WM_INFO,
	_XA__SWM_VROOT
#if 0
	    ,
	_XA_MOTIF_DRAG_RECEIVER_INFO,
	XA_WM_COMMAND,
	XA_WM_HINTS,
	XA_WM_CLIENT_MACHINE,
	XA_WM_ICON_NAME,
	XA_WM_ICON_SIZE,
	XA_WM_NAME,
	XA_WM_NORMAL_HINTS,
	XA_WM_SIZE_HINTS,
	XA_WM_ZOOM_HINTS,
	XA_WM_CLASS,
	XA_WM_TRANSIENT_FOR,
	_XA_WM_CLIENT_LEADER,
	_XA_WM_DELETE_WINDOW,
	_XA_WM_LOCALE_NAME,
	_XA_WM_PROTOCOLS,
	_XA_WM_TAKE_FOCUS,
	_XA_WM_WINDOW_ROLE,
	_XA_WM_STATE,
	_XA_WM_CHANGE_STATE,
	_XA_WM_SAVE_YOURSELF,
	_XA_SM_CLIENT_ID
#endif
    };
    XChangeProperty(dpy, root, _XA_WIN_PROTOCOLS, XA_ATOM, 32, PropModeReplace,
		    (unsigned char *) data, sizeof(data) / sizeof(long));
}

/** @brief Update the WinWM/WMH protocols.
  */
void
Ini_WIN_PROTOCOLS(ScreenInfo *scr)
{
    Set_WIN_PROTOCOLS(TwmWinRoot(scr));
}

/** @brief Delete the WinWM/WMH protocols.
  *
  * Should be called when shutting down the window manager (but not necessary
  * for restart).
  */
static void
Del_WIN_PROTOCOLS(ScreenInfo *scr)
{
    XDeleteProperty(dpy, TwmWinRoot(scr), _XA_WIN_PROTOCOLS);
    scr->wmh.props._WIN_PROTOCOLS = 0;
}

/** @} */

/** @name _WIN_CLIENT_LIST CARDINAL[]/32
  *
  * As an aide in having external applications be able to list and access clients
  * being managed by the Window Manager, a property should be set on the root
  * window of the name _WIN_CLIENT_LIST which is an array of type CARDINAL.  Each
  * entry is the Window ID of a managed client.  If the list of managed clients
  * changes, clients are added or deleted, this list should be updated.
  *
  * @{ */

/** @brief Set the client window list.
  * @param windows - the window list
  * @param count - number of items in window list
  */
static void
Set_WIN_CLIENT_LIST(Window root, Window *windows, int count)
{
    long *data = NULL;
    int i;

    if (count > 0 && (data = calloc(count, sizeof(long))) == NULL)
	count = 0;
    for (i = 0; i < count; i++)
	data[i] = windows[i];
    XChangeProperty(dpy, root, _XA_WIN_CLIENT_LIST, XA_CARDINAL, 32, PropModeReplace,
		    (unsigned char *) data, count);
    free(data);
}

/** @brief Update the client window list.
  *
  * Should be called on startup and whenever the window list changes.  The
  * property is not changed unless some window in the list changes.
  */
void
Upd_WIN_CLIENT_LIST(ScreenInfo *scr)
{
    Window *clients = NULL;
    int count = 0;

    TwmGetWinClientList(scr, &clients, &count);
    if (!scr->wmh.props._WIN_CLIENT_LIST
	|| cmp_window_list(scr->wmh.clients, clients) != 0) {
	Set_WIN_CLIENT_LIST(TwmWinRoot(scr), clients, count);
	scr->wmh.props._WIN_CLIENT_LIST = 1;
	free(scr->wmh.clients);
	scr->wmh.clients = clients;
    }
}

/** @brief Initialize the client window list.
  * @param scr - screen
  *
  * The client window lists is initialized to an empty list.
  */
static void
Ini_WIN_CLIENT_LIST(ScreenInfo *scr)
{
    Set_WIN_CLIENT_LIST(TwmWinRoot(scr), NULL, 0);
}

/** @brief Delete the client window list.
  *
  * Should be called when shutting down the window manager (but not necessary
  * for restart).
  */
static void
Del_WIN_CLIENT_LIST(ScreenInfo *scr)
{
    XDeleteProperty(dpy, TwmWinRoot(scr), _XA_WIN_CLIENT_LIST);
    scr->wmh.props._WIN_CLIENT_LIST = 0;
}

/** @} */

/** @name _WIN_WORKSPACE_COUNT CARDINAL/32
  *
  * If your Window Manager supports the concept of Multiple/Virtual Desktops or
  * Workspaces then you will definitely want to include it.  This involves your
  * Window Manager setting several properties on the root window.  First you
  * should advertise how many Desktops your Window Manager supports.  This is
  * done by setting a property on the root window with the atom name
  * _WIN_WORKSPACE_COUNT of type CARDINAL.  The properties data is a 32-bit
  * integer that is the number of Desktops your Window Manager currently
  * supports.  If you can add and delete desktops while running, you may change
  * this property and its value whenever required.
  *
  * @{ */

/** @brief Set the workspace count.
  * @param count - the count to set.
  */
static void
Set_WIN_WORKSPACE_COUNT(Window root, long count)
{
    XChangeProperty(dpy, root, _XA_WIN_WORKSPACE_COUNT, XA_CARDINAL, 32, PropModeReplace,
		    (unsigned char *) &count, 1);
}

/** @brief Get the workspace count.
  * @param count - where to store the count.
  */
static Bool
Get_WIN_WORKSPACE_COUNT(Window root, int *count)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, root, _XA_WIN_WORKSPACE_COUNT, 0L, 1L, False, XA_CARDINAL,
			   &actual_type, &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 1) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    *count = prop[0];
    XFree(prop);
    return True;
}

/** @brief Update the workspace count.
  *
  * Should be called if ever the number of desktops changes.  This function only
  * sets the property when it has actually changed.
  */
void
Upd_WIN_WORKSPACE_COUNT(ScreenInfo *scr)
{
    int workspace_count;

    TwmGetWorkspaceCount(scr, &workspace_count);
    if (!scr->wmh.props._WIN_WORKSPACE_COUNT
	|| scr->wmh.workspace_count != workspace_count) {
	Set_WIN_WORKSPACE_COUNT(TwmWinRoot(scr), workspace_count);
	scr->wmh.props._WIN_WORKSPACE_COUNT = 1;
	scr->wmh.workspace_count = workspace_count;
    }
}

static void
Ini_WIN_WORKSPACE_COUNT(ScreenInfo *scr)
{
    Bool present;
    int workspace_count = 1;

    present = Get_WIN_WORKSPACE_COUNT(TwmWinRoot(scr), &workspace_count);
    scr->wmh.props._WIN_WORKSPACE_COUNT = present;
    scr->wmh.workspace_count = workspace_count;
}

/** @brief Retrieve the workspace count.
  *
  * Should be called on startup to retrieve the workspace count set previously
  * (on start or restart).
  */
static void
Ret_WIN_WORKSPACE_COUNT(ScreenInfo *scr)
{
    Bool present;
    int workspace_count = 1;

    if ((present = Get_WIN_WORKSPACE_COUNT(TwmWinRoot(scr), &workspace_count)))
	TwmSetWorkspaceCount(scr, workspace_count);
    scr->wmh.props._WIN_WORKSPACE_COUNT = present;
    scr->wmh.workspace_count = workspace_count;
    Upd_WIN_WORKSPACE_COUNT(scr);
}

/** @brief Delete the workspace count.
  *
  * Should be called on exit (but not necessary on restart).
  */
static void
Del_WIN_WORKSPACE_COUNT(ScreenInfo *scr)
{
    XDeleteProperty(dpy, TwmWinRoot(scr), _XA_WIN_WORKSPACE_COUNT);
    scr->wmh.props._WIN_WORKSPACE_COUNT = 0;
    scr->wmh.workspace_count = 0;
}

/** @} */

/** @name _WIN_WORKSPACE
  *
  * You should also set a property of the atom _WIN_WORKSPACE of type CARDINAL
  * that contains the number of the currently active desktop (which is a number
  * between 0 and the number advertised by _WIN_WORKSPACE_COUNT - 1).  Whenever
  * the active desktop changes, change this property.
  *
  * @{ */

/** @brief Set the workspace for window or root.
  * @param window - window or root
  * @param workspace - workspace to set
  */
static void
Set_WIN_WORKSPACE(Window window, long workspace)
{
    XChangeProperty(dpy, window, _XA_WIN_WORKSPACE, XA_CARDINAL, 32, PropModeReplace,
		    (unsigned char *) &workspace, 1);
}

/** @brief Get the workspace for window or root.
  * @param window - window or root
  * @param workspace - where to store the workspace
  *
  * Returns True when the workspace was retrieved.
  */
Bool
Get_WIN_WORKSPACE(Window window, int *workspace)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, window, _XA_WIN_WORKSPACE, 0L, 1L, False, XA_CARDINAL,
			   &actual_type, &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 1) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    *workspace = *prop;
    XFree(prop);
    return True;
}

void
Upd_WIN_WORKSPACE_root(ScreenInfo *scr)
{
    int workspace = -2;

    TwmGetWorkspace(scr, &workspace);
    if (!scr->wmh.props._WIN_WORKSPACE || scr->wmh.workspace != workspace) {
	Set_WIN_WORKSPACE(TwmWinRoot(scr), workspace);
	scr->wmh.props._WIN_WORKSPACE = 1;
	scr->wmh.workspace = workspace;
    }
}

/** @brief Update the workspace for window or root.
  * @param twin - TWM window
  */
void
Upd_WIN_WORKSPACE(ScreenInfo *scr, TwmWindow *twin)
{
    int workspace = -2;

    TwmGetWinWorkspace(scr, twin, &workspace);
    if (!twin->wmh.props._WIN_WORKSPACE || twin->wmh.workspace != workspace) {
	Set_WIN_WORKSPACE(twin->w, workspace);
	twin->wmh.props._WIN_WORKSPACE = 1;
	twin->wmh.workspace = workspace;
    }
}

static void
Ini_WIN_WORKSPACE_root(ScreenInfo *scr)
{
    Bool present;
    int workspace = -2;

    present = Get_WIN_WORKSPACE(TwmWinRoot(scr), &workspace);
    scr->wmh.props._WIN_WORKSPACE = present;
    scr->wmh.workspace = workspace;
}

/** @brief Retrieve the workspace for window or root.
  * @param twin - TWM window or NULL for root
  *
  * Call this function when initializing (twin == NULL) to read or write the
  * current workspace to the root.  Call this function (twin != NULL) when
  * adding a window to read or write the current desktop to the window.
  */
static void
Ret_WIN_WORKSPACE(ScreenInfo *scr, TwmWindow *twin)
{
    int workspace = -2;

    if (Get_WIN_WORKSPACE(twin->w, &workspace)) {
	TwmSetWinWorkspace(scr, twin, workspace);
	twin->wmh.props._WIN_WORKSPACE = 1;
	twin->wmh.workspace = workspace;
    } else {
	twin->wmh.props._WIN_WORKSPACE = 0;
	twin->wmh.workspace = workspace;
    }
    Upd_WIN_WORKSPACE(scr, twin);
}

static void
Del_WIN_WORKSPACE_root(ScreenInfo *scr)
{
    XDeleteProperty(dpy, TwmWinRoot(scr), _XA_WIN_WORKSPACE);
    scr->wmh.props._WIN_WORKSPACE = 0;
    scr->wmh.workspace = -2;
}

/** @brief Delete the workspace for window or root.
  * @param twin - TWM window or NULL for root
  *
  * Should be called for the root when exiting; for a window when it is
  * withdrawn.
  */
static void
Del_WIN_WORKSPACE(TwmWindow *twin)
{
    XDeleteProperty(dpy, twin->w, _XA_WIN_WORKSPACE);
    twin->wmh.props._WIN_WORKSPACE = 0;
    twin->wmh.workspace = -2;
}

/** @} */

/** @name _WIN_WORKSPACE_NAMES
  *
  * Lastly you should set a property that is a list of strings called
  * _WIN_WORKSPACE_NAMES that contains names for the desktops (the first string
  * is the name of the first desktop, the second string is the second desktop,
  * etc.). This will allow applications to know what the name of the desktop is
  * too, possibly to display it.
  *
  * @{ */

/** @brief Set the workspace names
  * @param names - the list of names
  * @param count - the count of names in the list
  */
static void
Set_WIN_WORKSPACE_NAMES(Window root, char **names, int count)
{
    XTextProperty text = { NULL, };

    if (XStringListToTextProperty(names, count, &text) == Success) {
	XSetTextProperty(dpy, root, &text, _XA_WIN_WORKSPACE_NAMES);
	XFree(text.value);
    }
}

/** @brief Get the workspace names
  * @param names - where to return the list of names
  * @param count - where to return the count of list names
  *
  * Returns True when the property is successfully retrieved.
  */
static Bool
Get_WIN_WORKSPACE_NAMES(Window root, char ***names, int *count)
{
    XTextProperty text = { NULL, };

    if (XGetTextProperty(dpy, root, &text, _XA_WIN_WORKSPACE_NAMES) == Success) {
	if (XTextPropertyToStringList(&text, names, count) != 0) {
	    XFree(text.value);
	    return True;
	}
	if (text.value != NULL)
	    XFree(text.value);
    }
    return False;
}

/** @brief Update the workspace names
  *
  * Should be called whenever the workspace labels are updated.
  */
void
Upd_WIN_WORKSPACE_NAMES(ScreenInfo *scr)
{
    char **names = NULL;
    int count = 0;

    TwmGetWorkspaceNames(scr, &names, &count);
    if (!scr->wmh.props._WIN_WORKSPACE_NAMES || strcmp_list(scr->wmh.names, names) != 0) {
	Set_WIN_WORKSPACE_NAMES(TwmWinRoot(scr), names, count);
	scr->wmh.props._WIN_WORKSPACE_NAMES = 1;
	if (scr->wmh.names != NULL)
	    XFreeStringList(scr->wmh.names);
	scr->wmh.names = names;
    }
}

/** @brief - Initialize the workspace names.
  * @param scr - screen
  */
static void
Ini_WIN_WORKSPACE_NAMES(ScreenInfo *scr)
{
    Bool present;
    char **names = NULL;
    int count = 0;

    present = Get_WIN_WORKSPACE_NAMES(TwmWinRoot(scr), &names, &count);
    scr->wmh.props._WIN_WORKSPACE_NAMES = present;
    if (scr->wmh.names != NULL)
	XFreeStringList(scr->wmh.names);
    scr->wmh.names = names;
}

/** @brief Retrieve the workspace names.
  *
  * Should be called at startup.  Called automatically whenever the
  * _WIN_WORKSPACE_NAMES property changes on the root window.
  */
static void
Ret_WIN_WORKSPACE_NAMES(ScreenInfo *scr)
{
    Bool present;
    char **names = NULL;
    int count = 0;

    if ((present = Get_WIN_WORKSPACE_NAMES(TwmWinRoot(scr), &names, &count)))
	TwmSetWorkspaceNames(scr, names, count);
    scr->wmh.props._WIN_WORKSPACE_NAMES = present;
    if (scr->wmh.names != NULL)
	XFreeStringList(scr->wmh.names);
    scr->wmh.names = names;
    Upd_WIN_WORKSPACE_NAMES(scr);
}

/** @brief Delete the workspace names.
  *
  * Should be called when shutting down (but not necessary when restarting).
  */
static void
Del_WIN_WORKSPACE_NAMES(ScreenInfo *scr)
{
    XDeleteProperty(dpy, TwmWinRoot(scr), _XA_WIN_WORKSPACE_NAMES);
    scr->wmh.props._WIN_WORKSPACE_NAMES = 0;
    if (scr->wmh.names) {
	XFreeStringList(scr->wmh.names);
	scr->wmh.names = NULL;
    }
}

/** @} */

/** @name _WIN_LAYER CARDINAL/32
  *
  * When a client first maps a window, before calling XMapWindow, it will set
  * properties on the client window with certain atoms as their types.  The
  * property atoms set can be any or all of _WIN_LAYER, _WIN_STATE,
  * _WIN_WORKSPACE, _WIN_EXPANDED_SIZE and _WIN_HINTS.
  *
  * _WIN_LAYER is also a CARDINAL that is the stacking layer the application
  * wishes to exist in.
  *
  *  #define WIN_LAYER_DESKTOP      0
  *  #define WIN_LAYER_BELOW        2
  *  #define WIN_LAYER_NORMAL       4
  *  #define WIN_LAYER_ONTOP        6
  *  #define WIN_LAYER_DOCK         8
  *  #define WIN_LAYER_ABOVE_DOCK  10
  *  #define WIN_LAYER_MENU        12
  *
  * The application can choose one of these layers to exist in.  It can also
  * specify a layer other than the ones listed above if it wishes to exist
  * between 2 layers.  The layer remains constant and the window will always be
  * arranged in stacking order between windows in the layers above and below its
  * own layer. If the Window Manager changes the layer of an application it
  * should change this property.
  *
  * @{ */
static void
Set_WIN_LAYER(Window window, long layer)
{
    XChangeProperty(dpy, window, _XA_WIN_STATE, XA_CARDINAL, 32, PropModeReplace,
		    (unsigned char *) &layer, 1);
}

static Bool
Get_WIN_LAYER(Window window, unsigned *layer)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, window, _XA_WIN_LAYER, 0L, 1L, False, XA_CARDINAL,
			   &actual_type, &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 1) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    *layer = *prop;
    XFree(prop);
    return True;
}

/*
 * Called to update the layer of the window.
 */
void
Upd_WIN_LAYER(TwmWindow *twin)
{
    unsigned layer = 0;

    TwmGetWinLayer(twin, &twin->wmh.layer);
    if (!twin->wmh.props._WIN_LAYER || twin->wmh.layer != layer) {
	Set_WIN_LAYER(twin->w, twin->wmh.layer);
	twin->wmh.props._WIN_LAYER = 1;
	twin->wmh.layer = layer;
    }
}

/*
 * Called when the manged window is first added (i.e. when it is mapped) to
 * retrieve or set the _WIN_LAYER.
 */
static void
Ret_WIN_LAYER(TwmWindow *twin)
{
    unsigned layer = 0;

    if (Get_WIN_LAYER(twin->w, &layer)) {
	TwmSetWinLayer(twin, layer);
	twin->wmh.props._WIN_LAYER = 1;
	twin->wmh.layer = layer;
    } else {
	twin->wmh.props._WIN_LAYER = 0;
	twin->wmh.layer = 0;
    }
    Upd_WIN_LAYER(twin);
}

static void
Del_WIN_LAYER(TwmWindow *twin)
{
    XDeleteProperty(dpy, twin->w, _XA_WIN_LAYER);
    twin->wmh.props._WIN_LAYER = 0;
    twin->wmh.layer = 0;
}

/*
 * After an application has mapped a window, it may wish to change its own
 * state.  To do this the client sends ClientMessages to the root window with
 * information on how to change the application's state. Clients will send
 * messages as follows:
 *
 * data.l[0] = new_layer
 * 
 */
static void
Rcv_WIN_LAYER(TwmWindow *twin, XClientMessageEvent *event)
{
    unsigned layer = event->data.l[0];

    TwmSetWinLayer(twin, layer);
    Upd_WIN_LAYER(twin);
}

/** @} */

/** @name _WIN_STATE CARDINAL/32
  *
  * When a client first maps a window, before calling XMapWindow, it will set
  * properties on the client window with certain atoms as their types.  The
  * property atoms set can be any or all of _WIN_LAYER, _WIN_STATE,
  * _WIN_WORKSPACE, _WIN_EXPANDED_SIZE and _WIN_HINTS.
  *
  * For the _WIN_STATE and _WIN_HINTS properties, the bits set mean that
  * state/property is desired by the client.
  *
  *  #define WIN_STATE_STICKY          (1<<0) * everyone knows sticky *
  *  #define WIN_STATE_MINIMIZED       (1<<1) * Reserved - definition is unclear *
  *  #define WIN_STATE_MAXIMIZED_VERT  (1<<2) * window in maximized V state *
  *  #define WIN_STATE_MAXIMIZED_HORIZ (1<<3) * window in maximized H state *
  *  #define WIN_STATE_HIDDEN          (1<<4) * not on taskbar but window visible *
  *  #define WIN_STATE_SHADED          (1<<5) * shaded (MacOS / Afterstep style) *
  *  #define WIN_STATE_HID_WORKSPACE   (1<<6) * not on current desktop *
  *  #define WIN_STATE_HID_TRANSIENT   (1<<7) * owner of transient is hidden *
  *  #define WIN_STATE_FIXED_POSITION  (1<<8) * window is fixed in position even *
  *  #define WIN_STATE_ARRANGE_IGNORE  (1<<9) * ignore for auto arranging *
  *
  * These are a simple bitmasks - if the bit is set, that state is desired by the
  * application.  Once the application window has been mapped it is the
  * responsibility of the Window Manager to set these properties to the current
  * state of the Window whenever it changes states.  If the window is unmapped
  * the application is again responsible, if unmapped by the application.
  *
  * @{ */
static void
Set_WIN_STATE(Window window, long state)
{
    XChangeProperty(dpy, window, _XA_WIN_STATE, XA_CARDINAL, 32, PropModeReplace,
		    (unsigned char *) &state, 1);
}

static Bool
Get_WIN_STATE(Window window, unsigned *state)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, window, _XA_WIN_STATE, 0L, 1L, False, XA_CARDINAL,
			   &actual_type, &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 1) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    *state = *prop;
    XFree(prop);
    return True;
}

/*
 * Called to update the state of the window.
 */
void
Upd_WIN_STATE(ScreenInfo *scr, TwmWindow *twin)
{
    unsigned state = 0;

    TwmGetWinState(scr, twin, &twin->wmh.state);
    if (!twin->wmh.props._WIN_STATE || twin->wmh.state != state) {
	Set_WIN_STATE(twin->w, state);
	twin->wmh.props._WIN_STATE = 1;
	twin->wmh.state = state;
    }
}

/*
 * Called when the managed window is first added (i.e. when it is mapped) to
 * retrieve or set the _WIN_STATE.
 */
static void
Ret_WIN_STATE(ScreenInfo *scr, TwmWindow *twin)
{
    unsigned state = 0;

    if (Get_WIN_STATE(twin->w, &state)) {
	TwmSetWinState(scr, twin, -1U, state);
	twin->wmh.props._WIN_STATE = 1;
	twin->wmh.state = state;
    } else {
	twin->wmh.props._WIN_STATE = 0;
	twin->wmh.state = 0;
    }
    Upd_WIN_STATE(scr, twin);
}

static void
Del_WIN_STATE(TwmWindow *twin)
{
    XDeleteProperty(dpy, twin->w, _XA_WIN_STATE);
    twin->wmh.props._WIN_STATE = 0;
    twin->wmh.state = 0;
}

/*
 * After an application has mapped a window, it may wish to change its own
 * state.  To do this the client sends ClientMessages to the root window with
 * information on how to change the application's state. Clients will send
 * messages as follows:
 *
 * data.l[0] = mask_of_members_to_change
 * data.l[1] = new_members
 * 
 */
static void
Rcv_WIN_STATE(ScreenInfo *scr, TwmWindow *twin, XClientMessageEvent *event)
{
    unsigned mask = event->data.l[0];
    unsigned change = event->data.l[1];

    TwmSetWinState(scr, twin, mask, change);
    Upd_WIN_STATE(scr, twin);
}

/** @} */

/** @name _WIN_WORKSPACE CARDINAL/32
  *
  * When a client first maps a window, before calling XMapWindow, it will set
  * properties on the client window with certain atoms as their types.  The
  * property atoms set can be any or all of _WIN_LAYER, _WIN_STATE,
  * _WIN_WORKSPACE, _WIN_EXPANDED_SIZE and _WIN_HINTS.
  *
  * _WIN_WORKSPACE is a CARDINAL that is the Desktop number the app
  * would like to be on.  This desktop number is updated by the Window Manager
  * after the window is mapped and until the window is unmapped by the
  * application. The value for this property is simply the numeric for the
  * desktop 0, being the first desktop available.
  *
  * On an application window, this is the current workspace of the application
  * window.  The default workspace can be set by applications, but they must use
  * ClientMessages to change them.  When window is mapped, the property should
  * only be updated by the window manager.
  *
  * @{ */

static void
Rcv_WIN_WORKSPACE_root(ScreenInfo *scr, XClientMessageEvent *event)
{
    int workspace = event->data.l[0];

    TwmSetWorkspace(scr, workspace);
    Upd_WIN_WORKSPACE_root(scr);
}

/*
 * After an application has mapped a window, it may wish to change its own
 * state.  To do this the client sends ClientMessages to the root window with
 * information on how to change the application's state.  Clients will send
 * messages as follows:
 *
 * ev.type = ClientMessage
 * ev.display = dpy;
 * ev.window = root;
 * ev.message_type = _XA_WIN_WORKSPACE;
 * ev.format = 32;
 * ev.data.l[0] = new_workspace;
 * 
 */
static void
Rcv_WIN_WORKSPACE(ScreenInfo *scr, TwmWindow *twin, XClientMessageEvent *event)
{
    int workspace = event->data.l[0];

    TwmSetWinWorkspace(scr, twin, workspace);
    Upd_WIN_WORKSPACE(scr, twin);
}

/** @} */

/** @name _WIN_EXPANDED_SIZE x, y, width, height CARDINAL[4]/32
  *
  * When a client first maps a window, before calling XMapWindow, it will set
  * properties on the client window with certain atoms as their types.  The
  * property atoms set can be any or all of _WIN_LAYER, _WIN_STATE,
  * _WIN_WORKSPACE, _WIN_EXPANDED_SIZE and _WIN_HINTS.
  *
  * The expanded space occupied is the area on the screen that the app's window
  * will occupy when "expanded".  That is, if you have a botton on an app that
  * "hides" it by reducing its size, this is the geometry of the expanded window
  * - so the window manager can allow for this when doing auto position of the
  * client windows assuming the app can at any point use this area and thus try
  * and keep it clear.  Only the client sets this.
  *
  * @{
  */
static Bool
Get_WIN_EXPANDED_SIZE(Window window, struct WinGeometry *geom)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, window, _XA_WIN_EXPANDED_SIZE, 0L, 4L, False, XA_CARDINAL,
			   &actual_type, &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 1) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    geom->x = prop[0];
    geom->y = prop[1];
    geom->width = prop[2];
    geom->height = prop[3];
    XFree(prop);
    return True;
}

/*
 * Called by the TWM when adding a new managed window to retrieve the expanded
 * size of the window (when set by the client).  Called internally when the
 * _WIN_EXPANDED_SIZE property changes on an already managed window.
 */
static void
Ret_WIN_EXPANDED_SIZE(TwmWindow *twin)
{
    struct WinGeometry expanded = { 0, };

    if (Get_WIN_EXPANDED_SIZE(twin->w, &expanded)) {
	TwmSetExpandedSize(twin, &expanded);
	twin->wmh.props._WIN_EXPANDED_SIZE = 1;
	twin->wmh.expanded = expanded;
    } else {
	TwmDelExpandedSize(twin, &expanded);
	twin->wmh.props._WIN_EXPANDED_SIZE = 0;
	twin->wmh.expanded = expanded;
    }
}

static void
Del_WIN_EXPANDED_SIZE(TwmWindow *twin)
{
    twin->wmh.props._WIN_EXPANDED_SIZE = 0;
    twin->wmh.expanded.x = 0;
    twin->wmh.expanded.y = 0;
    twin->wmh.expanded.width = 0;
    twin->wmh.expanded.height = 0;
}

/** @} */

/** @name _WIN_HINTS, CARDINAL/32
  *
  * When a client first maps a window, before calling XMapWindow, it will set
  * properties on the client window with certain atoms as their types.  The
  * property atoms set can be any or all of _WIN_LAYER, _WIN_STATE,
  * _WIN_WORKSPACE, _WIN_EXPANDED_SIZE and _WIN_HINTS.
  *
  * For the _WIN_STATE and _WIN_HINTS properties, the bits set mean that
  * state/property is desired by the client.
  *
  *  #define WIN_HINTS_SKIP_FOCUS      (1<<0) * "alt-tab" skips this win *
  *  #define WIN_HINTS_SKIP_WINLIST    (1<<1) * do not show in window list *
  *  #define WIN_HINTS_SKIP_TASKBAR    (1<<2) * do not show on taskbar *
  *  #define WIN_HINTS_GROUP_TRANSIENT (1<<3) * Reserved - definition is unclear *
  *  #define WIN_HINTS_FOCUS_ON_CLICK  (1<<4) * app only accepts focus if clicked *
  *
  * This is also a simple bitmask but only the application changes it, thus
  * whenever this property changes the Window Manager should re-read it and honor
  * any changes.
  *
  * @{ */

static Bool
Get_WIN_HINTS(Window window, unsigned *hints)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, window, _XA_WIN_HINTS, 0L, 1L, False, XA_CARDINAL,
			   &actual_type, &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 1) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    *hints = *prop;
    XFree(prop);
    return True;
}

/*
 * Called when the manged window is first added (i.e. when it is mapped) to
 * retrieve the _WIN_HINTS.  Called internally whenever the _WIN_HINTS property
 * is updated.
 */
static void
Ret_WIN_HINTS(TwmWindow *twin)
{
    unsigned hints = 0;

    if (Get_WIN_HINTS(twin->w, &hints)) {
	TwmSetWinHints(twin, hints);
	twin->wmh.props._WIN_HINTS = 1;
	twin->wmh.hints = hints;
    } else {
	TwmDelWinHints(twin, &hints);
	twin->wmh.props._WIN_HINTS = 0;
	twin->wmh.hints = 0;
    }
}

static void
Del_WIN_HINTS(TwmWindow *twin)
{
    twin->wmh.props._WIN_HINTS = 0;
    twin->wmh.hints = 0;
}

/** @} */

/** @name _WIN_DESKTOP_BUTTON_PROXY window CARDINAL/32
  *
  * X imposes the limitation that only 1 client can be selected for button
  * presses on awindow - this is due to the implicit grab nature of button press
  * events in X.  This poses a problem when more than one client wishes to select
  * for these events on thesame window - E.g., the root window, or in the case of
  * a WM that has more than one root window (virtual root windows) any of these
  * windows.  The solution to this is to have the client that receives these
  * events handle any of the events it is interested in, and then "proxy" or
  * "pass on" any events it does not care about. The traditional model has always
  * been that the WM selects for button presses on the desktop, it is only
  * natural that it keep doing this BUT have a way of sending unwanted
  * presses on to some other process(es) that may well be interested.
  *
  * This is done as follows:
  *
  * 1. Set a property on the root window called _WIN_DESKTOP_BUTTON_PROXY.  It is
  *    of the type cardinal - its value is the Window ID of another window that
  *    is not mapped and is created as an immediate child of the root window.
  *    This window also has this property set on it pointing to itself.
  *
  * 2. Whenever the WM gets a button press or release event it can check the
  *    buttonon the mouse pressed, any modifiers, etc. - if the WM wants the
  *    event it can deal with it as per normal and not proxy it on - if the WM
  *    does not wish to do anythingas a result of this event, then it should pass
  *    the event along like the following:
  *
  *    Display *disp;
  *    Window bpress_win;
  *    XEvent *ev;
  *
  *    XUngrabPointer(disp, CurrentTime);
  *    XSendEvent(disp, bpress_win, False, SubstructureNotifyMask, ev);
  *
  *    where ev is a pointer to the actual Button press or release event it
  *    receives from theX Server (retaining timestamp, original window ID,
  *    coordinates etc.) NB - the XUngrabPointer is only required before proxying
  *    a press, not a release.
  *
  * @{ */
static void
Set_WIN_DESKTOP_BUTTON_PROXY(Window root, Window window)
{
    long data = window;

    XChangeProperty(dpy, window, _XA_WIN_DESKTOP_BUTTON_PROXY, XA_CARDINAL, 32,
		    PropModeReplace, (unsigned char *) &data, 1);
    XChangeProperty(dpy, root, _XA_WIN_DESKTOP_BUTTON_PROXY, XA_CARDINAL, 32,
		    PropModeReplace, (unsigned char *) &data, 1);
}

void
Upd_WIN_DESKTOP_BUTTON_PROXY(ScreenInfo *scr)
{
    Window proxy = TwmWinManager(scr);

    if (!scr->wmh.props._WIN_DESKTOP_BUTTON_PROXY || scr->wmh.proxy != proxy) {
	Set_WIN_DESKTOP_BUTTON_PROXY(TwmWinRoot(scr), proxy);
	scr->wmh.props._WIN_DESKTOP_BUTTON_PROXY = 1;
	scr->wmh.proxy = proxy;
    }
}

static void
Del_WIN_DESKTOP_BUTTON_PROXY(ScreenInfo *scr)
{
    XDeleteProperty(dpy, TwmWinRoot(scr), _XA_WIN_DESKTOP_BUTTON_PROXY);
    scr->wmh.props._WIN_DESKTOP_BUTTON_PROXY = 0;
    scr->wmh.proxy = None;
}

void
Snd_WIN_DESKTOP_BUTTON_PROXY(ScreenInfo *scr, XEvent *event)
{
    XUngrabPointer(dpy, CurrentTime);
    XSendEvent(dpy, scr->wmh.proxy, False, SubstructureNotifyMask, event);
}

/** @} */

/** @name _WIN_AREA_COUNT, cols rows CARDINAL[2]/32
  *
  * The best way to explain this is as follows.  Desktops are completely
  * geometrically disjoint workspaces.  They have no geometric relevance to each
  * other in terms of the client window plane.  Desktop Areas have geometric
  * relevance - they are next to, above, or below each other.  The best examples
  * are FVWM's desktops and virtual desktops - you can have multiple desktops
  * that are disjoint and each desktop can be N x M screens in size - these N x M
  * areas are what are termed "desktop areas" for the purposes of this document
  * and the WM API.  If your WM supports both methods like FVMW, Enlightenment
  * and possible others, you should use _WIN_WORKSPACE messages and atoms for the
  * geometrically disjoint desktops - for geometrically arranged desktops you
  * should use the_WIN_AREA messages and atoms.  If you only support one of these
  * it is preferable to use _WIN_WORKSPACE only.  The APi for _WIN_AREA is very
  * similar to _WIN_WORKSPACE.  To advertise the size of your areas (E.g., N x M
  * screens in size) you set an atom on the root window.
  *
  *  data.l[0] = cols
  *  data.l[1] = rows
  *
  * @{ */
static void
Set_WIN_AREA_COUNT(Window root, struct WinLayout *layout)
{
    XChangeProperty(dpy, root, _XA_WIN_AREA_COUNT, XA_CARDINAL, 32, PropModeReplace,
		    (unsigned char *) layout, 2);
}

static Bool
Get_WIN_AREA_COUNT(Window root, struct WinLayout *layout)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, root, _XA_WIN_AREA_COUNT, 0L, 2L, False, XA_CARDINAL,
			   &actual_type, &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 2) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    layout->cols = prop[0];
    layout->rows = prop[1];
    XFree(prop);
    return True;
}

void
Upd_WIN_AREA_COUNT(ScreenInfo *scr)
{
    struct WinLayout area_count = { 1, 1 };

    TwmGetWinAreaCount(scr, &area_count);
    if (!scr->wmh.props._WIN_AREA_COUNT || cmp_layout(&scr->wmh.area_count, &area_count) != 0) {
	Set_WIN_AREA_COUNT(TwmWinRoot(scr), &area_count);
	scr->wmh.props._WIN_AREA_COUNT = 1;
	scr->wmh.area_count = area_count;
    }
}

static void
Ini_WIN_AREA_COUNT(ScreenInfo *scr)
{
    Bool present;
    struct WinLayout area_count = { 1, 1 };

    present = Get_WIN_AREA_COUNT(TwmWinRoot(scr), &area_count);
    scr->wmh.props._WIN_AREA_COUNT = present;
    scr->wmh.area_count = area_count;
}

static void
Ret_WIN_AREA_COUNT(ScreenInfo *scr)
{
    Bool present;
    struct WinLayout area_count = { 1, 1 };

    if ((present = Get_WIN_AREA_COUNT(TwmWinRoot(scr), &area_count)))
	TwmSetWinAreaCount(scr, &area_count);
    scr->wmh.props._WIN_AREA_COUNT = present;
    scr->wmh.area_count = area_count;
    Upd_WIN_AREA_COUNT(scr);
}

static void
Rcv_WIN_AREA_COUNT(ScreenInfo *scr, XClientMessageEvent *event)
{
    struct WinLayout layout;

    layout.cols = event->data.l[0];
    layout.rows = event->data.l[1];

    TwmSetWinAreaCount(scr, &layout);
    Upd_WIN_AREA_COUNT(scr);
}

static void
Del_WIN_AREA_COUNT(ScreenInfo *scr)
{
    XDeleteProperty(dpy, TwmWinRoot(scr), _XA_WIN_AREA_COUNT);
    scr->wmh.props._WIN_AREA_COUNT = 0;
    scr->wmh.area_count.cols = scr->wmh.area_count.rows = 0;
}

/** @} */

/** @name _WIN_AREA, active_area_x active_area_y CARDINAL[2]/32
  *
  * To advertise which desktop area is currently the active one, set this
  * property on the root window.  The property is the current workspace area as a
  * column index (starting at zero), active_area_x, and a row index (starting at
  * zero), active_area_y.
  *
  * @{ */
static void
Set_WIN_AREA(Window root, struct WinArea *area)
{
    XChangeProperty(dpy, root, _XA_WIN_AREA, XA_CARDINAL, 32, PropModeReplace,
		    (unsigned char *) area, 2);
}

static Bool
Get_WIN_AREA(Window root, struct WinArea *area)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, root, _XA_WIN_AREA, 0L, 2L, False, XA_CARDINAL,
			   &actual_type, &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 2) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    area->col = prop[0];
    area->row = prop[1];
    XFree(prop);
    return True;
}

/*
 * Called to update the active area (viewport position) whenever it changes.
 */
void
Upd_WIN_AREA(ScreenInfo *scr)
{
    struct WinArea area = { 0, 0 };

    TwmGetWinArea(scr, &area);
    if (!scr->wmh.props._WIN_AREA || cmp_area(&scr->wmh.area, &area) != 0) {
	Set_WIN_AREA(TwmWinRoot(scr), &area);
	scr->wmh.props._WIN_AREA = 1;
	scr->wmh.area = area;
    }
}

/*
 * Called on startup to retrieve the active area (viewport position).
 * This should only be called after Ini_WIN_WORKSPACE_root().
 */
static void
Ini_WIN_AREA(ScreenInfo *scr)
{
    Bool present;
    struct WinArea area = { 0, 0 };

    present = Get_WIN_AREA(TwmWinRoot(scr), &area);
    scr->wmh.props._WIN_AREA = present;
    scr->wmh.area = area;
}

/*
 * If a client wishes to change what the current active area is they simply send
 * a clientmessage like:
 *
 * ev.window = root;
 * ev.message_type = _XA_WIN_AREA;
 * ev.format = 32;
 * ev.data.l[0] = new_active_area_x;
 * ev.data.l[1] = new_actvie_area_y;
 */
static void
Rcv_WIN_AREA(ScreenInfo *scr, XClientMessageEvent *event)
{
    struct WinArea area;

    area.col = event->data.l[0];
    area.row = event->data.l[1];
    TwmSetWinArea(scr, &area);
    Upd_WIN_AREA(scr);
}

static void
Del_WIN_AREA(ScreenInfo *scr)
{
    XDeleteProperty(dpy, TwmWinRoot(scr), _XA_WIN_AREA);
    scr->wmh.props._WIN_AREA = 0;
    scr->wmh.area.col = 0;
    scr->wmh.area.row = 0;
}

/** @} */

/** @name _WIN_APP_STATE, state CARDINAL/32
  *
  * This property contains the application state (also "color reactiveness") -
  * the application can keep changing this property when it changes its state and
  * the window amnager or monitoring program will pick this up and display
  * something accordingly.  Only the client sets this property.
  *
  * @{ */
static Bool
Get_WIN_APP_STATE(Window window, unsigned *state)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, window, _XA_WIN_APP_STATE, 0L, 1L, False, XA_CARDINAL,
			   &actual_type, &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 1) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    *state = prop[0];
    return True;
}

static void
Ret_WIN_APP_STATE(TwmWindow *twin)
{
    unsigned app_state = 0;

    if (Get_WIN_APP_STATE(twin->w, &app_state)) {
	TwmSetWinAppState(twin, app_state);
	twin->wmh.props._WIN_APP_STATE = 1;
	twin->wmh.app_state = app_state;
    } else {
	TwmDelWinAppState(twin, &app_state);
	twin->wmh.props._WIN_APP_STATE = 0;
	twin->wmh.app_state = 0;
    }
}

static void
Del_WIN_APP_STATE(TwmWindow *twin)
{
    twin->wmh.props._WIN_APP_STATE = 0;
    twin->wmh.app_state = 0;
}

/** @} */

/** @name _WIN_ICONS n length (pixmap mask width height depth drawable ...) CARDINAL[]/32
  *
  * This property contains additional icons for the application.  If this
  * property is set, the WM will ignore default X icon things and KWM_WIN_ICON
  * hint.  Icon Mask can be None if transparency is not required.
  *
  *  n        - icon record count
  *  length   - length of each icon record
  *
  * Each icon record has:
  *
  *  pixmap   - XID
  *  mask     - XID
  *  width    - width of icon (pixmap and mask)
  *  height   - height of icon (pixmap and mask)
  *  depth    - depth of icon (pixmap), mask is depth 1
  *  drawable - XID screen root drawable of pixmap
  *  ...      - possible additional fields
  *
  * Icon mask can be None if transparency is not required.
  *
  * Window managers currently need/use icons of size 16x16 and 32x32 with default
  * depth.  Applications are recommended to set several icon sizes (recommended
  * sizes 16x16, 32x32, 48x48 at least).
  *
  * @{ */
static Bool
Get_WIN_ICONS(Window window, long **data)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0, len;
    long *prop = NULL;

    if (*data != NULL) {
	XFree(*data);
	*data = NULL;
    }
    status =
	XGetWindowProperty(dpy, window, _XA_WIN_ICONS, 0L, 2L, False, XA_CARDINAL,
			   &actual_type, &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 2) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    len = prop[0] * prop[1] + 2;
    XFree(prop);
    prop = NULL;
    if (bytes_after < (len - 2) * 4)
	return False;
    status =
	XGetWindowProperty(dpy, window, _XA_WIN_ICONS, 0L, len, False, XA_CARDINAL,
			   &actual_type, &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < len) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    *data = prop;
    return True;
}

static void
Ret_WIN_ICONS(TwmWindow *twin)
{
    long *icons = NULL;

    if (Get_WIN_ICONS(twin->w, &icons)) {
	TwmSetWinIcons(twin, icons);
	twin->wmh.props._WIN_ICONS = 1;
	if (twin->wmh.icons != NULL)
	    XFree(twin->wmh.icons);
	twin->wmh.icons = icons;
    } else {
	twin->wmh.props._WIN_ICONS = 0;
	if (twin->wmh.icons != NULL)
	    XFree(twin->wmh.icons);
	twin->wmh.icons = NULL;
    }
}

static void
Del_WIN_ICONS(TwmWindow *twin)
{
    twin->wmh.props._WIN_ICONS = 0;
    if (twin->wmh.icons != NULL) {
	XFree(twin->wmh.icons);
	twin->wmh.icons = NULL;
    }
}

/** @} */

/** @name _WIN_WORKAREA, x-min y-min x-max y-max CARDINAL[4]/32
  *
  * Contains a list of minimum and maximum x and y coordinates of the available
  * work area.  The minimum coordinates are the upper-left corner and the maximum
  * coordinates are the lower-right corner of the avilable area.  This is only
  * set by the window manager.
  *
  * Note that event though the values specified are signed numbers, they are
  * stored in the property with type CARDINAL.
  *
  * @{ */
static void
Set_WIN_WORKAREA(Window root, struct WinWorkarea *area)
{
    XChangeProperty(dpy, root, _XA_WIN_WORKAREA, XA_CARDINAL, 32, PropModeReplace,
		    (unsigned char *) area, 4);
}

void
Upd_WIN_WORKAREA(ScreenInfo *scr)
{
    struct WinWorkarea workarea = { 0, 0, scr->rootw, scr->rooth };

    TwmGetWinWorkarea(scr, &workarea);
    if (!scr->wmh.props._WIN_WORKAREA || cmp_workarea(&scr->wmh.workarea, &workarea) != 0) {
	Set_WIN_WORKAREA(TwmWinRoot(scr), &workarea);
	scr->wmh.props._WIN_WORKAREA = 1;
	scr->wmh.workarea = workarea;
    }
}

static void
Del_WIN_WORKAREA(ScreenInfo *scr)
{
    XDeleteProperty(dpy, TwmWinRoot(scr), _XA_WIN_WORKAREA);
    scr->wmh.props._WIN_WORKAREA = 0;
}

/** @} */

/** @name _WIN_WORKSPACES, mask1 mask2 ... CARDINAL[]/32
  *
  * This property is complex enough to largely be unusable.  The property
  * consists of a set of 32-bit masks.  The first mask represents the occupancy
  * of the window on the first 32 workspaces; then second, the next 32
  * workspaces, and so on.  The least significant bit represents ealier
  * workspaces (workspace 0 in the first mask) and the most significant
  * represents later workspaces (workspace 32 in the first mask).  A 1 in a bit
  * position indicates that the window exists on the corresponding workspace; a
  * 0, that the window does not exist on the corresponding workspace.
  *
  * If the client wishes to change the workspace occupancy after intial mapping,
  * it must send a client message to the root window with and 'index' and
  * 'replace' arguments.  The 'index' argument indicates which mask to affect,
  * where 0 indicates the first mask.  The 'replace' argument provide
  * replacement bits for the mask indicted by the 'index'.
  *
  * @{ */

/** @brief Set the workspace masks of a window.
  * @param window - window to set
  * @param mask - bit mask of workspaces
  * @param count - number of words in bit mask
  */
static void
Set_WIN_WORKSPACES(Window window, long *mask, int count)
{
    XChangeProperty(dpy, window, _XA_WIN_WORKSPACES, XA_CARDINAL, 32,
		    PropModeReplace, (unsigned char *) mask, count);
}

/** @brief Get the workspace masks of a window.
  * @param window - window from which to fetch
  * @param mask - where to return the mask
  * @param count - where to returnt the count of words in the mask
  */
static Bool
Get_WIN_WORKSPACES(Window window, long **mask, int *count)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0, len, n;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, window, _XA_WIN_WORKSPACES, 0L, 1L, False,
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
	XGetWindowProperty(dpy, window, _XA_WIN_WORKSPACES, 0L, len, False,
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

/** @brief Update the workspace mask for a window.
  * @param twin - TWM window to update
  */
void
Upd_WIN_WORKSPACES(ScreenInfo *scr, TwmWindow *twin)
{
    long *mask = NULL;
    int masks = 0;

    TwmGetWMWorkspaces(scr, twin, &mask, &masks);
    if (!twin->wmh.props._WIN_WORKSPACES || cmp_mask_list(twin->wmh.mask, twin->wmh.masks, mask, masks) != 0) {
	Set_WIN_WORKSPACES(twin->w, mask, masks);
	twin->wmh.props._WIN_WORKSPACES = 1;
	free(twin->wmh.mask);
	twin->wmh.mask = mask;
	twin->wmh.masks = masks;
    }
}

/** @brief Retrieve the workspace mask for a window.
  * @param twin - TWM window
  *
  * Sets the workspace mask to the property or derives it from the workspace
  * property.
  */
static void
Ret_WIN_WORKSPACES(ScreenInfo *scr, TwmWindow *twin)
{
    long *mask = NULL;
    int masks = 0;

    if (Get_WIN_WORKSPACES(twin->w, &mask, &masks)) {
	TwmSetWMWorkspaces(scr, twin, mask, masks);
	twin->wmh.props._WIN_WORKSPACES = 1;
	free(twin->wmh.mask);
	twin->wmh.mask = mask;
	twin->wmh.masks = masks;
    } else {
	twin->wmh.props._WIN_WORKSPACES = 0;
	free(twin->wmh.mask);
	twin->wmh.mask = NULL;
	twin->wmh.masks = 0;
    }
    Upd_WIN_WORKSPACES(scr, twin);
}

/** @brief Delete the workspace mask from a window.
  * @param twin - TWM window
  */
void
Del_WIN_WORKSPACES(TwmWindow *twin)
{
    XDeleteProperty(dpy, twin->w, _XA_WIN_WORKSPACES);
    twin->wmh.props._WIN_WORKSPACES = 0;
    free(twin->wmh.mask);
    twin->wmh.mask = NULL;
    twin->wmh.masks = 0;
}

/** @brief Change the workspace mask for a window.
  * @param scr - screen
  * @param twin - TWM window
  * @param event - client message changing the workspace mask
  */
static void
Rcv_WIN_WORKSPACES(ScreenInfo *scr, TwmWindow *twin, XClientMessageEvent *event)
{
    unsigned index = event->data.l[0];
    unsigned mask = event->data.l[1];

    TwmChgWMWorkspaces(scr, twin, index, mask);
    Upd_WIN_WORKSPACES(scr, twin);
    Upd_WIN_WORKSPACE(scr, twin);
    Upd_WIN_STATE(scr, twin);	/* for sticky state */
}

/** @} */

/** @name _WIN_CLIENT_MOVING
  *
  * This atom is a 32-bit integer that is either 0 or 1 (currently).  0 denotes
  * everything is as per usual but 1 denotes that ALL configure requests by the
  * client on the client window with this property are not just a simple
  * "moving" of the window, but the result of a user move the window BUT the
  * client is handling that interaction by moving its own window.  The window
  * manager should respond accordingly by assuming any configure requests for
  * this window whilst this atom is "active" in the "1" state are a client move
  * and should handle flipping desktops if the window is being dragged "off
  * screen" or across desktop boundaries etc.  This atom is only ever set by the
  * client.
  *
  * @{ */
static Bool
Get_WIN_CLIENT_MOVING(Window window, Bool *moving)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, window, _XA_WIN_CLIENT_MOVING, 0L, 1L, False, XA_CARDINAL,
			   &actual_type, &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    if (status != Success || actual_type == None) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    *moving = False;
    if (nitems >= 1)
	*moving = prop[0] ? True : False;
    XFree(prop);
    return True;
}

static void
Ret_WIN_CLIENT_MOVING(TwmWindow *twin)
{
    Bool moving = False;

    if (Get_WIN_CLIENT_MOVING(twin->w, &moving)) {
	TwmSetMoving(twin, moving);
	twin->wmh.props._WIN_CLIENT_MOVING = 1;
	twin->wmh.moving = moving;
    } else {
	TwmSetMoving(twin, False);
	twin->wmh.props._WIN_CLIENT_MOVING = 0;
	twin->wmh.moving = False;
    }
}

/** @} */

/** @name _WIN_MAXIMIZED_GEOMETRY
  *
  * @{ */

static void
Set_WIN_MAXIMIZED_GEOMETRY(Window window, struct WinGeometry *maximized)
{
    XChangeProperty(dpy, window, _XA_WIN_MAXIMIZED_GEOMETRY, XA_CARDINAL, 32,
		    PropModeReplace, (unsigned char *) maximized, 4);
}

static Bool
Get_WIN_MAXIMIZED_GEOMETRY(Window window, struct WinGeometry *maximized)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

    status =
	XGetWindowProperty(dpy, window, _XA_WIN_MAXIMIZED_GEOMETRY, 0L, 4L, False,
			   XA_CARDINAL, &actual_type, &actual_format, &nitems,
			   &bytes_after, (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 4) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    *maximized = *(struct WinGeometry *) prop;
    XFree(prop);
    return True;
}

void
Upd_WIN_MAXIMIZED_GEOMETRY(ScreenInfo *scr, TwmWindow *twin)
{
    struct WinGeometry maximized = { 0, 0, scr->rootw, scr->rooth };

    TwmGetMaximizedGeometry(scr, twin, &maximized);
    if (!twin->wmh.props._WIN_MAXIMIZED_GEOMETRY || cmp_geo(&twin->wmh.maximized, &maximized) != 0) {
	Set_WIN_MAXIMIZED_GEOMETRY(twin->w, &maximized);
	twin->wmh.props._WIN_MAXIMIZED_GEOMETRY = 1;
	twin->wmh.maximized = maximized;
    }
}

static void
Ret_WIN_MAXIMIZED_GEOMETRY(ScreenInfo *scr, TwmWindow *twin)
{
    struct WinGeometry maximized = { 0, 0, scr->rootw, scr->rooth };

    if (Get_WIN_MAXIMIZED_GEOMETRY(twin->w, &maximized)) {
	TwmSetMaximizedGeometry(scr, twin, &maximized);
	twin->wmh.props._WIN_MAXIMIZED_GEOMETRY = 1;
	twin->wmh.maximized = maximized;
    } else {
	twin->wmh.props._WIN_MAXIMIZED_GEOMETRY = 0;
	twin->wmh.maximized.x = 0;
	twin->wmh.maximized.y = 0;
	twin->wmh.maximized.width = scr->rootw;
	twin->wmh.maximized.height = scr->rooth;
    }
    Upd_WIN_MAXIMIZED_GEOMETRY(scr, twin);
}

static void
Del_WIN_MAXIMIZED_GEOMETRY(TwmWindow *twin)
{
    XDeleteProperty(dpy, twin->w, _XA_WIN_MAXIMIZED_GEOMETRY);
    twin->wmh.props._WIN_MAXIMIZED_GEOMETRY = 0;
    twin->wmh.maximized.x = 0;
    twin->wmh.maximized.y = 0;
    twin->wmh.maximized.width = 0;
    twin->wmh.maximized.height = 0;
}

/** @} */

/** @name __SWM_VROOT
  *
  * This property is the old-old way of identifying virtual roots, particularly
  * for root background setting programs.  The new way of doing this is the
  * _NET_VIRTUAL_ROOTS property of NetWM/EWMH.  The __SWM_VROOT property is set
  * on each virtual root window.  It is expected that virtual root windows are
  * top-level windows: that is, a direct descendant of the root window.
  *
  * @{ */

/** @brief Set virtual roots.
  * @param windows - list of virtual root windows
  * @param count - count of the windows in the list
  */
static void
Set__SWM_VROOT(Window *windows, int count)
{
    int i;

    for (i = 0; i < count; i++) {
	long data = windows[i];

	XChangeProperty(dpy, data, _XA__SWM_VROOT, XA_WINDOW, 32, PropModeReplace,
			(unsigned char *) &data, 1);
    }
}

/** @brief Update virtual roots.
  *
  * Should be called by the window manager if ever the list of virtual roots
  * changes.
  */
void
Upd__SWM_VROOT(ScreenInfo *scr)
{
    Window *vroots = NULL;
    int count = 0;

    TwmGetSwmVroots(scr, &vroots, &count);
    if (!scr->wmh.props.__SWM_VROOT || cmp_window_list(scr->wmh.vroots, vroots) != 0) {
	Set__SWM_VROOT(vroots, count);
	scr->wmh.props.__SWM_VROOT = 1;
	free(scr->wmh.vroots);
	scr->wmh.vroots = vroots;
    }
}

/** @} */

/** @name Initialization and termination of the window manager.
  * @{ */

/** @brief Initialize the window manager in a WinWM/WMH sense.
  *
  * This function is called when TWM starts to initialize root window properties
  * and screen information on initialization of the window manager.
  */
void
InitWmh(ScreenInfo *scr)
{
    InternWinAtoms();

#ifdef DEBUG_WMH
    fprintf(stderr, "Initializing WinWM/WMH support.\n");
    fflush(stderr);
#endif

#ifdef DEBUG_WMH
    fprintf(stderr, "Initializing _WIN_PROTOCOLS on root\n");
    fflush(stderr);
#endif
    Ini_WIN_PROTOCOLS(scr);
#ifdef DEBUG_WMH
    fprintf(stderr, "Initializing _WIN_CLIENT_LIST on root\n");
    fflush(stderr);
#endif
    Ini_WIN_CLIENT_LIST(scr);
#ifdef DEBUG_WMH
    fprintf(stderr, "Initializing _WIN_WORKSPACE_COUNT on root\n");
    fflush(stderr);
#endif
    Ini_WIN_WORKSPACE_COUNT(scr);
#ifdef DEBUG_WMH
    fprintf(stderr, "Initializing _WIN_WORKSPACE_NAMES on root\n");
    fflush(stderr);
#endif
    Ini_WIN_WORKSPACE_NAMES(scr);
#ifdef DEBUG_WMH
    fprintf(stderr, "Initializing _WIN_AREA_COUNT on root\n");
    fflush(stderr);
#endif
    Ini_WIN_AREA_COUNT(scr);
#ifdef DEBUG_WMH
    fprintf(stderr, "Initializing _WIN_WORKSPACE on root\n");
    fflush(stderr);
#endif
    Ini_WIN_WORKSPACE_root(scr);
#ifdef DEBUG_WMH
    fprintf(stderr, "Initializing _WIN_AREA on root\n");
    fflush(stderr);
#endif
    Ini_WIN_AREA(scr);

#if 0
    /* Do not do this until all other things are set up in UpdateWmh() */
    Ini_WIN_SUPPORTING_WM_CHECK(scr);
#endif
}

/** @brief Update the window manager in the WinWM/WMH sense.
  *
  * This function can be used to update root window properties when it is known
  * that something might have changed but not known which property might have
  * changed.
  */
void
UpdateWmh(ScreenInfo *scr)
{
#ifdef DEBUG_WMH
    fprintf(stderr, "Updating _WIN_CLIENT_LIST on root\n");
    fflush(stderr);
#endif
    Upd_WIN_CLIENT_LIST(scr);
#ifdef DEBUG_WMH
    fprintf(stderr, "Updating _WIN_WORKSPACE_COUNT on root\n");
    fflush(stderr);
#endif
    Upd_WIN_WORKSPACE_COUNT(scr);
#ifdef DEBUG_WMH
    fprintf(stderr, "Updating _WIN_WORKSPACE_NAMES on root\n");
    fflush(stderr);
#endif
    Upd_WIN_WORKSPACE_NAMES(scr);
#ifdef DEBUG_WMH
    fprintf(stderr, "Updating _WIN_AREA_COUNT on root\n");
    fflush(stderr);
#endif
    Upd_WIN_AREA_COUNT(scr);
#ifdef DEBUG_WMH
    fprintf(stderr, "Updating _WIN_WORKSPACE on root\n");
    fflush(stderr);
#endif
    Upd_WIN_WORKSPACE_root(scr);
#ifdef DEBUG_WMH
    fprintf(stderr, "Updating _WIN_AREA on root\n");
    fflush(stderr);
#endif
    Upd_WIN_AREA(scr);
#ifdef DEBUG_WMH
    fprintf(stderr, "Updating _WIN_WORKAREA on root\n");
    fflush(stderr);
#endif
    Upd_WIN_WORKAREA(scr);
#ifdef DEBUG_WMH
    fprintf(stderr, "Updating __SWM_VROOT on virtual roots\n");
    fflush(stderr);
#endif
    Upd__SWM_VROOT(scr);

#ifdef DEBUG_WMH
    fprintf(stderr, "Updating _WIN_SUPPORTING_WM_CHECK on root\n");
    fflush(stderr);
#endif
    Upd_WIN_SUPPORTING_WM_CHECK(scr);
}

/** @brief Prepare the window manager for restart in the WinWM/WMH sense.
  *
  * This function is called when TWM prepares to restart, but is not called when
  * TWM shuts down gracefully.  In that case, TermWmh() is called instead.
  */
void
RestartWmh(ScreenInfo *scr)
{
    Del_WIN_SUPPORTING_WM_CHECK(scr);
    Del_WIN_PROTOCOLS(scr);
    Del_WIN_CLIENT_LIST(scr);
    // Del_WIN_WORKSPACE_COUNT(scr);
    // Del_WIN_WORKSPACE_root(scr);
    // Del_WIN_WORKSPACE_NAMES(scr);
    Del_WIN_DESKTOP_BUTTON_PROXY(scr);
    // Del_WIN_AREA_COUNT(scr);
    // Del_WIN_AREA(scr);
    Del_WIN_WORKAREA(scr);
    /* Don't worry about property notifications, the event loop will never be
       re-entered. */
}

/** @brief Terminate the window manager in the WinWM/WMH sense.
  *
  * This function is called when TWM prepares to shut down gracefully, either to
  * start another window manager or exit altogether.
  */
void
TermWmh(ScreenInfo *scr)
{
    Del_WIN_SUPPORTING_WM_CHECK(scr);
    Del_WIN_PROTOCOLS(scr);
    Del_WIN_CLIENT_LIST(scr);
    Del_WIN_WORKSPACE_COUNT(scr);
    Del_WIN_WORKSPACE_root(scr);
    Del_WIN_WORKSPACE_NAMES(scr);
    Del_WIN_DESKTOP_BUTTON_PROXY(scr);
    Del_WIN_AREA_COUNT(scr);
    Del_WIN_AREA(scr);
    Del_WIN_WORKAREA(scr);
    /* Don't worry about property notifications, the event loop will never be
       re-entered. */
}

/** @} */

/** @name Addition and Withdrawal of windows
  * CTWM treats withdrawal and deletion of windows identically.
  * @{ */

/** @brief Add a window in the WMH sense.
  * @param twin - the window to add
  *
  * This function is used to add a newly managed window in the WinWM/WMH sense
  * in response to a MapRequest of a managed window.    The entire TwmWindow
  * structure has been completed and we only need to add WinWM/WMH stuff.
  */
void
AddWindowWmh(ScreenInfo *scr, TwmWindow *twin)
{
    /* Get or set application window properties */
    Ret_WIN_LAYER(twin);
    Ret_WIN_STATE(scr, twin);
    Ret_WIN_WORKSPACE(scr, twin);
    Ret_WIN_EXPANDED_SIZE(twin);
    Ret_WIN_HINTS(twin);
    Ret_WIN_APP_STATE(twin);
    Ret_WIN_ICONS(twin);
    // Ret_WIN_WORKSPACES(twin);
    Ret_WIN_CLIENT_MOVING(twin);
    Ret_WIN_MAXIMIZED_GEOMETRY(scr, twin);
    // Ret_WIN_RESIZE(twin);
    // Ret_WIN_FOCUS(twin);

    /* Update the client list because a window was added. */
    Upd_WIN_CLIENT_LIST(scr);
}

void
UpdWindowWmh(ScreenInfo *scr, TwmWindow *twin)
{
    Upd_WIN_WORKSPACE(scr, twin);
    Upd_WIN_LAYER(twin);
    Upd_WIN_STATE(scr, twin);
    // Upd_WIN_EXPANDED_SIZE(twin);
    // Upd_WIN_HINTS(twin);
    // Upd_WIN_APP_STATE(twin);
    // Upd_WIN_ICONS(twin);
    Upd_WIN_WORKSPACES(scr, twin);
    // Upd_WIN_CLIENT_MOVING(twin);
    // Upd_WIN_MAXIMIZED_GEOMETRY(twin);
}

/** @brief Withdraw a window in the WMH sense.
  * @param twin - the window to delete
  *
  * This function is used to delete a managed window in the WinWM/WMH sense in
  * response to an UnmapRequest or DestroyNotify for a managed window.  The
  * TwmWindow structure has not yet been removed or prepared for deallocation:
  * we need to delete the WinWM/WMH stuff first.
  */
void
DelWindowWmh(ScreenInfo *scr, TwmWindow *twin)
{
    WmhWindow *wwin;

    /* Delete properties that should not hang around on unmapped windows in
       case a restart is performed.  Not all Del_* functions delete the
       property from the window.  All Del_* functions deallocate any local
       storage associated with the property. */
    Del_WIN_LAYER(twin);
    Del_WIN_STATE(twin);
    Del_WIN_WORKSPACE(twin);
    Del_WIN_EXPANDED_SIZE(twin);
    Del_WIN_HINTS(twin);
    Del_WIN_APP_STATE(twin);
    Del_WIN_ICONS(twin);
    // Del_WIN_WORKSPACES(twin);
    // Del_WIN_CLIENT_MOVING(twin);
    Del_WIN_MAXIMIZED_GEOMETRY(twin);
    // Del_WIN_RESIZE(twin);
    // Del_WIN_FOCUS(twin);

    /* Update the client list because a window was removed. */
    Upd_WIN_CLIENT_LIST(scr);
    /* Do not worry about property notifications, the context that would
       deliver them will be removed before the event loop is reentered. */

    /* free some things */
    wwin = &twin->wmh;

    if (wwin->icons != NULL) {
	XFree(wwin->icons);
	wwin->icons = NULL;
    }

}

/** @} */

/** @name Event loop hooks
  * @{ */

/** @brief Handle the receipt of client messages.
  *
  * Returns True when the event has been handled, False otherwise.
  */
Bool
HandleWinClientMessage(ScreenInfo *scr, TwmWindow *twin, XEvent *xev)
{
    XClientMessageEvent *event = &xev->xclient;
    Atom message_type = event->message_type;

    if (scr == NULL)
	return False;
    if (event->format != 32)
	return False;
    if (twin != NULL) {
	if (0) {
	} else if (message_type == _XA_WIN_LAYER) {
	    Rcv_WIN_LAYER(twin, event);
	} else if (message_type == _XA_WIN_STATE) {
	    Rcv_WIN_STATE(scr, twin, event);
	} else if (message_type == _XA_WIN_WORKSPACE) {
	    Rcv_WIN_WORKSPACE(scr, twin, event);
	} else
	    return False;
    } else {
	if (0) {
	} else if (message_type == _XA_WIN_WORKSPACE) {
	    Rcv_WIN_WORKSPACE_root(scr, event);
	} else if (message_type == _XA_WIN_AREA) {
	    Rcv_WIN_AREA(scr, event);
	} else if (message_type == _XA_WIN_AREA_COUNT) {
	    Rcv_WIN_AREA_COUNT(scr, event);
	} else
	    return False;
    }
    return True;
}

/** @brief Handle property change notifications.
  *
  * Returns True when the event has been handled, False otherwise.
  */
Bool
HandleWinPropertyNotify(ScreenInfo *scr, TwmWindow *twin, XEvent *xev)
{
    XPropertyEvent *event = &xev->xproperty;
    Atom atom = event->atom;

    if (scr == NULL)
	return False;
    if (twin != NULL) {
	if (0) {
	} else if (atom == _XA_WIN_HINTS) {
	    Ret_WIN_HINTS(twin);
	} else if (atom == _XA_WIN_APP_STATE) {
	    Ret_WIN_APP_STATE(twin);
	} else if (atom == _XA_WIN_EXPANDED_SIZE) {
	    Ret_WIN_EXPANDED_SIZE(twin);
	} else if (atom == _XA_WIN_ICONS) {
	    Ret_WIN_ICONS(twin);
	} else if (atom == _XA_WIN_CLIENT_MOVING) {
	    Ret_WIN_CLIENT_MOVING(twin);
	} else
	    return False;
    } else {
	if (0) {
	} else if (atom == _XA_WIN_WORKSPACE_NAMES) {
	    Ret_WIN_WORKSPACE_NAMES(scr);
	} else if (atom == _XA_WIN_WORKSPACE_COUNT) {
	    Ret_WIN_WORKSPACE_COUNT(scr);
	} else if (atom == _XA_WIN_AREA_COUNT) {
	    Ret_WIN_AREA_COUNT(scr);
	} else
	    return False;
    }
    return True;
}

/** @} */

// vim: set sw=4 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
