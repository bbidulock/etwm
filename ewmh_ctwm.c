#include <stdio.h>
#include "ewmh.h"
#ifdef MWMH
#include "mwmh.h"
#endif				/* MWMH */
#include "screen.h"
#include "parse.h"
#include "icons.h"
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
	&& !(twin->ewmh.allowed & _NET_WM_ACTION_CHANGE_DESKTOP))
	return (NULL);
    if (!CanChangeOccupation(&twin))
	return (NULL);
    if (twin->ewmh.props._NET_WM_ALLOWED_ACTIONS
	&& !(twin->ewmh.allowed & _NET_WM_ACTION_CHANGE_DESKTOP))
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
		    XCSUCCESS && twin->frame == children[n])
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
  * @param scr - screen
  * @param current - where to return the current desktop index.
  *
  * There is only one desktop when the workspace manager is not active.  CTWM
  * reverts to TWM mode in that case.
  *
  * Unfortunaely CTWM behaves as follows:
  *
  * - each virtual screen display a different desktop;
  *
  * - a desktop can only be displayed on one virtual screen;
  *
  * - windows that occupy multiple desktops can only be displayed on one virtual
  *   screen at a time;
  *
  * The above restrictions are because workspaces are attached to the real
  * screen and each workspace only has only window list.  Twm windows can only
  * be on one list.
  *
  * We can only store one current desktop per real screen, so, for now, we just
  * track the first virtual screen, regardless of which desktops are set on the
  * other virtual screens.  During startup we will set the other virtual screens
  * to the desktops after the current one (for kind of a ring effect).
  */
void
TwmGetCurrentDesktop(ScreenInfo *scr, int *current)
{
    VirtualScreen *vs;
    WorkSpace *ws;

    if (!scr->workSpaceManagerActive) {
	*current = 0;
	return;
    }
    if (scr == NULL) {
	fprintf(stderr, "ERROR: Screen is NULL!\n");
	*current = 0;
	return;
    }
    if ((vs = scr->vScreenList) == NULL) {
	fprintf(stderr, "ERROR: First virtual screen is NULL!\n");
	*current = 0;
	return;
    }
    if (vs->wsw == NULL) {
	fprintf(stderr, "ERROR: Workspace window is NULL!\n");
	*current = 0;
	return;
    }
    if ((ws = vs->wsw->currentwspc) == NULL) {
	fprintf(stderr, "ERROR: Current workspace is NULL!\n");
	*current = 0;
	return;
    }

    *current = ws->number;
}

/** @brief Set the current desktop.
  * @param scr - screen
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
  * @param scr - screen
  * @param window - the window to activate
  * @param active - currently active window in group
  * @param timestamp - time of request
  * @param source - source of request
  *
  * Note that the currently active window in group, time of request and source
  * of request should be used to make sure that windows are not attempting to
  * take focus when they shouldn't.  This, however, is a TODO.
  */
void
TwmSetActiveWindow(ScreenInfo *scr, Window window, Window active, Time timestamp,
		   enum _NET_SOURCE source)
{
    TwmWindow *twin;

