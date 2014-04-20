#include <stdio.h>
#include "mwmh.h"
#include "screen.h"
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/Xmu/CharSet.h>

/** @file
  * Motif/MWMH Implementation File
  *
  * This file contains a generic implementation of the MOTIF/MWMH.
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

Atom _XA_WM_DESKTOP;
Atom _XA_MOTIF_BINDINGS;
Atom _XA_MOTIF_DEFAULT_BINDINGS;
Atom _XA_MOTIF_WM_HINTS;
Atom _XA_MOTIF_WM_MESSAGES;
Atom _XA_MOTIF_WM_OFFSET;
Atom _XA_MOTIF_WM_MENU;
Atom _XA_MOTIF_WM_INFO;

Atom _XA_DT_WORKSPACE_HINTS;
Atom _XA_DT_WORKSPACE_PRESENCE;
Atom _XA_DT_WORKSPACE_LIST;
Atom _XA_DT_WORKSPACE_CURRENT;
Atom _XA_DT_WORKSPACE_INFO;
Atom _XA_DT_WM_HINTS;
Atom _XA_DT_WM_REQUEST;
Atom _XA_DT_WORKSPACE_EMBEDDED_CLIENTS;
Atom _XA_DT_WMSAVE_HINT;

Atom _XA_MOTIF_DRAG_WINDOW;
Atom _XA_MOTIF_DRAG_PROXY_WINDOW;
Atom _XA_MOTIF_DRAG_ATOM_PAIRS;
Atom _XA_MOTIF_DRAG_TARGETS;
Atom _XA_MOTIF_DRAG_INITIATOR_INFO;
Atom _XA_MOTIF_DRAG_RECEIVER_INFO;
Atom _XA_MOTIF_DRAG_MESSAGE;
Atom _XA_MOTIF_DRAG_AND_DROP_MESSAGE;

/* Manger selections:
 *
 * _MOTIF_ATOM_%d
 * _MOTIF_WM_QUERY_%d
 */

typedef struct MwmAtom {
    Atom *atom_p;
    const char *name;
} MwmAtom;

static const struct MwmAtom MwmAtoms[] = {
    {&_XA_WM_DESKTOP, "WM_DESKTOP"},
    {&_XA_MOTIF_BINDINGS, "_MOTIF_BINDINGS"},
    {&_XA_MOTIF_DEFAULT_BINDINGS, "_MOTIF_DEFAULT_BINDINGS"},
    {&_XA_MOTIF_WM_HINTS, "_MOTIF_WM_HINTS"},
    {&_XA_MOTIF_WM_MESSAGES, "_MOTIF_WM_MESSAGES"},
    {&_XA_MOTIF_WM_OFFSET, "_MOTIF_WM_OFFSET"},
    {&_XA_MOTIF_WM_MENU, "_MOTIF_WM_MENU"},
    {&_XA_MOTIF_WM_INFO, "_MOTIF_WM_INFO"},

    {&_XA_DT_WORKSPACE_HINTS, "_DT_WORKSPACE_HINTS"},
    {&_XA_DT_WORKSPACE_PRESENCE, "_DT_WORKSPACE_PRESENCE"},
    {&_XA_DT_WORKSPACE_LIST, "_DT_WORKSPACE_LIST"},
    {&_XA_DT_WORKSPACE_CURRENT, "_DT_WORKSPACE_CURRENT"},
    {&_XA_DT_WORKSPACE_INFO, "_DT_WORKSPACE_INFO"},
    {&_XA_DT_WM_HINTS, "_DT_WM_HINTS"},
    {&_XA_DT_WM_REQUEST, "_DT_WM_REQUEST"},
    {&_XA_DT_WORKSPACE_EMBEDDED_CLIENTS, "_DT_WORKSPACE_EMBEDDED_CLIENTS"},
    {&_XA_DT_WMSAVE_HINT, "_DT_WMSAVE_HINT"},

    {&_XA_MOTIF_DRAG_WINDOW, "_MOTIF_DRAG_WINDOW"},
    {&_XA_MOTIF_DRAG_PROXY_WINDOW, "_MOTIF_DRAG_PROXY_WINDOW"},
    {&_XA_MOTIF_DRAG_ATOM_PAIRS, "_MOTIF_DRAG_ATOM_PAIRS"},
    {&_XA_MOTIF_DRAG_TARGETS, "_MOTIF_DRAG_TARGETS"},
    {&_XA_MOTIF_DRAG_INITIATOR_INFO, "_MOTIF_DRAG_INITIATOR_INFO"},
    {&_XA_MOTIF_DRAG_RECEIVER_INFO, "_MOTIF_DRAG_RECEIVER_INFO"},
    {&_XA_MOTIF_DRAG_MESSAGE, "_MOTIF_DRAG_MESSAGE"},
    {&_XA_MOTIF_DRAG_AND_DROP_MESSAGE, "_MOTIF_DRAG_AND_DROP_MESSAGE"},

    {NULL, NULL}
};

