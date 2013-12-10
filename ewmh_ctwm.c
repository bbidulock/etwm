#include <stdio.h>
#include "ewmh.h"
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
TwmNetRoot(ScreenInfo *scr)
{
    return scr->XineramaRoot;
}

Window
TwmNetManager(ScreenInfo *scr)
{
    return scr->ManagerWindow;
}

extern int CanChangeOccupation(TwmWindow **twm_winp);

static TwmWindow *
TwmCanChangeDesktop(TwmWindow *twin)
{
    if (twin->ewmh.props._NET_WM_ALLOWED_ACTIONS
	&& !(twin->ewmh.allowed & _NET_WM_ACTION_CHANGE_DESKTOP_BIT))
	return (NULL);
    if (!CanChangeOccupation(&twin))
	return (NULL);
    if (twin->ewmh.props._NET_WM_ALLOWED_ACTIONS
	&& !(twin->ewmh.allowed & _NET_WM_ACTION_CHANGE_DESKTOP_BIT))
	return (NULL);
    return (twin);
}

/** @file
  *
  * This file provides implementation of NetWM/EWMH hook functions for CTWM.
  * The NetWM/EWMH functions in ewmh.c are written to be independent of the TWM
  * variant that uses them.  These hook functions allow the NetWM/EWMH module to
  * interrogate and perform actions on the TWM variant of window manager.
  * Implementation modules are provided for TWM, VTWM, CTWM and ETWM.  This is
  * the implementation module for CTWM.
  */

/** @brief Get the client window list.
  * @param clients - where to return the list of windows
  * @param count - where to return the count
  *
  * Note that windows should be in oldest to newset mapping order.
  */
void
TwmGetClientList(ScreenInfo *scr, Window **clients, int *count)
{
    TwmWindow *twin;
    int number = 0;

    for (twin = scr->FirstWindow; twin != NULL; twin = twin->next)
	number++;
    if (*clients != NULL) {
	free(*clients);
	*clients = NULL;
    }
    if ((*clients = calloc(number + 1, sizeof(Window))) != NULL) {
	int i = 0;

	for (twin = scr->FirstWindow; twin != NULL; twin = twin->next)
	    (*clients)[i++] = twin->w;
	(*clients)[i] = None;
    }
    *count = *clients == NULL ? 0 : number;
}

/** @brief Get the client window list in stacking order.
  * @param stacking - where to return the list of windows
  * @param count - where to return the count.
  *
  * Note that windows should be in bottom-to-top stacking order.  We use
  * XQueryTree() here to give us the children of the current virtual root in
  * bottom to top stacking order and then translate the frame to the client
  * window using contexts.  This is good because it lets the X Server determine
  * stacking order.
  */
void
TwmGetClientListStacking(ScreenInfo *scr, Window **stacking, int *count)
{
    VirtualScreen *vs;
    Window *list = NULL;
    int number = 0;

    for (vs = scr->vScreenList; vs != NULL; vs = vs->next) {
	Window root = None, parent = None, *children = NULL, *newlist;
	int nchildren = 0, n;

	if (XQueryTree(dpy, vs->window, &root, &parent, &children, &nchildren) == 0
	    || nchildren == 0)
	    continue;
	if ((newlist = realloc(list, (number + nchildren + 1) * sizeof(Window))) != NULL) {
	    TwmWindow *twin;

	    list = newlist;
	    for (n = 0; n < nchildren; n++)
		if (XFindContext(dpy, children[n], TwmContext, (XPointer *) &twin) ==
		    XCSUCCESS && twin->w != None)
		    list[number++] = twin->w;
	    list[number] = None;
	}
	XFree(children);
    }
    *stacking = list;
    *count = number;
}

/** @brief Get the number of desktops.
  * @param number - where to return the number.
  *
  * Gets the count of desktops (workspaces) that are currently provided by the
  * window manager.
  */
void
TwmGetNumberOfDesktops(ScreenInfo *scr, int *number)
{
    *number = scr->workSpaceManagerActive ? scr->workSpaceMgr.count : 1;
}

/** @brief Set the number of desktops.
  * @param number - number of desktops to set.
  *
  * Set the count of desktops (workspace) that are currently provided by the
  * window manager.  This function must peform sanity checking of the number as
  * the number is exactly as provided in the client message.
  */
void
TwmSetNumberOfDesktops(ScreenInfo *scr, int number)
{
    int old_number = 0;

    TwmGetNumberOfDesktops(scr, &old_number);
    if (number == old_number)
	return;
    if (number < 1 || number > 32)
	return;
    /* TODO: actually increase or decrease the number of desktops. */
}

/** @brief Get the desktop geometry.
  * @param geometry - where to return geometry
  *
  * Get the desktop geometry.  CTWM does not support large virtual desktops, so
  * this is always the root window size of the current screen.
  */
void
TwmGetDesktopGeometry(ScreenInfo *scr, struct NetSize *geometry)
{
    geometry->width = scr->rootw;
    geometry->height = scr->rooth;
}

/** @brief Set the desktop geometry.
  * @param geometry - the desktop geometry
  *
  * Set the desktop geometry.  CTWM does not support large virtual desktops, so
  * this request is never honored.
  */
void
TwmSetDesktopGeometry(ScreenInfo *scr, struct NetSize *geometry)
{
    (void) scr;
    (void) geometry;
}

/** @brief Get the viewport for a desktop.
  * @param scr - screen
  * @param desktop - desktop of viewport
  * @param viewport - where to return the viewport position
  *
  * Get the viewport for desktop 'desktop'.  The coordinates returned are the
  * x-y position of the upper left corner of the viewport on the virtual
  * desktop.
  *
  * CTWM does not support large virtual desktops, so this is always (0,0) for
  * any desktop.
  */
void
TwmGetDesktopViewport(ScreenInfo *scr, int desktop, struct NetPosition *viewport)
{
    (void) scr;
    (void) desktop;
    viewport->x = 0;
    viewport->y = 0;
}

/** @brief Get the viewports for all desktops.
  * @param viewport - where to return the positions
  * @param viewports - where to return the number of viewports
  *
  * Get the list of viewports, one for each desktop.
  *
  * CTWM does not support large virtual desktops, so this is always (0,0) for
  * each desktop.
  */
void
TwmGetDesktopViewports(ScreenInfo *scr, struct NetPosition **viewport, int *viewports)
{
    int desktops = 1, n;
    struct NetPosition *list = NULL;

    TwmGetNumberOfDesktops(scr, &desktops);
    if (desktops < 1)
	return;
    if ((list = calloc(desktops, sizeof(*list))) == NULL)
	return;
    for (n = 0; n < desktops; n++)
	TwmGetDesktopViewport(scr, n, &list[n]);
    free(*viewport);
    *viewport = list;
    *viewports = desktops;
}