    if (window == None)
	return;
    if ((twin = TwmFindWindow(scr, window)) != NULL)
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

extern int showingBackground;

/** @brief Get the desktop showing mode.
  * @param scr - screen
  * @param showing - where to store the mode
  */
void
TwmGetShowingDesktop(ScreenInfo *scr, Bool *showing)
{
    *showing = scr->showingDesktop;
}

/** @brief Set the desktop showing mode.
  * @param scr - screen
  * @param showing - mode to set
  */
void
TwmSetShowingDesktop(ScreenInfo *scr, Bool showing)
{
    if (scr->showingDesktop == showing)
	return;
    ShowBackground(scr->currentvs);
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
	    {0, 0},		/* ForgetGravity */
	    {-1, -1},		/* NorthWestGravity */
	    {0, -1},		/* NorthGravity */
	    {1, -1},		/* NorthEastGravity */
	    {-1, 0},		/* WestGravity */
	    {0, 0},		/* CenterGravity */
	    {1, 0},		/* EastGravity */
	    {-1, 1},		/* SouthWestGravity */
	    {0, 1},		/* SouthGravity */
	    {1, 1},		/* SouthEastGravity */
	    {0, 0},		/* StaticGravity */
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

extern int Cancel;
extern int ResizeOrigX;
extern int ResizeOrigY;
extern XEvent Event;

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
    {
	XEvent xev;

	xev.type = ButtonPress;
	xev.xbutton.x_root = x_root;
	xev.xbutton.y_root = y_root;

	ResizeOrigX = x_root;
	ResizeOrigY = y_root;

	StartResize(&xev, twin, False, True);

	do {
	    XMaskEvent(dpy,
		       ButtonPressMask | ButtonReleaseMask | EnterWindowMask |
		       LeaveWindowMask | ButtonMotionMask | VisibilityChangeMask |
		       ExposureMask, &Event);
	    if (Event.type == MotionNotify)
		/* discard any extra motion events before a release */
		while (XCheckMaskEvent(dpy, ButtonMotionMask | ButtonReleaseMask, &Event))
		    if (Event.type == ButtonRelease)
			break;
	    if (!DispatchEvent2())
		continue;
	} while (!(Event.type == ButtonRelease || Cancel));
	break;
    }
    case _NET_WM_MOVERESIZE_SIZE_KEYBOARD:
    {
	XEvent xev;

	xev.type = KeyPress;
	xev.xkey.x_root = x_root;
	xev.xkey.y_root = y_root;

	ResizeOrigX = x_root;
	ResizeOrigY = y_root;

	StartResize(&xev, twin, False, True);

	do {
	    XMaskEvent(dpy,
		       ButtonPressMask | ButtonReleaseMask | EnterWindowMask |
		       LeaveWindowMask | ButtonMotionMask | VisibilityChangeMask |
		       ExposureMask, &Event);
	    if (Event.type == MotionNotify)
		/* discard any extra motion events before a release */
		while (XCheckMaskEvent(dpy, ButtonMotionMask | ButtonReleaseMask, &Event))
		    if (Event.type == ButtonRelease)
			break;
	    if (!DispatchEvent2())
		continue;
	} while (!(Event.type == ButtonRelease || Cancel));
	break;
    }
    case _NET_WM_MOVERESIZE_MOVE:
    case _NET_WM_MOVERESIZE_MOVE_KEYBOARD:
	break;
    case _NET_WM_MOVERESIZE_CANCEL:
	Cancel = True;
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

/** @brief Get the client machine for a window.
  * @param scr - screen
  * @param twin - TWM window
  * @param machine - where to store the client machine
  *
  * This function assists clients that neglect to set the ICCCM property (or do
  * not set it correctly) when a compliant XDG startup notification launcher has
  * been used to launch the application.
  */
void
TwmGetWMClientMachine(ScreenInfo *scr, TwmWindow *twin, char **machine)
{
    EwmhSequence *seq;

    if ((seq = twin->ewmh.sequence) == NULL)
	return;
    if (seq->field.hostname == NULL)
	return;
    *machine = strdup(seq->field.hostname);
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

extern void RedoIcon(void);

extern char *NoName;

void
TwmSetWMName(ScreenInfo *scr, TwmWindow *twin, char *name)
{
    XRectangle inc_rect, logical_rect;

    if (name == NULL)
	return;
#ifdef CLAUDE
    if (strstr(name, " - Mozilla"))
	*strstr(name, " - Mozilla") = '\0';
#endif				/* CLAUDE */
    free_window_names(twin, True, True, False);

    twin->full_name = name;
    twin->name = name;
    twin->nameChanged = 1;
    Xutf8TextExtents(scr->TitleBarFont.font_set, name, strlen(name), &inc_rect,
		     &logical_rect);
    twin->name_width = logical_rect.width;
    SetupWindow(twin, twin->frame_x, twin->frame_y, twin->frame_width, twin->frame_height,
		-1);
    if (twin->title_w)
	XClearArea(dpy, twin->title_w, 0, 0, 0, 0, True);
    if (scr->AutoOccupy)
	WmgrRedoOccupation(twin);
    if (twin->icon_name == NoName) {
	twin->icon_name = name;
	RedoIcon();
    }
}

/** @brief Get the visible name.
  * @param twin - TWM window
  * @param name - where to store the name
  *
  * Retrieves the visible name from the window manager and returns it to the
  * NetWM/EWMH module to be included in the _NET_WM_VISIBLE_NAME property.
  * Returning a null pointer (or simply not changing the pointer) results in
  * removal of the property.  The caller must eventually free any name returned
  * with free().
  *
  * CTWM does not alter visible names, so this function simply returns the name
  * if it is available, otherwise untitled.
  */
void
TwmGetWMVisibleName(TwmWindow *twin, char **name)
{
    if (twin->ewmh.name != NULL)
	*name = strdup(twin->ewmh.name);
    else
	*name = strdup("[untitled]");
}

void
TwmSetWMIconName(TwmWindow *twin, char *name)
{
    if (name == NULL)
	return;
#ifdef CLAUDE
    {
	char *moz = strstr(prop, " - Mozilla");

	if (moz)
	    *moz = '\0';
    }
#endif
    free_window_names(twin, False, False, True);
    twin->icon_name = name;

    if (strcmp(twin->icon_name, name))
	RedoIcon();
}

/** @brief Get the visible icon name.
  * @param twin - TWM window
  * @param name - where to store the name
  *
  * Retrieves the visible icon name from the window manager and returns it to
  * the NetWM/EWMH module to be included in the _NET_WM_VISIBLE_ICON_NAME
  * property.  Returning a null pointer (or simply not changing the pointer)
  * results in removal of the property.  The caller must eventually free any
  * name returned with free().
  *
  * CTWM does not alter visible names, so this function simply returns the icon
  * name if it is available, otherwise the name, or untitled.
  */
void
TwmGetWMVisibleIconName(TwmWindow *twin, char **name)
{
    if (twin->ewmh.icon_name != NULL)
	*name = strdup(twin->ewmh.icon_name);
    else if (twin->ewmh.name != NULL)
	*name = strdup(twin->ewmh.name);
    else
	*name = strdup("[untitled]");
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

/** @brief Initialize the desktop for a window.
  * @param scr - screen
  * @param twin - TWM window
  * @param desktop - where to store initialize desktop number
  *
  * Only called when the client has not specified a desktop for a window.  When
  * a startup notification sequence is associated with the window that includes
  * the DESKTOP= key field, we use that desktop as the initial desktop when the
  * property does not yet exist.  This is because, when the application does not
  * specify a desktop, the desktop specified by the launcher applies.  The
  * default specified by the launcher is normally the desktop on which the
  * application was launched.
  *
  * When there is no startup sequence, the desktop should be the current
  * desktop.
  */
void
TwmIniWMDesktop(ScreenInfo *scr, TwmWindow *twin, int *desktop)
{
    EwmhSequence *seq;

    if ((seq = twin->ewmh.sequence) != NULL && seq->field.desktop != NULL)
	*desktop = seq->numb.desktop;
    else
	TwmGetCurrentDesktop(scr, desktop);

}

/** @brief Set the desktop for a window.
  * @param scr - screen
  * @param twin - TWM window
  * @param desktop - desktop to set
  * @param source - source of the request
  */
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

/** @brief Get the window type for a window.
  * @param twin - TWM window
  * @param type - where to store the type
  *
  * This function is only called when there is no _NET_WM_WINDOW_TYPE property
  * set on the window.  Managed windows with WM_TRANSIENT_FOR set default to
  * type _NET_WM_WINDOW_TYPE_DIALOG.  Managed windows without WM_TRANSIENT_FOR
  * set default to type _NET_WM_WINDOW_TYPE_NORMAL.
  */
void
TwmGetWMWindowType(TwmWindow *twin, unsigned *type)
{
    if (twin->transient)
	*type = _NET_WM_WINDOW_TYPE_NORMAL;
    else
	*type = _NET_WM_WINDOW_TYPE_DIALOG;
}

/** @brief Set the window type.
  * @param twin - TWM window
  * @param type - window type bitmask
  */
void
TwmSetWMWindowType(TwmWindow *twin, unsigned type)
{
    unsigned m, j, result;
    union WindowLists *list = &twin->list;
    union WindowFunctions *func = &twin->func;
    union WindowDecorations *decor = &twin->decor;
    short *layer = &twin->ontoppriority;
    
    unsigned old_lists = list->lists;
    unsigned old_decor = decor->decorations;
    unsigned old_funcs = func->functions;
    short old_layer = *layer;

    if (twin->ewmh.props._KDE_NET_WM_WINDOW_TYPE_OVERRIDE) {
	func->functions = 0;
	decor->decorations = 0;
    } else {
	func->functions = -1U;
	decor->decorations = -1U;
    }
    list->lists = -1U;
    layer[0] = CTWM_LAYER_NORMAL;

    if (type == 0)
	TwmGetWMWindowType(twin, &type);

    for (j = 0, m = 1; j < 31; j++, m <<= 1) {
	if (type & m) {
	    switch (j) {
	    case _NET_WM_WINDOW_TYPE_DESKTOP_BIT:
		func->functions = 0;
		decor->decorations = 0;
		list->lists = 0;
		layer[0] = CTWM_LAYER_DESKTOP;
		break;
	    case _NET_WM_WINDOW_TYPE_DOCK_BIT:
		func->functions = 0;
		decor->decorations = 0;
		list->lists = 0;
		layer[0] = CTWM_LAYER_DOCK;
		break;
	    case _NET_WM_WINDOW_TYPE_TOOLBAR_BIT:
		layer[0] = CTWM_LAYER_NORMAL;
		break;
	    case _NET_WM_WINDOW_TYPE_MENU_BIT:
		layer[0] = CTWM_LAYER_NORMAL;
		break;
	    case _NET_WM_WINDOW_TYPE_UTILITY_BIT:
		layer[0] = CTWM_LAYER_NORMAL;
		break;
	    case _NET_WM_WINDOW_TYPE_SPLASH_BIT:
		func->functions = 0;
		decor->decorations = 0;
		list->lists = 0;
		layer[0] = CTWM_LAYER_ABOVE_DOCK;
		break;
	    case _NET_WM_WINDOW_TYPE_DIALOG_BIT:
		layer[0] = CTWM_LAYER_NORMAL;
		break;
	    case _NET_WM_WINDOW_TYPE_DROPDOWN_MENU_BIT:
		layer[0] = CTWM_LAYER_MENU;
		break;
	    case _NET_WM_WINDOW_TYPE_POPUP_MENU_BIT:
		layer[0] = CTWM_LAYER_MENU;
		break;
	    case _NET_WM_WINDOW_TYPE_TOOLTIP_BIT:
		layer[0] = CTWM_LAYER_ABOVE_DOCK;
		break;
	    case _NET_WM_WINDOW_TYPE_NOTIFICATION_BIT:
		layer[0] = CTWM_LAYER_ABOVE_DOCK;
		break;
	    case _NET_WM_WINDOW_TYPE_COMBO_BIT:
		layer[0] = CTWM_LAYER_MENU;
		break;
	    case _NET_WM_WINDOW_TYPE_DND_BIT:
		layer[0] = CTWM_LAYER_MENU;
		break;
	    case _NET_WM_WINDOW_TYPE_NORMAL_BIT:
		layer[0] = CTWM_LAYER_NORMAL;
		break;
	    }
	}
    }

}

#define _NET_WM_STATE_ALLZOOM_MASK ( \
	_NET_WM_STATE_MAXIMIZED_HORZ | \
	_NET_WM_STATE_MAXIMIZED_VERT | \
	_NET_WM_STATE_FULLSCREEN | \
	_NET_WM_STATE_MAXIMUS_BOTTOM | \
	_NET_WM_STATE_MAXIMUS_LEFT | \
	_NET_WM_STATE_MAXIMUS_RIGHT | \
	_NET_WM_STATE_MAXIMUS_TOP )

/** @brief Get the current state of a window.
  * @param twin - TWM window
  * @param flags - where to store the state flags
  *
  * Note that not all state is maintained by CTWM.  There are some state bits in
  * which CTWM is just not interested at all.  These bits are maintained in the
  * twin->ewmh.state bitmask itself.
  */
void
TwmGetWMState(TwmWindow *twin, unsigned *flags)
{
    unsigned state = twin->ewmh.state;

    if (twin->squeezed)
	state |= _NET_WM_STATE_SHADED;
    else
	state &= ~_NET_WM_STATE_SHADED;

#if 0
    /* Note that this is not really what sticky means.  Occupy all is a
       different concept.  Sticky means to stick to the viewport (and therefore
       move with it). */
    if (twin->occupation == fullOccupation)
	state |= _NET_WM_STATE_STICKY;
    else
	state &= ~_NET_WM_STATE_STICKY;
#endif

    state &= ~_NET_WM_STATE_ALLZOOM_MASK;
    switch (twin->zoomed) {
    case ZOOM_NONE:
	break;
    case F_ZOOM:
	state |= _NET_WM_STATE_MAXIMIZED_VERT;
	break;
    case F_LEFTZOOM:
	state |= _NET_WM_STATE_MAXIMUS_LEFT;
	state |= _NET_WM_STATE_MAXIMIZED_VERT;
	break;
    case F_RIGHTZOOM:
	state |= _NET_WM_STATE_MAXIMUS_RIGHT;
	state |= _NET_WM_STATE_MAXIMIZED_VERT;
	break;
    case F_BOTTOMZOOM:
	state |= _NET_WM_STATE_MAXIMUS_BOTTOM;
	state |= _NET_WM_STATE_MAXIMIZED_HORZ;
	break;
    case F_TOPZOOM:
	state |= _NET_WM_STATE_MAXIMUS_TOP;
	state |= _NET_WM_STATE_MAXIMIZED_HORZ;
	break;
    case F_FULLZOOM:
	state |= _NET_WM_STATE_MAXIMIZED_VERT;
	state |= _NET_WM_STATE_MAXIMIZED_HORZ;
	break;
    case F_HORIZOOM:
	state |= _NET_WM_STATE_MAXIMIZED_HORZ;
	break;
    }

    if (twin->isicon)
	state |= _NET_WM_STATE_HIDDEN;
    else
	state &= ~_NET_WM_STATE_HIDDEN;

    /* FIXME: do other bits */

    *flags = state;
}

#define _NET_WM_STATE_MAXIMIZED_FULL \
    (_NET_WM_STATE_MAXIMIZED_HORZ|_NET_WM_STATE_MAXIMIZED_VERT)

#define _NET_WM_STATE_MAXIMUS \
    (_NET_WM_STATE_MAXIMUS_BOTTOM | \
     _NET_WM_STATE_MAXIMUS_TOP | \
     _NET_WM_STATE_MAXIMUS_LEFT | \
     _NET_WM_STATE_MAXIMUS_RIGHT)

/** @brief Set the window state.
  * @param twin - TWM window
  * @parma state - state to set
  *
  * This is normally called when a window is intially mapped.  We need to
  * restore any conditions that are indicated in the state parameter.
  */
void
TwmSetWMState(TwmWindow *twin, unsigned state)
{
    unsigned m, j, result;

    /* handle aliases */
    if (state & _NET_WM_STATE_STAYS_AT_BOTTOM) {
	state &= ~_NET_WM_STATE_STAYS_AT_BOTTOM;
	state |= _NET_WM_STATE_BELOW;
    }
    if (state & _NET_WM_STATE_STAYS_ON_TOP) {
	state &= ~_NET_WM_STATE_STAYS_ON_TOP;
	state |= _NET_WM_STATE_ABOVE;
    }

    if (!(state ^ twin->ewmh.state))
	return;

    for (j = 0, m = 1; j < 31; j++, m <<= 1) {
	if ((state ^ twin->ewmh.state) & m) {
	    switch (j) {
	    case _NET_WM_STATE_MODAL_BIT:
		break;
	    case _NET_WM_STATE_STICKY_BIT:
		/* just set or clear the bit */
		break;
	    case _NET_WM_STATE_MAXIMIZED_HORZ_BIT:
	    case _NET_WM_STATE_MAXIMIZED_VERT_BIT:
		if (!(state & (_NET_WM_STATE_MAXIMUS | _NET_WM_STATE_FULLSCREEN))) {
		    switch (state & _NET_WM_STATE_MAXIMIZED_FULL) {
		    case 0:
			if (twin->zoomed != ZOOM_NONE)
			    fullzoom(twin, twin->zoomed);
			break;
		    case _NET_WM_STATE_MAXIMIZED_HORZ:
			if (twin->zoomed != F_HORIZOOM)
			    fullzoom(twin, F_HORIZOOM);
			break;
		    case _NET_WM_STATE_MAXIMIZED_VERT:
			if (twin->zoomed != F_ZOOM)
			    fullzoom(twin, F_ZOOM);
			break;
		    case _NET_WM_STATE_MAXIMIZED_FULL:
			if (twin->zoomed != F_FULLZOOM)
			    fullzoom(twin, F_FULLZOOM);
			break;
		    }
		}
		break;
	    case _NET_WM_STATE_SHADED_BIT:
		if (((state & m) && !twin->squeezed) || (!(state & m) && twin->squeezed))
		    Squeeze(twin);
		break;
	    case _NET_WM_STATE_SKIP_TASKBAR_BIT:
		/* just set or clear the bit */
		break;
	    case _NET_WM_STATE_SKIP_PAGER_BIT:
		/* just set or clear the bit */
		break;
	    case _NET_WM_STATE_HIDDEN_BIT:
		/* actually minimized */
		if (state & m) {
		    if (!twin->isicon)
			Iconify(twin, 0, 0);
		} else {
		    if (twin->isicon)
			DeIconify(twin);
		}
		break;
	    case _NET_WM_STATE_FULLSCREEN_BIT:
		if (twin->zoomed != F_FULLZOOM)
		    fullzoom(twin, F_FULLZOOM);
		break;
	    case _NET_WM_STATE_ABOVE_BIT:
		break;
	    case _NET_WM_STATE_BELOW_BIT:
		break;
	    case _NET_WM_STATE_DEMANDS_ATTENTION_BIT:
		break;
	    case _NET_WM_STATE_FOCUSED_BIT:
		break;
	    case _NET_WM_STATE_DECOR_BORDER_BIT:
		/* just set or clear the bit */
		break;
	    case _NET_WM_STATE_DECOR_HANDLE_BIT:
		/* just set or clear the bit */
		break;
	    case _NET_WM_STATE_DECOR_TITLE_BIT:
		/* just set or clear the bit */
		break;
	    case _NET_WM_STATE_DECOR_BIT:
		/* just set or clear the bit */
		break;
	    case _NET_WM_STATE_PARENTRELATIVE_BACKGROUND_BIT:
		break;
	    case _NET_WM_STATE_STAYS_AT_BOTTOM_BIT:
		break;
	    case _NET_WM_STATE_STAYS_ON_TOP_BIT:
		break;
	    case _NET_WM_STATE_MAXIMUS_BOTTOM_BIT:
		if (state & m)
		    if (twin->zoomed != F_BOTTOMZOOM)
			fullzoom(twin, F_BOTTOMZOOM);
		break;
	    case _NET_WM_STATE_MAXIMUS_LEFT_BIT:
		if (state & m)
		    if (twin->zoomed != F_LEFTZOOM)
			fullzoom(twin, F_LEFTZOOM);
		break;
	    case _NET_WM_STATE_MAXIMUS_RIGHT_BIT:
		if (state & m)
		    if (twin->zoomed != F_RIGHTZOOM)
			fullzoom(twin, F_RIGHTZOOM);
		break;
	    case _NET_WM_STATE_MAXIMUS_TOP_BIT:
		if (state & m)
		    if (twin->zoomed != F_TOPZOOM)
			fullzoom(twin, F_TOPZOOM);
		break;
	    case _NET_WM_STATE_AUTORAISE_BIT:
		break;
	    case _NET_WM_STATE_AUTOLOWER_BIT:
		break;
	    default:
		break;
	    }
	    if (state & m)
		twin->ewmh.state |= m;
	    else
		twin->ewmh.state &= ~m;
	}
    }

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
		    if (twin->ewmh.state & _NET_WM_STATE_STICKY)
			ChangeOccupation(tmp_win, 1 << current);
		break;
	    case _NET_WM_STATE_ADD:
		if (tmp_win != NULL)
		    if (!(twin->ewmh.state & _NET_WM_STATE_STICKY))
			if (twin->ewmh.allowed & _NET_WM_ACTION_STICK)
			    OccupyAll(twin);
		break;
	    case _NET_WM_STATE_TOGGLE:
		if (tmp_win != NULL) {
		    if (twin->ewmh.state & _NET_WM_STATE_STICKY)
			ChangeOccupation(tmp_win, 1 << current);
		    else if (twin->ewmh.allowed & _NET_WM_ACTION_STICK)
			OccupyAll(twin);
		}
		break;
	    default:
		break;
	    }
	    break;
	case _NET_WM_STATE_MAXIMIZED_HORZ:
	    if (twin->ewmh.allowed & _NET_WM_ACTION_MAXIMIZE_HORZ)
		break;
	    break;
	case _NET_WM_STATE_MAXIMIZED_VERT:
	    if (twin->ewmh.allowed & _NET_WM_ACTION_MAXIMIZE_VERT)
		break;
	    break;
	case _NET_WM_STATE_SHADED:
	    switch (action) {
	    case _NET_WM_STATE_REMOVE:
		if (twin->ewmh.state & _NET_WM_STATE_SHADED)
		    Squeeze(twin);
		break;
	    case _NET_WM_STATE_ADD:
		if (!(twin->ewmh.state & _NET_WM_STATE_SHADED))
		    if (twin->ewmh.allowed & _NET_WM_ACTION_SHADE)
			Squeeze(twin);
		break;
	    case _NET_WM_STATE_TOGGLE:
		if (twin->ewmh.state & _NET_WM_STATE_SHADED)
		    Squeeze(twin);
		else if (twin->ewmh.allowed & _NET_WM_ACTION_SHADE)
		    Squeeze(twin);
		break;
	    default:
		break;
	    }
	    break;
	case _NET_WM_STATE_SKIP_TASKBAR:
	case _NET_WM_STATE_SKIP_PAGER:
	case _NET_WM_STATE_HIDDEN:
	case _NET_WM_STATE_DEMANDS_ATTENTION:
	    switch (action) {
	    case _NET_WM_STATE_REMOVE:
		twin->ewmh.state &= ~action1;
		break;
	    case _NET_WM_STATE_ADD:
		twin->ewmh.state |= action1;
		break;
	    case _NET_WM_STATE_TOGGLE:
		twin->ewmh.state ^= action1;
		break;
	    default:
		break;
	    }
	    break;
	case _NET_WM_STATE_FULLSCREEN:
	    if (twin->ewmh.allowed & _NET_WM_ACTION_FULLSCREEN)
		break;
	    break;
	case _NET_WM_STATE_ABOVE:
	case _NET_WM_STATE_STAYS_ON_TOP:
	    if (twin->ewmh.allowed & _NET_WM_ACTION_ABOVE)
		break;
	    break;
	case _NET_WM_STATE_BELOW:
	case _NET_WM_STATE_STAYS_AT_BOTTOM:
	    if (twin->ewmh.allowed & _NET_WM_ACTION_BELOW)
		break;
	    break;
	case _NET_WM_STATE_FOCUSED:
	    /* read only */
	    break;
/* NON-STANDARD starts here: */
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
	case _NET_WM_STATE_MAXIMUS_BOTTOM:
	    if (twin->ewmh.allowed & _NET_WM_ACTION_MAXIMIZE_HORZ)
		break;
	    break;
	case _NET_WM_STATE_MAXIMUS_LEFT:
	    if (twin->ewmh.allowed & _NET_WM_ACTION_MAXIMIZE_VERT)
		break;
	    break;
	case _NET_WM_STATE_MAXIMUS_RIGHT:
	    if (twin->ewmh.allowed & _NET_WM_ACTION_MAXIMIZE_VERT)
		break;
	    break;
	case _NET_WM_STATE_MAXIMUS_TOP:
	    if (twin->ewmh.allowed & _NET_WM_ACTION_MAXIMIZE_HORZ)
		break;
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
		    if (twin->ewmh.state & _NET_WM_STATE_STICKY)
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
		if (twin->ewmh.state & _NET_WM_STATE_SHADED)
		    Squeeze(twin);
		break;
	    case _NET_WM_STATE_ADD:
		if (!(twin->ewmh.state & _NET_WM_STATE_SHADED))
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
	case _NET_WM_STATE_SKIP_PAGER:
	case _NET_WM_STATE_HIDDEN:
	case _NET_WM_STATE_DEMANDS_ATTENTION:
	    switch (action) {
	    case _NET_WM_STATE_REMOVE:
		twin->ewmh.state &= ~action2;
		break;
	    case _NET_WM_STATE_ADD:
		twin->ewmh.state |= action2;
		break;
	    case _NET_WM_STATE_TOGGLE:
		twin->ewmh.state ^= action2;
		break;
	    default:
		break;
	    }
	    break;
	case _NET_WM_STATE_FULLSCREEN:
	    break;
	case _NET_WM_STATE_ABOVE:
	case _NET_WM_STATE_STAYS_ON_TOP:
	    break;
	case _NET_WM_STATE_BELOW:
	case _NET_WM_STATE_STAYS_AT_BOTTOM:
	    break;
	case _NET_WM_STATE_FOCUSED:
	    /* read-only */
	    break;
/* NON-STANDARD starts here: */
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

/** @brief Get the allowed actions for a window.
  * @param twin - TWM window
  * @param allowed - where to return the allowed actions
  */
void
TwmGetWMAllowedActions(TwmWindow *twin, unsigned *allowed)
{
    *allowed = 0;
    if (twin->func.function.move)
	*allowed |= _NET_WM_ACTION_MOVE;
    if (twin->func.function.resize)
	*allowed |= _NET_WM_ACTION_RESIZE;
    if (twin->func.function.minimize)
	*allowed |= _NET_WM_ACTION_MINIMIZE;
    if (twin->func.function.shade)
	*allowed |= _NET_WM_ACTION_SHADE;
    if (twin->func.function.stick)
	*allowed |= _NET_WM_ACTION_STICK;
    if (twin->func.function.maximize_horz)
	*allowed |= _NET_WM_ACTION_MAXIMIZE_HORZ;
    if (twin->func.function.maximize_vert)
	*allowed |= _NET_WM_ACTION_MAXIMIZE_VERT;
    if (twin->func.function.maximize)
	*allowed |= _NET_WM_ACTION_MAXIMIZE_VERT | _NET_WM_ACTION_MAXIMIZE_HORZ;
    if (twin->func.function.maximus)
	*allowed |= _NET_WM_ACTION_MAXIMIZE_VERT | _NET_WM_ACTION_MAXIMIZE_HORZ;
    if (twin->func.function.maximus_left | twin->func.function.maximus_right)
	*allowed |= _NET_WM_ACTION_MAXIMIZE_VERT;
    if (twin->func.function.maximus_top | twin->func.function.maximus_bottom)
	*allowed |= _NET_WM_ACTION_MAXIMIZE_HORZ;
    if (twin->func.function.fullscreen)
	*allowed |= _NET_WM_ACTION_FULLSCREEN;
    if (twin->func.function.change_desktop)
	*allowed |= _NET_WM_ACTION_CHANGE_DESKTOP;
    if (twin->func.function.close)
	*allowed |= _NET_WM_ACTION_CLOSE;
    if (twin->func.function.above)
	*allowed |= _NET_WM_ACTION_ABOVE;
    if (twin->func.function.below)
	*allowed |= _NET_WM_ACTION_BELOW;
}

/** @brief Get the pid for a window.
  * @param scr - screen
  * @param twin - TWM window
  * @param pid - where to store the pid
  *
  * This function assists clients that neglect to set the NetWM/EWMH _NET_WM_PID
  * property when a compliant XDG startup notification launcher has been used to
  * launch the application.
  */
void
TwmGetWMPid(ScreenInfo *scr, TwmWindow *twin, pid_t *pid)
{
    EwmhSequence *seq;

    if ((seq = twin->ewmh.sequence) != NULL && seq->field.pid != NULL)
	*pid = seq->numb.pid;
}


/** @brief Set the pid for a window.
  * @param scr - screen
  * @param twin - TWM window
  * @param pid - the pid to set
  *
  * This sets the specified process id value in the PID= field of the associated
  * startup notification sequence, if any, and sends a notification change
  * message when possible.
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
	Chg_NET_STARTUP_INFO(scr, seq);
    }
}

/** @brief Update handled icons for a screen.
  * @param scr - screen
  *
  * Updates whether any window (i.e. task bar, etc.) is handling icons.
  */
void
TwmUpdWMHandledIcons(ScreenInfo *scr)
{
    TwmWindow *twin;
    Bool handled = False;

    scr->ewmh.handled_icons = False;

    for (twin = scr->FirstWindow; twin != NULL; twin = twin->next) {
	if (twin->ewmh.props._NET_WM_HANDLED_ICONS) {
	    handled = True;
	    break;
	}
    }
    if (scr->ewmh.handled_icons == handled)
	return;
    scr->ewmh.handled_icons = handled;
    if (handled) {
	TwmWindow *t;

	/* set for no icons and unmap existing icons */
	scr->ewmh.IconifyByUnmapping = scr->IconifyByUnmapping;
	scr->IconifyByUnmapping = TRUE;
	for (t = scr->FirstWindow; t != NULL; t = t->next) {
	    t->iconify_by_unmapping = True;
	    if (t->isicon) {
		if (t->icon && t->icon->w) {
		    XUnmapWindow(dpy, t->icon->w);
		    IconDown(t);
		    if (scr->ShrinkIconTitles)
			t->icon->title_shrunk = True;
		}
		t->icon_on = FALSE;
	    }
	}
    } else {
	TwmWindow *t;

	/* set for icons and map existing icons */
	scr->IconifyByUnmapping = scr->ewmh.IconifyByUnmapping;
	for (t = scr->FirstWindow; t != NULL; t = t->next) {
	    /* restore flag: copied from add_window.c */
	    t->iconify_by_unmapping = Scr->IconifyByUnmapping;
	    if (Scr->IconifyByUnmapping) {
		t->iconify_by_unmapping =
		    t->iconmgr ? FALSE : !LookInList(Scr->DontIconify, t->full_name,
						     &t->class);
	    }
	    t->iconify_by_unmapping = t->iconify_by_unmapping
		|| LookInList(Scr->IconifyByUn, t->full_name, &t->class);
	    /* bring up icon if necessary */
	    if (t->isicon && !t->iconify_by_unmapping && !t->icon_on) {
		if (!t->icon || !t->icon->w)
		    CreateIconWindow(t, 0, 0);
		else
		    IconUp(t);
		if (visible(t)) {
		    if (Scr->WindowMask) {
			XRaiseWindow(dpy, Scr->WindowMask);
			XMapWindow(dpy, t->icon->w);
		    } else
			XMapRaised(dpy, t->icon->w);
		}
		t->icon_on = TRUE;
	    }
	}
    }
}

void
TwmGetWMUserTimeWindow(ScreenInfo *scr, TwmWindow *twin, Window *time_window)
{
    Window window = twin->w;

    if (twin->ewmh.props._NET_WM_USER_TIME_WINDOW && twin->ewmh.user_time_window != None)
	window = twin->ewmh.user_time_window;
    *time_window = window;
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
TwmSetUserTime(Time thisUserTime)
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
TwmGetUserTime(Time *thisUserTime)
{
    if (thisUserTime != NULL)
	*thisUserTime = lastUserTime;
}

/** @brief Get the user time for a window.
  * @param scr - screen
  * @param twin - TWM window
  * @param time - where to store the user time
  *
  * This function assists clients that neglect to set the NetWM/EWMH
  * _NET_WM_USER_TIME property when a compliant XDG startup notification
  * launcher has been uysed to launch the application.
  */
void
TwmGetWMUserTime(ScreenInfo *scr, TwmWindow *twin, Time *time)
{
    EwmhSequence *seq;

    if ((seq = twin->ewmh.sequence) != NULL &&
	    seq->field.timestamp != NULL &&
	    seq->numb.timestamp != 0) {
	*time = seq->numb.timestamp;
	TwmSetUserTime(*time);
    }
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

void
TwmGetFullscreenMonitors(ScreenInfo *scr, int *monitors)
{
    VirtualScreen *vs;
    int count = 0;

    for (vs = scr->vScreenList; vs != NULL; vs = vs->next)
	count++;
    *monitors = count;
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

/** @brief Initialize the fullscreen monitors for a window.
  * @param scr - screen
  * @param twin - TWM window
  * @param monitors - fullscreen monitors to initialize
  */
void
TwmIniWMFullscreenMonitors(ScreenInfo *scr, TwmWindow *twin, struct NetMonitors *monitors)
{
    VirtualScreen *vs;
    EwmhSequence *seq;
    int monitor = 0;
    int number = 1;

    TwmGetFullscreenMonitors(scr, &number);

    if ((vs = scr->currentvs) != NULL)
	monitor = vs->monitor;

    if ((vs = twin->vs) != NULL)
	monitor = vs->monitor;

    if ((seq = twin->ewmh.sequence) != NULL && seq->field.screen != NULL
	&& 0 <= seq->numb.screen && seq->numb.screen < number)
	monitor = seq->numb.screen;

    monitors->top = monitors->bottom = monitors->left = monitors->right = monitor;
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

void
TwmGetGroupLeader(TwmWindow *twin, Window *leader)
{
    Window window = None;
    if (twin->wmhints != NULL && (window = twin->wmhints->window_group) != None
	    && window != twin->w)
	*leader = window;
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
TwmGetWMVirtualPos(ScreenInfo *scr, TwmWindow *twin, struct NetPosition *virtual_pos)
{
    VirtualScreen *vs;

    if ((vs = twin->old_parent_vs) != NULL) {
	virtual_pos->x = vs->x + twin->frame_x;
	virtual_pos->y = vs->y + twin->frame_y;
    } else {
	virtual_pos->x = twin->frame_x;
	virtual_pos->y = twin->frame_y;
    }
}

void
TwmSetWMVirtualPos(ScreenInfo *scr, TwmWindow *twin, struct NetPosition *virtual_pos)
{
    /* we don't actually change the position of the window */
}

void
TwmSetWMSyncRequestCounter(TwmWindow *twin, struct NetCounter *counter)
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
TwmGetMaximizedRestore(ScreenInfo *scr, TwmWindow *twin, struct NetRestore *restore)
{
    VirtualScreen *vs;

    if (twin->zoomed == ZOOM_NONE) {
	restore->x = twin->frame_x;
	restore->y = twin->frame_y;
	restore->width = twin->frame_width;
	restore->height = twin->frame_height;

    } else {
	restore->x = twin->save_frame_x;
	restore->y = twin->save_frame_y;
	restore->width = twin->save_frame_width;
	restore->height = twin->save_frame_height;
    }
    if ((vs = twin->old_parent_vs) == NULL) {
	restore->vx = 0;
	restore->vy = 0;
    } else {
	restore->vx = vs->x;
	restore->vy = vs->y;
    }
}

void
TwmSetMaximizedRestore(ScreenInfo *scr, TwmWindow *twin, struct NetRestore *restore)
{
    VirtualScreen *vs;

    vs = findIfVScreenOf(restore->vx, restore->vy);
    if (vs != NULL)
	twin->old_parent_vs = vs;

    if (twin->zoomed == ZOOM_NONE) {
	SetupWindow(twin, restore->x, restore->y, restore->width, restore->height, -1);
	if (vs) {
	    if (twin->vs != NULL && twin->vs != vs) {
		/* FIXME: change virtual screens */
	    }
	}
    } else {
	twin->save_frame_x = restore->x;
	twin->save_frame_y = restore->y;
	twin->save_frame_width = restore->width;
	twin->save_frame_height = restore->height;
    }
}

void
TwmSetWMSystemTrayWindowFor(TwmWindow *twin)
{
}

void
TwmSetWMWindowTypeOverride(TwmWindow *twin)
{
    twin->func.functions = 0U;
    twin->decor.decorations = 0U;
}

// vim: sw=4 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