static void
InternMwmAtoms(void)
{
    const struct MwmAtom *p;

    for (p = MwmAtoms; p->atom_p != NULL; p++)
	*(p->atom_p) = XInternAtom(dpy, p->name, False);
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
Get_MWM_WM_atoms(Window window, Atom property, const Atom **defs, unsigned *flags)
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

static inline int
cmp_atom_list(Atom *al1, Atom *al2)
{
    if (al1 == NULL && al2 == NULL)
	return (0);
    if (al1 == NULL)
	return (-1);
    if (al2 == NULL)
	return (1);
    for (; *al1 != None || *al2 != None; al1++, al2++) {
	if (*al1 == None)
	    return (-1);
	if (*al2 == None)
	    return (1);
	if (*al1 < *al2)
	    return (-1);
	if (*al1 > *al2)
	    return (1);
    }
    return (0);
}

int
cmp_info(MwmInfo *i1, MwmInfo *i2)
{
    if (i1->flags < i2->flags)
	return (-1);
    if (i1->flags > i2->flags)
	return (1);
    if (i1->wm_window < i2->wm_window)
	return (-1);
    if (i1->wm_window > i2->wm_window)
	return (1);
    return (0);
}

static int
cmp_hints(MwmHints *h1, MwmHints *h2)
{
    if (h1->flags < h2->flags)
	return (-1);
    if (h1->flags > h2->flags)
	return (1);
    if (h1->functions < h2->functions)
	return (-1);
    if (h1->functions > h2->functions)
	return (1);
    if (h1->decorations < h2->decorations)
	return (-1);
    if (h1->decorations > h2->decorations)
	return (1);
    if (h1->input_mode < h2->input_mode)
	return (-1);
    if (h1->input_mode > h2->input_mode)
	return (1);
    if (h1->status < h2->status)
	return (-1);
    if (h1->status > h2->status)
	return (1);
    return (0);
}

/** @name Root window properties.
  * @{ */

/** @name _MOTIF_WM_INFO, flags wm_window _MOTIF_WM_INFO/32
  *
  * This property is placed on the root window to identify the startup mode and
  * the main window manager window.  The main window is similar to the
  * supporting window manager check window of WinWM/WMH or NetWM/EWMH, except
  * that this is not a recursive property.
  *
  * The Lesstif window manager (MWM) places this property on the root window and
  * sets the wm_window field (unfortunately)  to the root window.  Openmotif MWM
  * sets the wm_window field to the primary window manager window (the one with
  * WM_CLASS of 'mwm', 'Mwm').  To indicate motif window manager support, we set
  * the property on the root window to point to the primary check window (the
  * supporting wm check window for WinWM and NetWM as well as the owner of the
  * ICCCM 2.0 WM_Sn selection for the screen).
  *
  * @{ */

/** @brief Set the motif window manager information property.
  * @param root - root window
  * @param info - motif window manager information structure
  */
static void
Set_MOTIF_WM_INFO(Window root, MwmInfo *info)
{
#ifdef DEBUG_MWMH
    fprintf(stderr, "%s on root 0x%08lx\n", __FUNCTION__, root);
    fflush(stderr);
#endif
    XChangeProperty(dpy, root, _XA_MOTIF_WM_INFO, _XA_MOTIF_WM_INFO, 32, PropModeReplace,
		    (unsigned char *) info, 2);
}

/** @brief Initialize the motif window manager information property.
  * @param scr - screen
  */
void
Ini_MOTIF_WM_INFO(ScreenInfo *scr)
{
    MwmInfo info;

    TwmGetMwmInfo(scr, &info);
    Set_MOTIF_WM_INFO(TwmMwmRoot(scr), &info);
    scr->mwmh.props._MOTIF_WM_INFO = 1;
    scr->mwmh.info = info;
}

/** @brief Update the motif window manager information property.
  * @param scr - screen
  */
void
Upd_MOTIF_WM_INFO(ScreenInfo *scr)
{
    MwmInfo info;

    TwmGetMwmInfo(scr, &info);
    if (!scr->mwmh.props._MOTIF_WM_INFO || scr->mwmh.info.flags != info.flags
	|| scr->mwmh.info.wm_window != info.wm_window) {
	Set_MOTIF_WM_INFO(TwmMwmRoot(scr), &info);
	scr->mwmh.props._MOTIF_WM_INFO = 1;
	scr->mwmh.info = info;
    }
}

/** @brief Delete the motif window manager information property.
  * @param scr - screen
  */
static void
Del_MOTIF_WM_INFO(ScreenInfo *scr)
{
    XDeleteProperty(dpy, TwmMwmRoot(scr), _XA_MOTIF_WM_INFO);
    scr->mwmh.props._MOTIF_WM_INFO = 0;
    scr->mwmh.info.flags = 0;
    scr->mwmh.info.wm_window = None;
}

/** @} */

/** @} */

/** @name Mwm info window properties.
  * @{ */

/** @name  _DT_WORKSPACE_LIST
  *
  * This property is a list of atoms.  Each atom represents a name of a
  * workspace.  The list is in "order" such that the first element is for the
  * first workspace and so on.  This property is placed on the mwm ("wmWindow")
  * window.
  *
  * The window manager sets this property.
  *
  * @{ */

/** @brief Set the workspace atom list.
  * @param info - MWM info window
  * @param list - atom list
  * @param count - number of atoms in the list
  */
static void
Set_DT_WORKSPACE_LIST(Window info, Atom *list, int count)
{
#ifdef DEBUG_MWMH
    fprintf(stderr, "%s on info 0x%08lx\n", __FUNCTION__, info);
    fflush(stderr);
#endif
    XChangeProperty(dpy, info, _XA_DT_WORKSPACE_LIST, XA_ATOM, 32, PropModeReplace,
		    (unsigned char *) list, count);
}

Bool
Get_DT_WORKSPACE_LIST(Window info, Atom **list, int *count)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0, n;
    long *prop = NULL;

#ifdef DEBUG_MWMH
    fprintf(stderr, "%s from info 0x%08lx\n", __FUNCTION__, info);
    fflush(stderr);
#endif
    status =
	XGetWindowProperty(dpy, info, _XA_DT_WORKSPACE_CURRENT, 0L, 32L, False, XA_ATOM,
			   &actual_type, &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 1) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    if ((*list = calloc(nitems + 1, sizeof(Atom))) == NULL) {
	XFree(prop);
	*count = 0;
	return False;
    }
    for (n = 0; n < nitems; n++)
	(*list)[n] = prop[n];
    (*list)[nitems] = None;
    *count = nitems;
    return True;
}

/** @brief Update the workspace atom list.
  * @param scr - screen
  *
  * This function only changes the property on the MWM info window when the
  * window atom list actually changes.
  *
  * This function should be called by the window manager at initial startup
  * (after establishing the MWM info window), and whenever a workspace label
  * changes or the number of workspaces in the list, or order of the list,
  * changes.
  */