/** @brief Set the viewport for a given desktop.
  * @param desktop - desktop for which to set the viewport
  * @param viewport - position of the viewport
  *
  * Set the viewport position for desktop 'desktop'.  The coordinates are the
  * x-y coordinates of the upper left corner of the viewport on the virtual
  * desktop.
  *
  * CTWM does not support large virtual desktops, so this is always (0,0) and
  * this request is never honored.
  */
void
TwmSetDesktopViewport(ScreenInfo *scr, int desktop, struct NetPosition *viewport)
{
    (void) scr;
    (void) desktop;
    (void) viewport;
}

/** @brief Get the index of the current desktop.
  * @param current - where to return the current desktop index.
  *
  * There is only one desktop when the workspace manager is not active.  CTWM
  * reverts to TWM mode in that case.
  */
void
TwmGetCurrentDesktop(ScreenInfo *scr, int *current)
{
    VirtualScreen *vs;

    if (!scr->workSpaceManagerActive) {
	*current = 0;
	return;
    }
    if (scr == NULL) {
	fprintf(stderr, "ERROR: Screen is NULL!\n");
	*current = 0;
	return;
    }
    if ((vs = scr->currentvs) == NULL && (vs = scr->vScreenList) == NULL) {
	fprintf(stderr, "ERROR: Current virtual screen is NULL!\n");
	*current = 0;
	return;
    }
    if (vs->wsw == NULL) {
	fprintf(stderr, "ERROR: Workspace window is NULL!\n");
	*current = 0;
	return;
    }
    if (vs->wsw->currentwspc == NULL) {
	fprintf(stderr, "ERROR: Current workspace is NULL!\n");
	*current = 0;
	return;
    }

    *current = vs->wsw->currentwspc->number;
}

/** @brief Set the current desktop.
  * @param current - the index of the desktop to set.
  * @param timestamp - X server time of corresponding request.
  *
  * Change the current desktop, but only if it has not been changed since
  * timestamp or if timestamp is CurrentTime (zero).  Note that the desktop
  * index, current, must be range checked as it might come from a client
  * message.
  */
void
TwmSetCurrentDesktop(ScreenInfo *scr, int current, Time timestamp)
{
    int old_current = 0, number = 0;
    VirtualScreen *vs;

    if (!scr->workSpaceManagerActive)
	return;

    TwmGetCurrentDesktop(scr, &old_current);
    if (current == old_current)
	return;
    TwmGetNumberOfDesktops(scr, &number);
    if (current < 0 || current >= number)
	return;
    (void) timestamp;
    /* TODO: check that the desktop has not changed since the timestamp. */
    if ((vs = scr->currentvs) == NULL && (vs = scr->vScreenList) == NULL)
	return;
    GotoWorkSpaceByNumber(vs, current);
}

/** @brief Get the desktop names.
  * @param names - where to return the list of names.
  * @param count - where to return the names count.
  *
  * Get's the window manager's list of workspace names.  This is normally used
  * for setting the _NET_DESKTOP_NAMES property on the root window.
  */
void
TwmGetDesktopNames(ScreenInfo *scr, char ***names, int *count)
{
    struct WorkSpace *ws = NULL;
    int number = 0, n = 0;
    char **list = NULL, *name;

    /* There are no desktop names unless the workspace manager is active.  CTWM 
       reverts to TWM behaviour otherwise. */
    if (!scr->workSpaceManagerActive)
	return;
    for (ws = scr->workSpaceMgr.workSpaceList; ws != NULL; ws = ws->next)
	n++;
    if ((list = calloc(n + 1, sizeof(char *))) != NULL) {
	for (ws = scr->workSpaceMgr.workSpaceList; ws != NULL; ws = ws->next) {
	    if ((name = ws->label) == NULL)
		if ((name = ws->name) == NULL)
		    name = "";
	    if ((list[number++] = strdup(name)) == NULL)
		break;
	    list[number] = NULL;
	}
    }
    *names = list;
    *count = list == NULL ? 0 : number;
}

#define WSPCWINDOW 0

/** @brief Set the desktop names.
  * @param names - the list of names to set.
  * @param count - the number of names in the list.
  *
  * Sets the desktop names displayed by the window manager.
  */
void
TwmSetDesktopNames(ScreenInfo *scr, char **names, int count)
{
    VirtualScreen *vs;
    struct WorkSpace *ws = NULL;
    int n, number = 0;

    if (!scr->workSpaceManagerActive)
	return;

    TwmGetNumberOfDesktops(scr, &number);
    for (n = 0; n < number && n < count; n++) {
	for (ws = scr->workSpaceMgr.workSpaceList; ws != NULL; ws = ws->next) {
	    if (ws->number == n)
		break;
	}
	if (ws != NULL) {
	    ws->label = realloc(ws->label, strlen(names[n]) + 1);
	    strcpy(ws->label, names[n]);
	}
    }

    if ((vs = scr->currentvs) == NULL && (vs = scr->vScreenList) == NULL)
	return;
    PaintWorkSpaceManager(vs);
}

/** @brief Get the active window.
  * @param window - where to return the window
  *
  * Note that this function will result in setting _NET_ACTIVE_WINDOW to None
  * when there is no active window (i.e. the window manager is focussed on the
  * root window).
  */
void
TwmGetActiveWindow(ScreenInfo *scr, Window *window)
{
    *window = scr->Focus != NULL ? scr->Focus->w : None;
}

extern void SetFocus(TwmWindow *twin, Time timestamp);

/** @brief Set the active window.
  * @param window - the TWM window to activate
  * @param active - currently active window in group
  * @param timestamp - time of request
  * @param source - source of request
  *
  * Note that the currently active window in group, time of request and source
  * of request should be used to make sure that windows are not attempting to
  * take focus when they shouldn't.  This, however, is a TODO.
  */
void
TwmSetActiveWindow(TwmWindow *twin, Window active, Time timestamp,
		   enum _NET_SOURCE source)
{
    SetFocus(twin, timestamp);
}

/** @brief Get the work area for a desktop.
  * @param desktop - the desktop for which to get the work area.
  * @param workarea - where to return the work area.
  *
  * This uses the normal NetWM/EWMH approach of calculating the result of the
  * struts that are applied by various windows to determine the remaining work
  * area.
  */