void
Upd_DT_WORKSPACE_LIST(ScreenInfo *scr)
{
    Atom *list = NULL;
    int count = 0;

    TwmGetWorkspaceList(scr, &list, &count);
    if (!scr->mwmh.props._DT_WORKSPACE_LIST || cmp_atom_list(scr->mwmh.list, list) != 0) {
	Set_DT_WORKSPACE_LIST(TwmMwmInfo(scr), list, count);
	scr->mwmh.props._DT_WORKSPACE_LIST = 1;
	free(scr->mwmh.list);
	scr->mwmh.list = list;
    }
}

static void
Del_DT_WORKSPACE_LIST(ScreenInfo *scr)
{
    XDeleteProperty(dpy, TwmMwmInfo(scr), _XA_DT_WORKSPACE_LIST);
    scr->mwmh.props._DT_WORKSPACE_LIST = 0;
    free(scr->mwmh.list);
    scr->mwmh.list = NULL;
}


/** @} */

/** @name _DT_WORKSPACE_CURRENT
  *
  * This property is a single atom, representing the current workspace.  It is
  * updated each time the workspace changes.  This property is placed on the mwm
  * window.
  *
  * The window manager sets this property.
  *
  * @{ */

/** @brief Set the current workspace atom.
  * @param info - MWM info window
  * @param workspace - workspace atom to set
  */
static void
Set_DT_WORKSPACE_CURRENT(Window info, Atom workspace)
{
#ifdef DEBUG_MWMH
    fprintf(stderr, "%s on info 0x%08lx\n", __FUNCTION__, info);
    fflush(stderr);
#endif
    XChangeProperty(dpy, info, _XA_DT_WORKSPACE_CURRENT, XA_ATOM, 32,
		    PropModeReplace, (unsigned char *) &workspace, 1);
}

Bool
Get_DT_WORKSPACE_CURRENT(Window info, Atom *workspace)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

#ifdef DEBUG_MWMH
    fprintf(stderr, "%s from info 0x%08lx\n", __FUNCTION__, info);
    fflush(stderr);
#endif
    status =
	XGetWindowProperty(dpy, info, _XA_DT_WORKSPACE_CURRENT, 0L, 1L, False,
		XA_ATOM, &actual_type, &actual_format, &nitems, &bytes_after,
		(unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 1) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    *workspace = *(Atom *)prop;
    XFree(prop);
    return True;
}

/** @brief update the current workspace atom.
  * @param scr - screen
  *
  * This function only actually changes the property on the MWM info window when
  * the workspace atom has actually changed (either because the lable for
  * the current workspace has changed, or the current workspace has changed).
  * 
  * This function should be called by the window manager when it first starts up
  * (after establishing the MWM info window), and whenever the label of the
  * current workspace changes, or the current workspace changes.
  */
void
Upd_DT_WORKSPACE_CURRENT(ScreenInfo *scr)
{
    Atom current = None;

    TwmGetWorkspaceCurrent(scr, &current);
    if (!scr->mwmh.props._DT_WORKSPACE_CURRENT || scr->mwmh.current != current) { 
	Set_DT_WORKSPACE_CURRENT(TwmMwmInfo(scr), current);
	scr->mwmh.props._DT_WORKSPACE_CURRENT = 1;
	scr->mwmh.current = current;
    }
}

static void
Del_DT_WORKSPACE_CURRENT(ScreenInfo *scr)
{
    XDeleteProperty(dpy, TwmMwmInfo(scr), _XA_DT_WORKSPACE_CURRENT);
    scr->mwmh.props._DT_WORKSPACE_CURRENT = 0;
    scr->mwmh.current = None;
}



/** @} */

/** @name _DT_WORKSPACE_INFO_<name>
  *
  * There is one property of this form for each workspace in _DT_WORKSPACE_LIST.
  * This property is a sequence of ISO-LATIN1 NULL-terminated strings
  * representing the elements in a structure.  This information was formerly in
  * _DT_WORKSPACE_INFO but was broken out to allow for extensibility.  This
  * property is placed on the mwm window.
  *
  * The window manager sets this property.
  *
  * The property contains:
  *
  * %s    title of the workspace
  * %d    color set
  * 0x%lx backdrop background
  * 0x%lx backdrop foreground
  * 0x%lx backdrop name atom
  * %d    number of backdrop windows
  * 0x%lx backdrop window
  * 0x%lx backdrop window ... for number of backdrop windows
  *
  * @{ */

/** @} */

/** @name _DT_WM_REQUEST
  *
  * This property is of type string that is used to communication function
  * requests to dtwm.  This property is placed on the mwm window.  Dtwm listens
  * for changes to this property and dequeues requests off the top of the list.
  * Requests are NULL-terminated strings in the format:
  *
  *  <req_type> <req_parms>
  *
  * Each request ends with a literal '\0' character to insure separation from
  * the next request.
  *
  * Clients must always add requests to the end of the property (mode =
  * PropModeAppend).  Use of convenience routines is recommended since they take
  * care of proper formatting of the requests.
  *
  * This property is changed by the client and window manager.
  *
  * An example is appending "f.restart" to restart the window manager.  Another
  * is "f.change_backdrop /usr/share/images/penguins/penguin.jpg 800003f".
  *
  * @{ */


static void
Del_DT_WM_REQUEST(ScreenInfo *scr)
{
    XDeleteProperty(dpy, TwmMwmInfo(scr), _XA_DT_WM_REQUEST);
    Scr->mwmh.props._DT_WM_REQUEST = 0;
    free(Scr->mwmh.request);
    Scr->mwmh.request = NULL;
}

/** @} */

/** @name _DT_WORKSPACE_EMBEDDED_CLIENTS _DT_WORKSPACE_EMBEDDED_CLIENTS/32
  *
  * This property is a list (array) of top-level windows that are embedded in
  * the front panel of the window manager.  They would not be picked up
  * ordinarily by a session manager in a normal search for top-level windows
  * because they are reparented to the front panel which itself is a top-level
  * window.
  *
  * @{ */

/** @} */

/** @} */

/** @name Mwm client window properties.
  * @{ */

/** @name WM_PROTOCOLS, ATOM[]/32
  *
  * The ICCCM 2.0 WM_PROTOCOLS property that is set by a client on an
  * application window contians the atoms of supported protocols.  Currently
  * these are WM_DELETE and WM_SAVE_YOURSELF.  NetWM/EWMH defines _NET_WM_PING
  * and _NET_WM_SYNC_REQUEST as well.  Motif/MWMH defines _MOTIF_WM_MESSAGES and
  * _MOTIF_WM_OFFSET.
  *
  * @{ */

static const Atom *MwmWmProtocols[] = {
    TWM_ATOM_ENTRY(_WM_TAKE_FOCUS),
    TWM_ATOM_ENTRY(_WM_SAVE_YOURSELF),
    TWM_ATOM_ENTRY(_WM_DELETE_WINDOW),
#ifdef EWMH
    TWM_ATOM_ENTRY(_NET_WM_PING),
    TWM_ATOM_ENTRY(_NET_WM_SYNC_REQUEST),
#endif				/* EWMH */
    TWM_ATOM_ENTRY(_MOTIF_WM_MESSAGES),
    TWM_ATOM_ENTRY(_MOTIF_WM_OFFSET),
    [WM_PROTOCOLS_last] = NULL
};

static Bool
Get_MWM_WM_PROTOCOLS(Window window, unsigned *protocols)
{
#ifdef DEBUG_MWMH
    fprintf(stderr, "%s from window 0x%08lx\n", __FUNCTION__, window);
    fflush(stderr);
#endif
    return Get_MWM_WM_atoms(window, _XA_WM_PROTOCOLS, MwmWmProtocols, protocols);
}

static void
Ini_MWM_WM_PROTOCOLS(TwmWindow *twin)
{
    Bool present;
    unsigned protocols = 0;

    present = Get_MWM_WM_PROTOCOLS(twin->w, &protocols);
    twin->mwmh.props.WM_PROTOCOLS = present;
    twin->mwmh.protocols = protocols;
}

void
Ret_MWM_WM_PROTOCOLS(TwmWindow *twin)
{
    Bool present;
    unsigned protocols = 0;

    present = Get_MWM_WM_PROTOCOLS(twin->w, &protocols);
    twin->mwmh.props.WM_PROTOCOLS = present;
    twin->mwmh.protocols = protocols;
}

/** @} */

/** @name WM_DESKTOP, WM_DESKTOP/32
  *
  * @{ */

/** @} */

/** @name _MOTIF_BINDINGS, STRING/8
  *
  * Provides the bindings of some buttons and keys to the Motif toolkit buttons
  * and keys.  This is very Motif specific and unnecessary when mimicing MWM
  * behavior.  We do not actually get or set this property.
  *
  * @{ */

/** @} */

/** @name _MOTIF_DEFAULT_BINDINGS, STRING/8
  *
  * Provide the default bindings of some buttons and keys to Motif toolkit
  * buttons and keys.  This is very Motif specific and unnecessary when mimicing
  * MWM behavior.  We do not actually get or set this property.
  *
  * @{ */

/** @} */

/** @name _MOTIF_WM_HINTS, flags functions decorations input_mode status _MOTIF_WM_HINTS/8
  *
  * This property is provided on a window before it is initially mapped and
  * specifies the functions, decorations, input_mode and status of the
  * application window.
  *
  * The flags field indicates the presence of the other fields with a bitmask as
  * follows:
  *
  *  MWM_HINTS_FUNCTIONS         - the functions field is valid
  *  MWM_HINTS_DECORATIONS       - the decorations field is valid
  *  MWM_HINTS_INPUT_MODE        - the input_mode field is valid
  *  MWM_HINTS_STATUS            - the status field is valid
  *
  * The functions field is a bitmask of any of the following:
  *
  *  MWM_FUNC_ALL                - all functions
  *  MWM_FUNC_RESIZE             - whether the window can be resized
  *  MWM_FUNC_MOVE               - whether the window can be moved
  *  MWM_FUNC_MINIMIZE           - whether the window can be minimized
  *  MWM_FUNC_MAXIMIZE           - whether the window can be maximized
  *  MWM_FUNC_CLOSE              - whether the window can be closed
  *
  * The decorations field is a bitmask of any of the following:
  *
  *  MWM_DECOR_ALL               - all decorations
  *  MWM_DECOR_BORDER            - whether a decorative border is provided
  *  MWM_DECOR_RESIZEH           - whether resize handles are provided
  *  MWM_DECOR_TITLE             - whether a title bar is provided
  *  MWM_DECOR_MENU              - whether a menu button is provided
  *  MWM_DECOR_MINIMIZE          - whether a minimize button is provided
  *  MWM_DECOR_MAXIMIZE          - whether a maximize button is provided
  *
  * The input_mode field is one of the following:
  *
  *  MWM_INPUT_MODELESS          - normal
  *  MWM_INPUT_APPLICATION_MODAL - modal for window group
  *  MWM_INPUT_SYSTEM_MODAL      - modal for all windows
  *
  * The status field is a bitmask of any of the following:
  *
  *  MWM_TEAROFF_WINDOW          - window is a tear-off menu
  *
  * @{ */

/** @brief Set the hints on a window.
  * @param window - window to set hints on
  * @param hints - the hints to set
  */
static void
Set_MOTIF_WM_HINTS(Window window, MwmHints *hints)
{
#ifdef DEBUG_MWMH
    fprintf(stderr, "%s on window 0x%08lx\n", __FUNCTION__, window);
    fflush(stderr);
#endif
    XChangeProperty(dpy, window, _XA_MOTIF_WM_HINTS, _XA_MOTIF_WM_HINTS, 32,
	    PropModeReplace, (unsigned char *) hints, 5);
}

/** @brief Get the hints from a window.
  * @param window - window from which to get
  * @param hints - where to store the hints
  * @return Bool - returns true when successfully retrieved
  */
static Bool
Get_MOTIF_WM_HINTS(Window window, MwmHints *hints)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

#ifdef DEBUG_MWMH
    fprintf(stderr, "%s from window 0x%08lx\n", __FUNCTION__, window);
    fflush(stderr);