void
TwmGetWorkarea(ScreenInfo *scr, int desktop, struct NetGeometry *workarea)
{
    TwmWindow *twin;
    int top = 0, bottom = 0, left = 0, right = 0;

    workarea->x = workarea->y = 0;
    workarea->width = scr->rootw;
    workarea->height = scr->rooth;
    for (twin = scr->FirstWindow; twin != NULL; twin = twin->next) {
	if (!(twin->ewmh.props._NET_WM_STRUT || twin->ewmh.props._NET_WM_STRUT_PARTIAL))
	    continue;
	if (!(*twin->wmh.mask & (1 << desktop)))
	    continue;
	if (top < (int) twin->ewmh.strut.top)
	    top = (int) twin->ewmh.strut.top;
	if (bottom < (int) twin->ewmh.strut.bottom)
	    bottom = (int) twin->ewmh.strut.bottom;
	if (left < (int) twin->ewmh.strut.left)
	    left = (int) twin->ewmh.strut.left;
	if (right < (int) twin->ewmh.strut.right)
	    right = (int) twin->ewmh.strut.right;
    }
    workarea->x += left;
    workarea->y += top;
    workarea->width -= left + right;
    workarea->height -= top + bottom;
}

/** @brief Get all of the work areas.
  * @param scr - screen
  * @param workarea - where to store the work area list
  * @param workareas - where to store the number of work areas in the list
  */
void
TwmGetWorkareas(ScreenInfo *scr, struct NetGeometry **workarea, int *workareas)
{
    int desktops = 1, n;
    struct NetGeometry *list = NULL;

    TwmGetNumberOfDesktops(scr, &desktops);
    if (desktops < 1)
	return;
    if ((list = calloc(desktops, sizeof(*list))) == NULL)
	return;
    for (n = 0; n < desktops; n++)
	TwmGetWorkarea(scr, n, &list[n]);
    free(*workarea);
    *workarea = list;
    *workareas = desktops;
}

/** @brief Get list of virtual roots.
  * @param scr - screen
  * @param vroot - where to store the list of virtual roots
  * @param vroots - where to store the number of virtual roots
  *
  * From code inspection it does not appear that CTWM uses any virtual roots,
  * except possibly on non-Xinerama multi-head captive configurations (is this
  * really supported?).  This function should normall return zero virtual roots.
  */
void
TwmGetVirtualRoots(ScreenInfo *scr, Window **vroot, int *vroots)
{
    int number = 0;
    Window *windows = NULL;
    VirtualScreen *vs;
    Window root = TwmNetRoot(scr);

    for (vs = scr->vScreenList; vs != NULL; vs = vs->next)
	if (vs->window != None && vs->window != root)
	    number++;
    if ((windows = (Window *) calloc(number + 1, sizeof(Window))) != NULL) {
	int i = 0;

	for (vs = scr->vScreenList; vs != NULL; vs = vs->next)
	    if (vs->window != None && vs->window != root)
		windows[i++] = vs->window;
	windows[i++] = None;
    }
    *vroot = windows;
    *vroots = windows != NULL ? number : 0;
}

/** @brief Get the desktop layout.
  * @param scr - screen
  * @param layout - where to store the layout in cols and rows
  *
  * When the workspace manager is inactive, there is only one desktop (reverts
  * to TWM behaviour) and the layout is 1 column by 1 row.  When the workspace
  * manager is active, the desktop layout is determined by the layout of the
  * workspace manager.
  */
void
TwmGetDesktopLayout(ScreenInfo *scr, struct NetLayout *layout)
{
    if (scr->workSpaceManagerActive) {
	layout->columns = scr->workSpaceMgr.columns;
	layout->rows = scr->workSpaceMgr.lines;
	layout->orientation = _NET_WM_ORIENTATION_HORZ;
	layout->starting_corner = _NET_WM_STARTING_CORNER_TOPLEFT;
    } else {
	layout->columns = 1;
	layout->rows = 1;
	layout->orientation = _NET_WM_ORIENTATION_HORZ;
	layout->starting_corner = _NET_WM_STARTING_CORNER_TOPLEFT;
    }
}

/** @brief Set the desktop layout.
  * @param scr - screen
  * @param layout - the desktop layout to set
  *
  * When the workspace manager is inactive, there is only one desktop (reverts
  * to TWM behaviour) and the layout is 1 column by 1 row and changes are not
  * honored.  When the workspace manager is active, the layout can be changed,
  * but the code is involved, so this is a TODO item.
  */
void
TwmSetDesktopLayout(ScreenInfo *scr, struct NetLayout *layout)
{
    (void) scr;
    (void) layout;
}

/** @brief Get the desktop showing mode.
  * @param scr - screen
  * @param showing - where to store the mode
  *
  * CTWM does not really have a desktop showing mode, so, until we make it have
  * one, the mode is always false.  Providing a desktop showing mode is a TODO
  * item.  It should not be too difficult, windows not occupying the current
  * desktop are hidden, we just need to hide the ones on the current desktop
  * too.
  */
void
TwmGetShowingDesktop(ScreenInfo *scr, Bool *showing)
{
    *showing = False;
}

/** @brief Set the desktop showing mode.
  * @param scr - screen
  * @param showing - mode to set
  *
  * CTWM has no showing desktop mode.  We should probably remove it from
  * _NET_SUPPORTED.  See above for description of how to go about it.  This is a
  * TODO item.
  */
void
TwmSetShowingDesktop(ScreenInfo *scr, Bool showing)
{
    (void) showing;
}

/** @brief Close a TWM Window
  * @param twin - TWM Window to close
  * @param timestamp - the time of the close request
  * @param source - the source of the close request
  *
  * Typical behaviour should be to send a WM_DELETE protocol message if the
  * protocol is selected, otherwise, perform an XKillClient on the window.
  *
  * As a TODO item, we should initiate a _NET_WM_PING protocol when we delete to
  * ensure that the window eventually closes.  Also, if there is a _NET_WM_PID
  * property on the window and the WM_CLIENT_MACHINE matches that of the window
  * manager, we can kill the process.
  */
void
TwmCloseWindow(TwmWindow *twin, Time timestamp, enum _NET_SOURCE source)
{
    int status;
    Atom actual_type;
    int actual_format;
    unsigned long nitems, bytes_after, n;
    unsigned char *prop;
    int kill = 0;

    status =
	XGetWindowProperty(dpy, twin->w, _XA_WM_PROTOCOLS, 0L, 32L, False, XA_ATOM,
			   &actual_type, &actual_format, &nitems, &bytes_after, &prop);
    if (status != Success || actual_type == None)
	kill = 1;
    else
	for (n = 0; n < nitems; n++)
	    if ((Atom) prop[n] == _XA_WM_DELETE_WINDOW)
		kill = 1;
    if (kill) {
	XKillClient(dpy, twin->w);
    } else {
	XEvent event;

	event.xclient.type = ClientMessage;
	event.xclient.display = dpy;
	event.xclient.message_type = _XA_WM_PROTOCOLS;
	event.xclient.format = 32;
	event.xclient.window = twin->w;
	event.xclient.data.l[0] = _XA_WM_DELETE_WINDOW;
	XSendEvent(dpy, twin->w, False, 0, &event);
    }
    XSync(dpy, False);
}