#endif
    status =
	XGetWindowProperty(dpy, window, _XA_MOTIF_WM_HINTS, 0L, 5L, False,
			   _XA_MOTIF_WM_HINTS, &actual_type, &actual_format, &nitems,
			   &bytes_after, (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 3) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    hints->flags = prop[0];
    hints->functions = prop[1];
    hints->decorations = prop[2];
    hints->input_mode = nitems > 3 ? prop[3] : 0;
    hints->status = nitems > 4 ? prop[4] : 0;
    XFree(prop);
    return True;
}

/** @brief Update the hints for a window.
  * @param scr - screen
  * @param twin - TWM window
  */
void
Upd_MOTIF_WM_HINTS(ScreenInfo *scr, TwmWindow *twin)
{
    MwmHints hints = { 0, 0, 0, 0, 0 };

    TwmGetMwmHints(scr, twin, &hints);
    if (!twin->mwmh.props._MOTIF_WM_HINTS || cmp_hints(&twin->mwmh.hints, &hints) != 0) {
	Set_MOTIF_WM_HINTS(twin->w, &hints);
	twin->mwmh.props._MOTIF_WM_HINTS = 1;
	twin->mwmh.hints = hints;
    }
}

static void
Ini_MOTIF_WM_HINTS(ScreenInfo *scr, TwmWindow *twin)
{
    Bool present;
    MwmHints hints = { 0, 0, 0, 0, 0 };

    if ((present = Get_MOTIF_WM_HINTS(twin->w, &hints)))
	TwmIniMwmHints(scr, twin, &hints);
    twin->mwmh.props._MOTIF_WM_HINTS = present;
    twin->mwmh.hints = hints;
}

/** @brief Retrieve or set the hints for a window.
  * @param scr- screen
  * @param twin - TWM window
  *
  * This function should be called when the client window is initially managed,
  * and whenever this property changes after that.
  */
static void
Ret_MOTIF_WM_HINTS(ScreenInfo *scr, TwmWindow *twin)
{
    Bool present;
    MwmHints hints = { 0, 0, 0, 0, 0 };

    if ((present = Get_MOTIF_WM_HINTS(twin->w, &hints)))
	TwmSetMwmHints(scr, twin, &hints);
    twin->mwmh.props._MOTIF_WM_HINTS = present;
    twin->mwmh.hints = hints;
    Upd_MOTIF_WM_HINTS(scr, twin);
}

/** @} */

/** @name _MOTIF_WM_MESSAGES
  *
  * @{ */

/** @} */

/** @name _MOTIF_WM_OFFSET
  *
  * This atom is used by a client in WM_PROTOCOLS to indicate its desire to
  * receive _MOTIF_WM_OFFSET messages.  When the protocol is present, mwm and
  * dtwm will generate _MOTIF_WM_OFFSET messages to the client when the client
  * window is initially mapped to indicate the offset from the gravity reference
  * point of the client window within the decorative frame and borders.
  *
  * A _MOTIF_WM_OFFSET message is a _MOTIF_WM_MESSAGES message type that
  * contains the _MOTIF_WM_OFFSET atom in the data.l[0] element.  The data.l[1]
  * and data.l[2] components contain the x and y of the client window within its
  * decorative borders and frame considering the gvavity specified by the client
  * for the window in the WM_HINTS property.
  *
  * @{ */

void
Snd_MOTIF_WM_OFFSET(ScreenInfo *scr, TwmWindow *twin)
{
    long gravity, bw, x_off, y_off;
    XClientMessageEvent event;

    event.display = dpy;
    event.type = ClientMessage;
    event.window = twin->w;
    event.message_type = _XA_MOTIF_WM_MESSAGES;
    event.format = 32;
    event.data.l[0] = _XA_MOTIF_WM_OFFSET;

    TwmGetMwmOffset(scr, twin, &gravity, &bw, &x_off, &y_off);

    switch (gravity) {
    case NorthWestGravity:
    default:
	event.data.l[1] = x_off;
	event.data.l[2] = y_off;
	break;
    case NorthGravity:
	event.data.l[1] = bw;
	event.data.l[2] = y_off;
	break;
    case NorthEastGravity:
	event.data.l[1] = -(x_off - (2 * bw));
	event.data.l[2] = y_off;
	break;
    case EastGravity:
	event.data.l[1] = -(x_off - (2 * bw));
	event.data.l[2] = bw + (y_off - x_off) / 2;
	break;
    case SouthEastGravity:
	event.data.l[1] = -(x_off - (2 * bw));
	event.data.l[2] = -(y_off - (2 * bw));
	break;
    case SouthGravity:
	event.data.l[1] = bw;
	event.data.l[2] = -(x_off - (2 * bw));
	break;
    case SouthWestGravity:
	event.data.l[1] = x_off;
	event.data.l[2] = -(x_off - (2 * bw));
	break;
    case WestGravity:
	event.data.l[1] = x_off;
	event.data.l[2] = bw + (y_off - x_off) / 2;
	break;
    case CenterGravity:
	event.data.l[1] = 0;
	event.data.l[2] = (y_off - x_off) / 2;
	break;
    }
    event.data.l[3] = 0;
    event.data.l[4] = 0;

    XSendEvent(dpy, twin->w, False, NoEventMask, (XEvent *) &event);
}

/** @} */

/** @name _MOTIF_WM_MENU
  *
  * @{ */

/** @} */

/** @name _DT_WORKSPACE_HINTS, version flags wsflags numWorkspaces [workspace ...] _DT_WORKSPACE_HINTS/32
  *
  * This property is a list of atoms placed by a client on its top level
  * window(s).  Each atom is an "interned" string name for a workspace.  The
  * workspace manager looks at this property when it manages the window (e.g.
  * when the window is mapped) and will place the window in the workspace listed.
  *
  * The client sets this property.
  *
  * The version field is alway one (1).
  *
  * The flags field indicates the valid field and contains a bitmask of the
  * following bits:
  *
  *  DT_WORKSPACE_HINTS_WSFLAGS    - wsflags field is valid
  *  DT_WORKSPACE_HINTS_WORKSPACES - numWorkspaces field is valid
  *
  * The wsflags field is a bitmask containing the following bits:
  *
  *  DT_WORKSPACE_FLAGS_OCCUPY_ALL - occupy all workspaces
  *
  * @{ */

void
Set_DT_WORKSPACE_HINTS(Window window, DtWmWorkspaceHints *hints)
{
    int n;
    int len =
	4 + ((hints->flags & DT_WORKSPACE_HINTS_WORKSPACES) ? hints->numWorkspaces : 0);
    long *prop = calloc(len, sizeof(long));

#ifdef DEBUG_MWMH
    fprintf(stderr, "%s on window 0x%08lx\n", __FUNCTION__, window);
    fflush(stderr);
#endif
    if (prop == NULL)
	return;
    prop[0] = hints->version;
    prop[1] = hints->flags;
    prop[2] = (hints->flags & DT_WORKSPACE_HINTS_WSFLAGS) ? hints->wsflags : 0L;
    prop[3] = (hints->flags & DT_WORKSPACE_HINTS_WORKSPACES) ? hints->numWorkspaces : 0L;
    if (hints->flags & DT_WORKSPACE_HINTS_WORKSPACES)
	for (n = 0; n < (int) hints->numWorkspaces; n++)
	    prop[4 + n] = hints->workspaces[n];
    XChangeProperty(dpy, window, _XA_DT_WORKSPACE_HINTS, _XA_DT_WORKSPACE_HINTS, 32,
		    PropModeReplace, (unsigned char *) prop, len);
    free(prop);
}

static Bool
Get_DT_WORKSPACE_HINTS(Window window, DtWmWorkspaceHints *hints)
{
    Atom actual_type = None;
    int status, actual_format = 0, n;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

#ifdef DEBUG_MWMH
    fprintf(stderr, "%s from window 0x%08lx\n", __FUNCTION__, window);
    fflush(stderr);
#endif
    status =
	XGetWindowProperty(dpy, window, _XA_DT_WORKSPACE_HINTS, 0L, 4L, False,
			   _XA_DT_WORKSPACE_HINTS, &actual_type, &actual_format, &nitems,
			   &bytes_after, (unsigned char **) &prop);
    if (status != Success || actual_type == None || nitems < 4) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    hints->version = prop[0];
    hints->flags = prop[1];
    hints->wsflags = (hints->flags & DT_WORKSPACE_HINTS_WSFLAGS) ? prop[2] : 0;
    hints->numWorkspaces = (hints->flags & DT_WORKSPACE_HINTS_WORKSPACES) ? prop[3] : 0;
    if (nitems < 4 + hints->numWorkspaces) {
	XFree(prop);
	return False;
    }
    free(hints->workspaces);
    hints->workspaces = NULL;
    if (nitems > 4)
	if ((hints->workspaces = calloc(nitems - 4, sizeof(Atom))) != NULL)
	    for (n = 0; n < nitems - 4; n++)
		hints->workspaces[n] = prop[n + 4];
    XFree(prop);
    return True;
}

void
Upd_DT_WORKSPACE_HINTS(ScreenInfo *scr, TwmWindow *twin)
{
}

void
Ini_DT_WORKSPACE_HINTS(ScreenInfo *scr, TwmWindow *twin)
{
    Bool present;
    DtWmWorkspaceHints wshints = { 0, 0, 0, 0, NULL };

    if ((present = Get_DT_WORKSPACE_HINTS(twin->w, &wshints)))
	TwmIniWorkspaceHints(scr, twin, &wshints);
    twin->mwmh.props._DT_WORKSPACE_HINTS = present;
    free(twin->mwmh.wshints.workspaces);
    twin->mwmh.wshints = wshints;
}

void
Ret_DT_WORKSPACE_HINTS(ScreenInfo *scr, TwmWindow *twin)
{
    Bool present;
    DtWmWorkspaceHints wshints = { 0, 0, 0, 0, NULL };

    if ((present = Get_DT_WORKSPACE_HINTS(twin->w, &wshints)))
	TwmSetWorkspaceHints(scr, twin, &wshints);
    twin->mwmh.props._DT_WORKSPACE_HINTS = present;
    free(twin->mwmh.wshints.workspaces);
    twin->mwmh.wshints = wshints;
}

/** @} */

/** @name _DT_WORKSPACE_PRESENCE
  *
  * This property is a list of atoms placed on a client by dtwm.  Each atom is
  * an "interned" string name for a workspace.  This property lists the
  * workspaces that this client lives in.
  *
  * The window manager sets this property.
  *
  * @{ */

/** @brief Set the workspace presence for a window.
  * @param window - window for which to set
  * @param presence - list of workspace atoms to set
  * @param count - number of workspace atoms in the list
  */
static void
Set_DT_WORKSPACE_PRESENCE(Window window, Atom *presence, int count)
{
#ifdef DEBUG_MWMH
    fprintf(stderr, "%s on window 0x%08lx\n", __FUNCTION__, window);
    fflush(stderr);
#endif
    XChangeProperty(dpy, window, _XA_DT_WORKSPACE_PRESENCE, _XA_DT_WORKSPACE_PRESENCE, 32,
		    PropModeReplace, (unsigned char *) presence, count);
}

/** @brief Update the workspace presence of a window.
  * @param scr - screen
  * @param twin - TWM window
  *
  * Updates the _DT_WORKSPACE_PRESENCE property on a client window.  This
  * function should be called by the window manager when the client window is
  * first managed and whenever the presence of the window on a desktop changes,
  * or a desktop label changes.
  */