extern void GetGravityOffsets(TwmWindow *tmp, int *xp, int *yp);
extern void SetupFrame(TwmWindow *tmp_win, int x, int y, int w, int h, int bw,
		       Bool sendEvent);

/** @brief Move or resize a TWM window.
  * @param twin - the TWM window to reconfigure
  * @param gravity - gravity to use, WM_SIZE_HINTS.win_gravity when gravity zero
  * @param mask - mask of changes
  * @param changes - changes according to mask
  * @param source - source of the request
  */
void
TwmMoveResizeWindow(TwmWindow *twin, unsigned gravity, unsigned mask,
		    XWindowChanges *changes, enum _NET_SOURCE source)
{
    struct NetGeometry g;
    int bw, gravx = 0, gravy = 0;

    /* Calculate the size of the frame and client window from the request. */
    g.x = twin->frame_x;
    g.y = twin->frame_y;
    g.width = twin->frame_width;
    g.height = twin->frame_height;
    bw = twin->frame_bw;

    /* wm-spec-1.5 says that when gravity is zero (ForgetGravity) that the
       WM_SIZE_HINTS.win_gravity should be used.  Otherwise, we use the
       specified gravity.  GetGravityOffset() gives us the gravity from
       WM_SIZE_HINTS. */
    if (gravity == 0)
	GetGravityOffsets(twin, &gravx, &gravy);
    else {
	static struct {
	    int x, y;
	} gravity_offsets[11] = {
	    {
	    0, 0},		/* ForgetGravity */
	    {
	    -1, -1},		/* NorthWestGravity */
	    {
	    0, -1},		/* NorthGravity */
	    {
	    1, -1},		/* NorthEastGravity */
	    {
	    -1, 0},		/* WestGravity */
	    {
	    0, 0},		/* CenterGravity */
	    {
	    1, 0},		/* EastGravity */
	    {
	    -1, 1},		/* SouthWestGravity */
	    {
	    0, 1},		/* SouthGravity */
	    {
	    1, 1},		/* SouthEastGravity */
	    {
	    0, 0},		/* StaticGravity */
	};
	if (gravity > ForgetGravity && gravity <= StaticGravity) {
	    gravx = gravity_offsets[gravity].x;
	    gravy = gravity_offsets[gravity].y;
	}
    }
    if (mask & CWX) {
	g.x = changes->x - bw;
	g.x -= ((gravx < 0) ? 0 : twin->frame_bw3D);
    }
    if (mask & CWY) {
	g.y = changes->y - ((gravy < 0) ? 0 : twin->title_height) - bw;
	g.y -= ((gravy < 0) ? 0 : twin->frame_bw3D);

    }
    if (mask & CWWidth) {
	g.width = changes->width + 2 * twin->frame_bw3D;
    }
    if (mask & CWHeight) {
	g.height = changes->height + twin->title_height + 2 * twin->frame_bw3D;
    }
    /* Forget the zoom on a window that will be resized in either dimension.
       This is what core CTWM does.  That is probably not right.  There are a
       couple of other things that can be done here:

       - deny changes to a zoomed dimension. - forget only the zoom in the
       dimension that changed. */
    if (g.width != twin->frame_width || g.height != twin->frame_height)
	twin->zoomed = ZOOM_NONE;

    SetupFrame(twin, g.x, g.y, g.width, g.height, -1, False);
}

/** @brief Initiate move or resize on TWM window
  * @param twin - TWM window to initiate move or resize
  * @param x_root - root window x position of mouse
  * @param y_root - root window y position of mouse
  * @param direction - direction to resize or how to move
  * @param button - button that was pressed
  * @param source - source of the request
  */
void
TwmStartMoveResize(TwmWindow *twin, int x_root, int y_root,
		   enum _NET_WM_MOVERESIZE direction, int button, enum _NET_SOURCE source)
{
    switch (source) {
    case _NET_SOURCE_UNSPECIFIED:
    case _NET_SOURCE_APPLICATION:
    case _NET_SOURCE_PAGER:
	break;
    default:
#ifdef DEBUG_EWMH
	fprintf(stderr, "Got _NET_WM_MOVERESIZE with bad source %d\n", source);
#endif
	break;
    }
    switch (direction) {
    case _NET_WM_MOVERESIZE_SIZE_TOPLEFT:
    case _NET_WM_MOVERESIZE_SIZE_TOP:
    case _NET_WM_MOVERESIZE_SIZE_TOPRIGHT:
    case _NET_WM_MOVERESIZE_SIZE_RIGHT:
    case _NET_WM_MOVERESIZE_SIZE_BOTTOMRIGHT:
    case _NET_WM_MOVERESIZE_SIZE_BOTTOM:
    case _NET_WM_MOVERESIZE_SIZE_BOTTOMLEFT:
    case _NET_WM_MOVERESIZE_SIZE_LEFT:
    case _NET_WM_MOVERESIZE_MOVE:
    case _NET_WM_MOVERESIZE_SIZE_KEYBOARD:
    case _NET_WM_MOVERESIZE_MOVE_KEYBOARD:
    case _NET_WM_MOVERESIZE_CANCEL:
	break;
    default:
#ifdef DEBUG_EWMH
	fprintf(stderr, "Got _NET_WM_MOVERESIZE with bad direction %d\n", direction);
#endif
	break;
    }
}

extern TwmWindow *GetTwmWindow(Window w);

/** @brief Restack a window.
  * @param twin - TWM window
  * @param mask - XWindowChanges mask (CWStackMode and CWSibling only)
  * @param changes - XWindowChanges
  * @param source - source of the request
  *
  * ICCCM 2.0/4.1.5:
  * - Not changing the size, location, border width, or stacking order of the
  *   window at all.
  *
  *   A client will receive a synthetic ConfigureNotify event that describes the
  *   (unchanged) geometry of the window.  The (x,y) coordinates will be in the
  *   root coordinate system, adjusted for the border width the client
  *   requested, irrespective of any reparenting that has taken place.  The
  *   border_width will be the border width the client requested.  The client
  *   will not receive a real ConfigureNotify event because no change has
  *   actually taken place.
  *
  * - Moving or restacking the window without resizing it or changing its border
  *   width.
  *
  *   A client will receive a synthetic ConfigureNotify event following the
  *   change that describes the new geometry of the window.  The event's (x,y)
  *   coordinates wlil be in the root coordinate system adjusted for the border
  *   width the client requested.  The client may not receive a real
  *   ConfigureNotify event that describes this change because the window
  *   manager may have reparented the top-level window.  If the client does
  *   receive a real event, the synthetic event will follow the real one.
  */
void
TwmRestackWindow(TwmWindow *twin, unsigned mask, XWindowChanges *changes,
		 enum _NET_SOURCE source)
{
    if ((mask & CWStackMode) && twin->stackmode) {
	TwmWindow *otherwin;

	/* Need to adjust the target window and the sibling window.  The caller
	   uses the client windows where we need to use the parent frame window. 
	 */
	if ((mask & CWSibling) && (otherwin = GetTwmWindow(changes->sibling)) != NULL)
	    changes->sibling = otherwin->frame;
	XConfigureWindow(dpy, twin->frame, mask, changes);
    }
    SetupFrame(twin, twin->frame_x, twin->frame_y, twin->frame_width, twin->frame_height,
	       -1, True);
}

TwmWindow *
TwmFindWindow(ScreenInfo *scr, Window window)
{
    TwmWindow *twin = NULL;

    for (twin = scr->FirstWindow; twin != NULL; twin = twin->next)
	if (twin->w == window)
	    break;
    return twin;
}

/** @brief Estimate the frame extents for a window
  * @param window - window to estimate
  * @param extents - where to store the extents
  */
void
TwmEstFrameExtents(Window window, struct NetExtents *extents)
{
    extents->left = extents->right = extents->top = extents->bottom = 0;

    /* TODO: go through the perambulations to determine whether we would apply
       a border and titlebar on mapping and set the frame extents on the
       window.  This is normally only performed for windows that have not yet
       been mapped. */
}

extern void free_window_names(TwmWindow *twin, Bool nukefull, Bool nukename,
			      Bool nukeicon);

void
TwmSetWMName(TwmWindow *twin, char *name)
{
    if (name == NULL)
	return;
#ifdef CLAUDE
    if (strstr(name, " - Mozilla"))
	*strstr(name, " - Mozilla") = '\0';
#endif				/* CLAUDE */
    free_window_names(twin, True, True, False);
    /* FIXME: more... */
}

/*
 * Retrieves the visible name from the window manager and returns it to the
 * NetWM/EWMH module to be included in the _NET_WM_VISIBLE_NAME property.
 * Returning a null pointer (or simply not changing the pointer) results in
 * removal of the property.  The caller must eventually free any name returned
 * with free().
 */
void
TwmGetWMVisibleName(TwmWindow *twin, char **name)
{
    free(*name);
    *name = NULL;
    /* FIXME: get the name */
}

void
TwmSetWMIconName(TwmWindow *twin, char *name)
{
}

/*
 * Retrieves the visible icon name from the window manager and returns it to the
 * NetWM/EWMH module to be included in the _NET_WM_VISIBLE_ICON_NAME property.
 * Returning a null pointer (or simply not changing the pointer) results in
 * removal of the property.  The caller must eventually free any name returned
 * with free().
 */
void
TwmGetWMVisibleIconName(TwmWindow *twin, char **name)
{
    free(*name);
    *name = NULL;
    /* FIXME: get the name */
}

extern int fullOccupation;

/** @brief Get the desktop for a window.
  * @param scr - screen
  * @param twin - TWM window
  * @param desktop - where to store the desktop
  *
  * Called to update the _NET_WM_DESKTOP to the current value for the window,
  * twin.  When the window is not omnipresent, we only return the first desktop
  * that the window occupies.
  */
void
TwmGetWMDesktop(ScreenInfo *scr, TwmWindow *twin, int *desktop)
{
    if (twin->occupation == fullOccupation)
	*desktop = -1;
    else {
	int n, mask, number = 1;

	TwmGetNumberOfDesktops(scr, &number);
	for (n = 0, mask = 1; n < number; n++, mask <<= 1)
	    if (twin->occupation & mask) {
		*desktop = n;
		break;
	    }
    }
}

void
TwmSetWMDesktop(ScreenInfo *scr, TwmWindow *twin, int desktop, enum _NET_SOURCE source)
{
    int number = 1;

    if ((twin = TwmCanChangeDesktop(twin)) == NULL)
	return;

    TwmGetNumberOfDesktops(scr, &number);
    if (desktop != -1 && (0 > desktop || desktop >= number)) {
#ifdef DEBUG_EWMH
	fprintf(stderr, "Attempt to set invalid desktop %d\n", desktop);
#endif				/* DEBUG_EWMH */
	return;
    }
    if (desktop == -1)
	OccupyAll(twin);
    else
	ChangeOccupation(twin, 1 << desktop);
}

void
TwmSetWMWindowType(TwmWindow *twin, unsigned type)
{
}

#define _NET_WM_STATE_ALLZOOM_MASK ( \
	_NET_WM_STATE_MAXIMIZED_HORZ_BIT | \
	_NET_WM_STATE_MAXIMIZED_VERT_BIT | \
	_NET_WM_STATE_MAXIMUS_BOTTOM_BIT | \
	_NET_WM_STATE_MAXIMUS_LEFT_BIT | \
	_NET_WM_STATE_MAXIMUS_RIGHT_BIT | \
	_NET_WM_STATE_MAXIMUS_TOP_BIT )

void
TwmGetWMState(TwmWindow *twin, unsigned *flags)
{
    if (twin->squeezed)
	*flags |= _NET_WM_STATE_SHADED_BIT;
    else
	*flags &= ~_NET_WM_STATE_SHADED_BIT;

    if (twin->occupation == fullOccupation)
	*flags |= _NET_WM_STATE_STICKY_BIT;
    else
	*flags &= ~_NET_WM_STATE_STICKY_BIT;

    *flags &= ~_NET_WM_STATE_ALLZOOM_MASK;
    switch (twin->zoomed) {
    case ZOOM_NONE:
	break;
    case F_ZOOM:
	*flags |= _NET_WM_STATE_MAXIMIZED_VERT_BIT;
	break;
    case F_LEFTZOOM:
	*flags |= _NET_WM_STATE_MAXIMUS_LEFT_BIT;
	*flags |= _NET_WM_STATE_MAXIMIZED_VERT_BIT;
	break;
    case F_RIGHTZOOM:
	*flags |= _NET_WM_STATE_MAXIMUS_RIGHT_BIT;
	*flags |= _NET_WM_STATE_MAXIMIZED_VERT_BIT;
	break;
    case F_BOTTOMZOOM:
	*flags |= _NET_WM_STATE_MAXIMUS_BOTTOM_BIT;
	*flags |= _NET_WM_STATE_MAXIMIZED_HORZ_BIT;
	break;
    case F_TOPZOOM:
	*flags |= _NET_WM_STATE_MAXIMUS_TOP_BIT;
	*flags |= _NET_WM_STATE_MAXIMIZED_HORZ_BIT;
	break;
    case F_FULLZOOM:
	*flags |= _NET_WM_STATE_MAXIMIZED_VERT_BIT;
	*flags |= _NET_WM_STATE_MAXIMIZED_HORZ_BIT;
	break;
    case F_HORIZOOM:
	*flags |= _NET_WM_STATE_MAXIMIZED_HORZ_BIT;
	break;
    }

    /* FIXME: do other bits */
}

void
TwmSetWMState(TwmWindow *twin, unsigned state)
{
}

void
TwmChgWMState(ScreenInfo *scr, TwmWindow *twin, int action1, int action2, unsigned action,
	      unsigned source)
{
    int current = 0;
    TwmWindow *tmp_win;

    tmp_win = TwmCanChangeDesktop(twin);

    /* sanity checks */
    switch (source) {
    case _NET_SOURCE_UNSPECIFIED:
    case _NET_SOURCE_APPLICATION:
    case _NET_SOURCE_PAGER:
	break;
    default:
#ifdef DEBUG_EWMH
	fprintf(stderr, "Received _NET_WM_STATE message with invalid source %d\n",
		source);
#endif
	return;
    }
    switch (action) {
    case _NET_WM_STATE_REMOVE:
    case _NET_WM_STATE_ADD:
    case _NET_WM_STATE_TOGGLE:
	break;
    default:
#ifdef DEBUG_EWMH
	fprintf(stderr, "Received _NET_WM_STATE message with invalid action %d\n",
		action);
#endif
	return;
    }

    TwmGetCurrentDesktop(scr, &current);

    if (0 <= action1 && action1 <= 31) {
	switch (action1) {
	case _NET_WM_STATE_MODAL:
	    break;
	case _NET_WM_STATE_STICKY:
	    switch (action) {
	    case _NET_WM_STATE_REMOVE:
		if (tmp_win != NULL)
		    ChangeOccupation(tmp_win, 1 << current);
		break;
	    case _NET_WM_STATE_ADD:
		OccupyAll(twin);
		break;
	    case _NET_WM_STATE_TOGGLE:
		if (tmp_win != NULL) {
		    if (twin->ewmh.state & _NET_WM_STATE_STICKY_BIT)
			ChangeOccupation(tmp_win, 1 << current);
		    else
			OccupyAll(twin);
		}
		break;
	    default:
		break;
	    }
	    break;
	case _NET_WM_STATE_MAXIMIZED_HORZ:
	    break;
	case _NET_WM_STATE_MAXIMIZED_VERT:
	    break;
	case _NET_WM_STATE_SHADED:
	    switch (action) {
	    case _NET_WM_STATE_REMOVE:
		if (twin->ewmh.state & _NET_WM_STATE_SHADED_BIT)
		    Squeeze(twin);
		break;
	    case _NET_WM_STATE_ADD:
		if (!(twin->ewmh.state & _NET_WM_STATE_SHADED_BIT))
		    Squeeze(twin);
		break;
	    case _NET_WM_STATE_TOGGLE:
		Squeeze(twin);
		break;
	    default:
		break;
	    }
	    break;
	case _NET_WM_STATE_SKIP_TASKBAR:
	    break;
	case _NET_WM_STATE_SKIP_PAGER:
	    break;
	case _NET_WM_STATE_HIDDEN:
	    break;
	case _NET_WM_STATE_FULLSCREEN:
	    break;
	case _NET_WM_STATE_ABOVE:
	    break;
	case _NET_WM_STATE_BELOW:
	    break;
	case _NET_WM_STATE_DEMANDS_ATTENTION:
	    break;
	case _NET_WM_STATE_FOCUSED:
	    break;
	case _NET_WM_STATE_DECOR_BORDER:
	    break;
	case _NET_WM_STATE_DECOR_HANDLE:
	    break;
	case _NET_WM_STATE_DECOR_TITLE:
	    break;
	case _NET_WM_STATE_DECOR:
	    break;
	case _NET_WM_STATE_PARENTRELATIVE_BACKGROUND:
	    break;
	case _NET_WM_STATE_STAYS_AT_BOTTOM:
	    break;
	case _NET_WM_STATE_STAYS_ON_TOP:
	    break;
	case _NET_WM_STATE_MAXIMUS_BOTTOM:
	    break;
	case _NET_WM_STATE_MAXIMUS_LEFT:
	    break;
	case _NET_WM_STATE_MAXIMUS_RIGHT:
	    break;
	case _NET_WM_STATE_MAXIMUS_TOP:
	    break;
	case _NET_WM_STATE_AUTORAISE:
	    break;
	case _NET_WM_STATE_AUTOLOWER:
	    break;
	default:
	    break;
	}
	switch (action) {
	case _NET_WM_STATE_REMOVE:
	    twin->ewmh.state &= ~(1 << action1);
	    break;
	case _NET_WM_STATE_ADD:
	    twin->ewmh.state |= (1 << action1);
	    break;
	case _NET_WM_STATE_TOGGLE:
	    twin->ewmh.state ^= (1 << action1);
	    break;
	default:
	    break;
	}
    }
    if (0 <= action2 && action2 <= 31) {
	switch (action2) {
	case _NET_WM_STATE_MODAL:
	    break;
	case _NET_WM_STATE_STICKY:
	    switch (action) {
	    case _NET_WM_STATE_REMOVE:
		if (tmp_win != NULL)
		    ChangeOccupation(tmp_win, 1 << current);
		break;
	    case _NET_WM_STATE_ADD:
		OccupyAll(twin);
		break;
	    case _NET_WM_STATE_TOGGLE:
		if (tmp_win != NULL) {
		    if (twin->ewmh.state & _NET_WM_STATE_STICKY_BIT)
			ChangeOccupation(tmp_win, 1 << current);
		    else
			OccupyAll(tmp_win);
		}
		break;
	    default:
		break;
	    }
	    break;
	case _NET_WM_STATE_MAXIMIZED_HORZ:
	    break;
	case _NET_WM_STATE_MAXIMIZED_VERT:
	    break;
	case _NET_WM_STATE_SHADED:
	    switch (action) {
	    case _NET_WM_STATE_REMOVE:
		if (twin->ewmh.state & _NET_WM_STATE_SHADED_BIT)
		    Squeeze(twin);
		break;
	    case _NET_WM_STATE_ADD:
		if (!(twin->ewmh.state & _NET_WM_STATE_SHADED_BIT))
		    Squeeze(twin);
		break;
	    case _NET_WM_STATE_TOGGLE:
		Squeeze(twin);
		break;
	    default:
		break;
	    }
	    break;
	case _NET_WM_STATE_SKIP_TASKBAR:
	    break;
	case _NET_WM_STATE_SKIP_PAGER:
	    break;
	case _NET_WM_STATE_HIDDEN:
	    break;
	case _NET_WM_STATE_FULLSCREEN:
	    break;
	case _NET_WM_STATE_ABOVE:
	    break;
	case _NET_WM_STATE_BELOW:
	    break;
	case _NET_WM_STATE_DEMANDS_ATTENTION:
	    break;
	case _NET_WM_STATE_FOCUSED:
	    break;
	case _NET_WM_STATE_DECOR_BORDER:
	    break;
	case _NET_WM_STATE_DECOR_HANDLE:
	    break;
	case _NET_WM_STATE_DECOR_TITLE:
	    break;
	case _NET_WM_STATE_DECOR:
	    break;
	case _NET_WM_STATE_PARENTRELATIVE_BACKGROUND:
	    break;
	case _NET_WM_STATE_STAYS_AT_BOTTOM:
	    break;
	case _NET_WM_STATE_STAYS_ON_TOP:
	    break;
	case _NET_WM_STATE_MAXIMUS_BOTTOM:
	    break;
	case _NET_WM_STATE_MAXIMUS_LEFT:
	    break;
	case _NET_WM_STATE_MAXIMUS_RIGHT:
	    break;
	case _NET_WM_STATE_MAXIMUS_TOP:
	    break;
	case _NET_WM_STATE_AUTORAISE:
	    break;
	case _NET_WM_STATE_AUTOLOWER:
	    break;
	default:
	    break;
	}
	switch (action) {
	case _NET_WM_STATE_REMOVE:
	    twin->ewmh.state &= ~(1 << action2);
	    break;
	case _NET_WM_STATE_ADD:
	    twin->ewmh.state |= (1 << action2);
	    break;
	case _NET_WM_STATE_TOGGLE:
	    twin->ewmh.state ^= (1 << action2);
	    break;
	default:
	    break;
	}
    }
}

void
TwmGetWMAllowedActions(TwmWindow *twin, unsigned *flags)
{
}

/** @brief Set the pid for a window.
  * @param scr - screen
  * @param twin - TWM window
  * @param pid - the pid to set
  *
  * This sets the specified process id value in the PID= field of the associated
  * startup notification sequence, if any, and marks when the startup
  * notification sequence has changed.
  */
void
TwmSetWMPid(ScreenInfo *scr, TwmWindow *twin, pid_t pid)
{
    EwmhSequence *seq;

    if (pid <= 0)
	return;
    if ((seq = twin->ewmh.sequence) != NULL
	&& (seq->field.pid == NULL || seq->numb.pid == 0)) {
	char *str;

	if ((str = calloc(32, 1)) == NULL)
	    return;
	snprintf(str, 31, "%d", pid);
	free(seq->field.pid);
	seq->field.pid = str;
	seq->numb.pid = pid;
    }
}

void
TwmUpdWMHandledIcons(ScreenInfo *scr)
{
    TwmWindow *twin;

    scr->ewmh.handled_icons = False;

    for (twin = scr->FirstWindow; twin != NULL; twin = twin->next) {
	if (twin->ewmh.props._NET_WM_HANDLED_ICONS) {
	    scr->ewmh.handled_icons = True;
	    break;
	}
    }
}

/** @brief Set the user time window for a window.
  * @param scr - screen
  * @param twin - TWM window
  * @param time_window - XID of the time window
  *
  * We must do two things, select for property notifications on the time window,
  * and add the time window to the ScreenContext and TWmContext so that property
  * notifications will be directed to the correct place.
  */
void
TwmSetWMUserTimeWindow(ScreenInfo *scr, TwmWindow *twin, Window time_window)
{
    if (time_window != None) {
	XSelectInput(dpy, time_window, PropertyChangeMask);
	XSaveContext(dpy, time_window, TwmContext, (XPointer) twin);
	XSaveContext(dpy, time_window, ScreenContext, (XPointer) scr);
    }
}

/** @brief Delete the user time window for a window.
  * @param scr - screen
  * @param twin - TWM Window
  * @param time_window - XID of the time window
  *
  * Undo the things that we did when we set the time window.  This will normally
  * be called when the primary window is withdrawn.
  */
void
TwmDelWMUserTimeWindow(ScreenInfo *scr, TwmWindow *twin, Window time_window)
{
    if (time_window != None) {
	XSelectInput(dpy, time_window, 0);
	XDeleteContext(dpy, time_window, TwmContext);
	XDeleteContext(dpy, time_window, ScreenContext);
    }
}

Time lastUserTime = CurrentTime;

/** @brief Set the last user time.
  * @param thisUserTime - time to push
  *
  * This function pushes the global timestamp 'lastUserTime' along when
  * 'thisUserTime' is not 'CurrentTime' and either 'lastUserTime' has never been
  * set (equal to 'CurrentTime') or 'thisUserTime' is "later" than
  * 'lastUserTime'.
  */
void
TwmSetWMUserTime(Time thisUserTime)
{
    if (thisUserTime == CurrentTime) {
	/* cannot advance */
    } else if (lastUserTime == CurrentTime) {
	lastUserTime = thisUserTime;
    } else if ((int) ((int) thisUserTime - (int) lastUserTime) > (int) 0) {
	lastUserTime = thisUserTime;
    }
}

/** @brief Get the last user time.
  * @param thisUserTime - where to store the time
  */
void
TwmGetWMUserTime(Time *thisUserTime)
{
    if (thisUserTime != NULL)
	*thisUserTime = lastUserTime;
}

/** @brief Get the frame extents for a window.
  * @param twin - TWM window
  * @param extents - where to store the extents
  */
void
TwmGetWMFrameExtents(TwmWindow *twin, struct NetExtents *extents)
{
    extents->left = extents->right = extents->bottom = extents->top =
	twin->frame_bw + twin->frame_bw3D;
    extents->top += twin->title_height;
}

/** @brief Get the full screen monitors.
  * @param scr - screen
  * @param twin - TWM window
  * @param monitors - where to store the monitors
  */
void
TwmGetWMFullscreenMonitors(ScreenInfo *scr, TwmWindow *twin, struct NetMonitors *monitors)
{
    VirtualScreen *vs;
    int x_min = scr->rootw;
    int x_max = 0;
    int y_min = scr->rooth;
    int y_max = 0;

    for (vs = scr->vScreenList; vs != NULL; vs = vs->next) {
	if (vs->x < x_min) {
	    x_min = vs->x;
	    monitors->left = vs->monitor;
	}
	if (vs->x + vs->w > x_max) {
	    x_max = vs->x + vs->w;
	    monitors->right = vs->monitor;
	}
	if (vs->y < y_min) {
	    y_min = vs->y;
	    monitors->top = vs->monitor;
	}
	if (vs->y + vs->h > y_max) {
	    y_max = vs->y + vs->h;
	    monitors->bottom = vs->monitor;
	}
    }
}