void
Upd_DT_WORKSPACE_PRESENCE(ScreenInfo *scr, TwmWindow *twin)
{
    Atom *presence = NULL;
    int count = 0;

    TwmGetWorkspacePresence(scr, twin, &presence, &count);
    if (!twin->mwmh.props._DT_WORKSPACE_PRESENCE
	|| cmp_atom_list(twin->mwmh.presence, presence) != 0) {
	Set_DT_WORKSPACE_PRESENCE(twin->w, presence, count);
	twin->mwmh.props._DT_WORKSPACE_PRESENCE = 1;
	free(twin->mwmh.presence);
	twin->mwmh.presence = presence;
    } else
	free(presence);
}

void
Del_DT_WORKSPACE_PRESENCE(TwmWindow *twin)
{
    XDeleteProperty(dpy, twin->w, _XA_DT_WORKSPACE_PRESENCE);
    twin->mwmh.props._DT_WORKSPACE_PRESENCE = 0;
    free(twin->mwmh.presence);
    twin->mwmh.presence = NULL;
}

/** @} */

/** @name _DT_WM_HINTS
  *
  * This property requests specific window/workspace management behaviour.  The
  * functions member of the operty allows a client to enable or disable
  * workspace management functions.  The behaviour member is used to indicate
  * front panels and slide-ups.
  *
  * The client sets this property.
  *
  * @{ */

/** @brief Get the desktop hints for a window.
  * @param window - window to get from
  * @param hints - where to store the desktop hints
  */
static Bool
Get_DT_WM_HINTS(Window window, DtWmHints *hints)
{
    Atom actual_type = None;
    int status, actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    long *prop = NULL;

#ifdef DEBUG_MWMH
    fprintf(stderr, "%s from window 0x%08lx\n", __FUNCTION__, window);
    fflush(stderr);
#endif
    status =
	XGetWindowProperty(dpy, window, _XA_DT_WM_HINTS, 0L, 4L, False, _XA_DT_WM_HINTS,
			   &actual_type, &actual_format, &nitems, &bytes_after,
			   (unsigned char **) &prop);
    if (status != Success || actual_type != _XA_DT_WM_HINTS) {
	if (prop != NULL)
	    XFree(prop);
	return False;
    }
    *hints = *(DtWmHints *) prop;
    XFree(prop);
    return True;
}

static void
Ini_DT_WM_HINTS(TwmWindow *twin)
{
    Bool present;
    DtWmHints hints = { 0, 0, 0, 0 };

    if ((present = Get_DT_WM_HINTS(twin->w, &hints)))
	TwmIniDtWmHints(twin, &hints);
    twin->mwmh.props._MOTIF_WM_HINTS = present;
    twin->mwmh.dthints = hints;
}

/** @brief Retrieve the desktop hints for a window.
  * @param twin - TWM window
  *
  * This function should be called when a client window is initially managed and
  * whenever the _DT_WM_HINTS property changes on the window after that.
  */
static void
Ret_DT_WM_HINTS(TwmWindow *twin)
{
    Bool present;
    DtWmHints hints = { 0, 0, 0, 0 };

    if ((present = Get_DT_WM_HINTS(twin->w, &hints)))
	TwmSetDtWmHints(twin, &hints);
    twin->mwmh.props._MOTIF_WM_HINTS = present;
    twin->mwmh.dthints = hints;
}

/** @} */

/** @} */

/** @name _MOTIF_DRAG_WINDOW
  *
  * @{ */

/** @} */

/** @name _MOTIF_DRAG_PROXY_WINDOW
  *
  * @{ */

/** @} */

/** @name _MOTIF_DRAG_ATOM_PAIRS
  *
  * @{ */

/** @} */

/** @name _MOTIF_DRAG_TARGETS
  *
  * @{ */

/** @} */

/** @name _MOTIF_DRAG_INITIATOR_INFO
  *
  * @{ */

/** @} */

/** @name _MOTIF_DRAG_RECEIVER_INFO
  *
  * @{ */

/** @} */

/** @name _MOTIF_DRAG_MESSAGE
  *
  * @{ */

/** @} */

/** @name _MOTIF_DRAG_AND_DROP_MESSAGE
  *
  * @{ */

/** @} */

/** @name Initialization and termination of the window manager.
  * @{ */

/** @brief Initialize the window manager in a Motif/MWMH sense.
  *
  * This function is called when TWM starts to initialize root window properties
  * and screen information on initialization of the window manager.
  */
void
InitMwmh(ScreenInfo *scr)
{
    InternMwmAtoms();

#ifdef DEBUG_MWMH
    fprintf(stderr, "Initializing Motif/MWMH support.\n");
    fflush(stderr);
#endif

#if 0
    /* Do not do this until all other things are set up in UpdateMwmh() */
    Ini_MOTIF_WM_INFO(scr);
#endif
}

/** @brief Update the window manager in the Motif/MWMH sense.
  *
  * This function can be used to update root window properties when it is known
  * that something might have changed but not known which property might have
  * changed.
  */
void
UpdateMwmh(ScreenInfo *scr)
{
#ifdef DEBUG_MWMH
    fprintf(stderr, "Updating _DT_WORKSPACE_LIST on info\n");
    fflush(stderr);
#endif
    Upd_DT_WORKSPACE_LIST(scr);
#ifdef DEBUG_MWMH
    fprintf(stderr, "Updating _DT_WORKSPACE_CURRENT on info\n");
    fflush(stderr);
#endif
    Upd_DT_WORKSPACE_CURRENT(scr);

#ifdef DEBUG_MWMH
    fprintf(stderr, "Updating _MOTIF_WM_INFO on root\n");
    fflush(stderr);
#endif
    Upd_MOTIF_WM_INFO(scr);
}

/** @brief Prepare the window manager for restart in the Motif/MWMH sense.
  *
  * This function is called when TWM prepares to restart, but is not called when
  * TWM shuts down gracefully.  In that case, TermMwmh() is called instead.
  *
  * Normally we would update the root window properties in preparation for
  * restart; however, MWM properties are on an info window that will disappear
  * so they won't be available after the restart anyhoo.  We could update the
  * properties on the root window and then point _MOTIF_WM_INFO to the root
  * window (which is what Lesstif MWM does), but it is not too clean.
  */