/** @brief Set the full screen monitors.
  * @param scr - screen
  * @param twin - TWM window
  * @param monitors - monitors to set.
  *
  * Set the fullscreen monitors.  Some sanity checks are in order here.  The
  * monitors array can be obtained using Xinerama or XRANDR extentsions.
  */
void
TwmSetWMFullscreenMonitors(ScreenInfo *scr, TwmWindow *twin, struct NetMonitors *monitors)
{
    int x_min, x_max, y_min, y_max;
    VirtualScreen *vs;

    for (vs = scr->vScreenList; vs != NULL; vs = vs->next)
	if (vs->monitor == monitors->left)
	    break;
    if (vs == NULL)
	return;
    x_min = vs->x;

    for (vs = scr->vScreenList; vs != NULL; vs = vs->next)
	if (vs->monitor == monitors->right)
	    break;
    if (vs == NULL)
	return;
    x_max = vs->x + vs->w;

    for (vs = scr->vScreenList; vs != NULL; vs = vs->next)
	if (vs->monitor == monitors->top)
	    break;
    if (vs == NULL)
	return;
    y_min = vs->y;

    for (vs = scr->vScreenList; vs != NULL; vs = vs->next)
	if (vs->monitor == monitors->bottom)
	    break;
    if (vs == NULL)
	return;
    y_max = vs->y + vs->h;

    if (x_min >= x_max || y_min >= y_max)
	return;
}

/** @brief Set the startup id.
  * @param scr - screen
  * @param twin - TWM window
  * @param startup_id - startup notification identifier
  */
void
TwmSetStartupId(ScreenInfo *scr, TwmWindow *twin, char *startup_id)
{
    EwmhSequence *seq;
    Bool force = False;

    if ((seq = twin->ewmh.sequence) != NULL) {
	/* sequence already assigned */
	if (strcmp(startup_id, seq->field.id) != 0) {
	    /* incorrect sequence is associated with the window, release it
	       back to screen list */
	    if (seq->prev == NULL) {
		if ((seq->next = scr->ewmh.sequence) != NULL)
		    seq->next->prev = &seq->next;
		seq->prev = &scr->ewmh.sequence;
	    }
	    if (seq->twm != NULL) {
		seq->twm->ewmh.sequence = NULL;
		force = True;
	    }
	    seq->twm = NULL;
	    seq = NULL;
	}
    }
    if (seq == NULL) {
	/* look for an matching startup notification sequence */
	for (seq = scr->ewmh.sequence; seq != NULL; seq = seq->next)
	    if (strcmp(startup_id, seq->field.id) == 0)
		break;
	if (seq != NULL) {
	    if (seq->twm != NULL) {
		seq->twm->ewmh.sequence = NULL;
		force = True;
	    }
	    seq->twm = twin;
	    twin->ewmh.sequence = seq;
	}
    }
    if (seq == NULL) {
	/* need to create a new startup notification sequence for the
	   application */
	char *str = NULL;

	if ((str = strdup(startup_id)) == NULL)
	    return;		/* failed */
	if ((seq = calloc(1, sizeof(*seq))) == NULL) {
	    free(str);
	    return;		/* failed */
	}
	if ((seq->next = scr->ewmh.sequence) != NULL)
	    seq->next->prev = &seq->next;
	seq->prev = &scr->ewmh.sequence;
	seq->changed = False;

	seq->state = _NET_NOTIFY_IDLE;
	seq->field.id = str;

	seq->twm = twin;
	twin->ewmh.sequence = seq;
    }
    Upd_NET_STARTUP_INFO(scr, twin, force);
}

void
TwmSetWMProtocols(TwmWindow *twin, unsigned protocols)
{
}

/** @brief Get the desktop mask for a window.
  * @param scr - screen
  * @param twin - TWM window
  * @param mask - where to store list of masks
  * @param masks - where to store count of masks in list
  */
void
TwmGetWMDesktopMask(ScreenInfo *scr, TwmWindow *twin, long **mask, int *masks)
{
    long *list;

    if ((list = calloc(1, sizeof(long))) == NULL)
	return;
    list[0] = (long) (unsigned) twin->occupation;
    *mask = list;
    *masks = 1;
}

/** @brief Set the desktop mask for a window.
  * @param scr- screen
  * @param twin - TWM window
  * @param mask - list of masks
  * @param masks - count of masks in list
  */
void
TwmSetWMDesktopMask(ScreenInfo *scr, TwmWindow *twin, long *mask, int masks)
{
    unsigned occupation;

    if ((twin = TwmCanChangeDesktop(twin)) == NULL)
	return;

    if (masks < 1 || mask == NULL)
	return;
    occupation = (unsigned) mask[0];
    occupation &= fullOccupation;
    if (occupation == 0)
	return;
    if (occupation == fullOccupation) {
	OccupyAll(twin);
	return;
    }
    if (twin->occupation == occupation)
	return;
    ChangeOccupation(twin, occupation);
}

/** @brief Change the desktop mask for a window.
  * @param scr - screen
  * @param twin - TWM window
  * @param index - index of mask word
  * @param mask - replacement value for indexed mask
  */
void
TwmChgWMDesktopMask(ScreenInfo *scr, TwmWindow *twin, unsigned index, unsigned mask)
{
    unsigned occupation;

    if ((twin = TwmCanChangeDesktop(twin)) == NULL)
	return;

    if (index != 0)
	return;
    occupation = mask;
    occupation &= fullOccupation;
    if (occupation == 0)
	return;
    if (occupation == fullOccupation) {
	OccupyAll(twin);
	return;
    }
    if (twin->occupation == occupation)
	return;
    ChangeOccupation(twin, occupation);
}

void
TwmGetWMVirtualPos(TwmWindow *twin, struct NetPosition *virtual_pos)
{
}

void
TwmSetWMVirtualPos(TwmWindow *twin, struct NetPosition *virtual_pos)
{
}

void
TwmChgWMSyncRequestCounter(TwmWindow *twin, struct NetCounter *counter)
{
}

void
TwmGotWMPing(TwmWindow *twin, Time timestamp, Window client)
{
}

void
TwmGetKdeSystemTrayWindows(ScreenInfo *scr, Window **systray, int *count)
{
}

void
TwmGetMaximizedRestore(TwmWindow *twin, struct NetRestore *restore)
{
}

void
TwmSetMaximizedRestore(TwmWindow *twin, struct NetRestore *restore)
{
}

// vim: sw=4 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