void
RestartMwmh(ScreenInfo *scr)
{
    /* switch info window to root window for restart */
    scr->mwmh.info.wm_window = TwmMwmRoot(scr);

#ifdef DEBUG_MWMH
    fprintf(stderr, "Updating _DT_WORKSPACE_LIST for restart\n");
    fflush(stderr);
#endif
    scr->mwmh.props._DT_WORKSPACE_LIST = 0;
    Upd_DT_WORKSPACE_LIST(scr);
#ifdef DEBUG_MWMH
    fprintf(stderr, "Updating _DT_WORKSPACE_CURRENT for restart\n");
    fflush(stderr);
#endif
    scr->mwmh.props._DT_WORKSPACE_CURRENT = 0;
    Upd_DT_WORKSPACE_CURRENT(scr);
#ifdef DEBUG_MWMH
    fprintf(stderr, "Updating _MOTIF_WM_INFO for restart\n");
    fflush(stderr);
#endif
    scr->mwmh.props._MOTIF_WM_INFO = 0;
    Upd_MOTIF_WM_INFO(scr);
}

/** @brief Terminate the window manager in the Motif/MWMH sense.
  *
  * This function is called when TWM prepares to shut down gracefully, either to
  * start another window manager or exit altogether.
  */
void
TermMwmh(ScreenInfo *scr)
{
    Del_DT_WORKSPACE_LIST(scr);
    Del_DT_WORKSPACE_CURRENT(scr);
    Del_DT_WM_REQUEST(scr);
    Del_MOTIF_WM_INFO(scr);
}

/** @} */

/** @name Addition and Withdrawal of windows
  * CTWM treats withdrawal and deletion of windows identically.
  * @{ */

/** @brief Add a window in the MWMH sense.
  * @param twin - the window to add
  *
  * This function is used to add a newly managed window in the Motif/MWMH sense
  * in response to a MapRequest of a managed window.    The entire TwmWindow
  * structure has been completed and we only need to add Motif/MWMH stuff.
  */
void
AddWindowMwmh(ScreenInfo *scr, TwmWindow *twin)
{
#ifdef DEBUG_MWMH
    fprintf(stderr, "Initializing WM_PROTOCOLS from window 0x%08x.\n", twin->w);
    fflush(stderr);
#endif
    Ini_MWM_WM_PROTOCOLS(twin);
#ifdef DEBUG_MWMH
    fprintf(stderr, "Initializing _MOTIF_WM_HINTS from window 0x%08x.\n", twin->w);
    fflush(stderr);
#endif
    Ini_MOTIF_WM_HINTS(scr, twin);
#ifdef DEBUG_MWMH
    fprintf(stderr, "Initializing _DT_WM_HINTS from window 0x%08x.\n", twin->w);
    fflush(stderr);
#endif
    Ini_DT_WM_HINTS(twin);
#ifdef DEBUG_MWMH
    fprintf(stderr, "Initializing _DT_WORKSPACE_HINTS from window 0x%08x.\n", twin->w);
    fflush(stderr);
#endif
    Ini_DT_WORKSPACE_HINTS(scr, twin);
}

void
UpdWindowMwmh(ScreenInfo *scr, TwmWindow *twin)
{
#ifdef DEBUG_MWMH
    fprintf(stderr, "Updating _DT_WORKSPACE_PRESENCE for window 0x%08x.\n", twin->w);
    fflush(stderr);
#endif
    Upd_DT_WORKSPACE_PRESENCE(scr, twin);
}

/** @brief Withdraw a window in the MWMH sense.
  * @param twin - the window to delete
  *
  * This function is used to delete a managed window in the Motif/MWMH sense in
  * response to an UnmapRequest or DestroyNotify for a managed window.  The
  * TwmWindow structure has not yet been removed or prepared for deallocation:
  * we need to delete the Motif/MWMH stuff first.
  */
void
DelWindowMwmh(ScreenInfo *scr, TwmWindow *twin)
{
    MwmWindow *mwin;

#ifdef DEBUG_MWMH
    fprintf(stderr, "Deleting _DT_WORKSPACE_PRESENCE from window 0x%08x.\n", twin->w);
    fflush(stderr);
#endif
    Del_DT_WORKSPACE_PRESENCE(twin);

    /* free some things */
    mwin = &twin->mwmh;

    free(mwin->bindings);
    mwin->bindings = NULL;
    free(mwin->default_bindings);
    mwin->default_bindings = NULL;
    free(mwin->menu);
    mwin->menu = NULL;
    free(mwin->wshints.workspaces);
    mwin->wshints.workspaces = NULL;
    free(mwin->presence);
    mwin->presence = NULL;
}

/** @} */

/** @name Event loop hooks
  * @{ */

/** @brief Handle the receipt of client messages.
  *
  * Returns True when the event has been handled, False otherwise.
  */
Bool
HandleMwmClientMessage(ScreenInfo *scr, TwmWindow *twin, XEvent *xev)
{
    XClientMessageEvent *event = &xev->xclient;
    Atom message_type = event->message_type;

    (void) message_type;
    if (scr == NULL)
	return False;
    if (event->format != 32)
	return False;
    if (twin != NULL) {
	if (0) {
	} else
	    return False;
    } else {
	if (0) {
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
HandleMwmPropertyNotify(ScreenInfo *scr, TwmWindow *twin, XEvent *xev)
{
    XPropertyEvent *event = &xev->xproperty;
    Atom atom = event->atom;

    if (scr == NULL)
	return False;
    if (twin != NULL) {
	if (0) {
	} else if (atom == _XA_MOTIF_WM_HINTS) {
	    Ret_MOTIF_WM_HINTS(scr, twin);
	} else if (atom == _XA_DT_WM_HINTS) {
	    Ret_DT_WM_HINTS(twin);
	} else if (atom == _XA_DT_WORKSPACE_HINTS) {
	    Ret_DT_WORKSPACE_HINTS(scr, twin);
	} else
	    return False;
    } else {
	if (0) {
	} else
	    return False;
    }
    return True;
}

/** @} */

// vim: set sw=4 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS foldmarker=@{,@} foldmethod=marker:
