#include <stdio.h>
#include "wmh.h"
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
TwmWinRoot(ScreenInfo *scr)
{
    return scr->XineramaRoot;
}

/** @brief Get the manager window for screen.
  * @param scr - screen
  */
Window
TwmWinManager(ScreenInfo *scr)
{
    return scr->ManagerWindow;
}

extern int CanChangeOccupation(TwmWindow **twm_winp);

static TwmWindow *
TwmCanChangeWorkspace(TwmWindow *twin)
{
    if (!CanChangeOccupation(&twin))
	return (NULL);
    return (twin);
}

/** @file
  *
  * This file provides implementation of WinWM/WMH hook functions for CTWM.  The
  * WinWM/WMH functions in wmh.c are written to be independent of the TWM
  * variant that uses them.  These hook functions allow the WinWM/WMH module to
  * interrogate and perform actions on the TWM variant of window manager.
  * Implementation modules are provided for TWM, VTWM, CTWM and ETWM.  This is
  * the implementation module for CTWM.
  */

/** @brief Get the client window list.
  * @param windows - pointer to where to return the list of windows
  * @param count - pointer to where to return the count
  *
  * Note that windows are in no particular order.
  */
void
TwmGetWinClientList(ScreenInfo *scr, Window **windows, int *count)
{
    TwmWindow *twin;
    int number = 0;

    for (twin = scr->FirstWindow; twin != NULL; twin = twin->next)
	number++;
    if (*windows != NULL) {
	free(*windows);
	*windows = NULL;
    }
    if ((*windows = calloc(number + 1, sizeof(Window))) != NULL) {
	int i = 0;

	for (twin = scr->FirstWindow; twin != NULL; twin = twin->next)
	    (*windows)[i++] = twin->w;
    }
    *count = *windows == NULL ? 0 : number;
}

/** @brief Get the workspace count.
  * @param count - where to return the count
  *
  * Gets the count of desktops (workspaces) that are currently provided by the
  * window manager.
  */
void
TwmGetWorkspaceCount(ScreenInfo *scr, int *count)
{
    *count = scr->workSpaceManagerActive ? scr->workSpaceMgr.count : 1;
}

/** @brief Set the workspace count.
  * @param count - count of workspaces to set.
  *
  * Set the count of desktops (workspace) that are currently provided by the
  * window manager.  This function must peform sanity checking of the number as
  * the number is exactly as provided in the client message.
  *
  * Returns True when the count was actually updated as requested.
  */
void
TwmSetWorkspaceCount(ScreenInfo *scr, int count)
{
    int old_count = 0;

    TwmGetWorkspaceCount(scr, &old_count);
    if (count == old_count)
	return;
    if (count < 1 || count > 32)
	return;
    /* TODO: actually increase or decrease the workspace count. */
}

/** @brief Get the active workspace.
  * @param workspace - where to return the workspace
  *
  * There is only one workspace when the workspace manager is not active.  CTWM
  * reverts to TWM mode in that case.
  */
void
TwmGetWorkspace(ScreenInfo *scr, int *workspace)
{
    VirtualScreen *vs;

    if (!scr->workSpaceManagerActive) {
	*workspace = 0;
	return;
    }
    if (scr == NULL) {
	fprintf(stderr, "ERROR: Screen is NULL!\n");
	*workspace = 0;
	return;
    }
    if ((vs = scr->currentvs) == NULL && (vs = scr->vScreenList) == NULL) {
	fprintf(stderr, "ERROR: Current virtual screen is NULL!\n");
	*workspace = 0;
	return;
    }
    if (vs->wsw == NULL) {
	fprintf(stderr, "ERROR: Workspace window is NULL!\n");
	*workspace = 0;
	return;
    }
    if (vs->wsw->currentwspc == NULL) {
	fprintf(stderr, "ERROR: Current workspace is NULL!\n");
	*workspace = 0;
	return;
    }

    *workspace = vs->wsw->currentwspc->number;
}

/** @brief Set the active workspace.
  * @param workspace - the workspace to activate.
  *
  * There is only one desktop when the workspace manager is not active.  CTWM
  * reverts to TWM mode in that case.
  */
void
TwmSetWorkspace(ScreenInfo *scr, int workspace)
{
    int old_workspace = 0, count = 0;

    if (!scr->workSpaceManagerActive)
	return;

    TwmGetWorkspace(scr, &old_workspace);
    if (workspace == old_workspace)
	return;
    TwmGetWorkspaceCount(scr, &count);
    if (workspace < 0 || workspace >= count)
	return;
    GotoWorkSpaceByNumber(scr->currentvs, workspace);
}

/** @brief Get the workspace names.
  * @param names - where to return the list of names.
  * @param count - where to return the names count.
  *
  * Get's the window manager's list of workspace names.  This is normally used
  * for setting the _NET_DESKTOP_NAMES property on the root window.
  */
void
TwmGetWorkspaceNames(ScreenInfo *scr, char ***names, int *count)
{
    struct WorkSpace *ws;
    int number = 0, n = 0;
    char **list = NULL, *name;

    /* There are no workspace names unless the workspace manager is active.  CTWM 
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

/** @brief Set the workspace names.
  * @param names - the list of names to set.
  * @param count - the number of names in the list.
  *
  * Sets the workspace names displayed by the window manager.
  */
void
TwmSetWorkspaceNames(ScreenInfo *scr, char **names, int count)
{
    struct WorkSpace *ws = NULL;
    int n, number = 0;

    if (!scr->workSpaceManagerActive)
	return;

    TwmGetWorkspaceCount(scr, &number);
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
    PaintWorkSpaceManager(scr->currentvs);
}

/** @brief Get the layer for a window.
  * @param twin - the TWM window
  * @param layer - where to return the layer
  *
  * CTWM does not really have layers; however, it does have an ontop priority
  * that has the same effect as layers.  It originally only used two levels:
  * ONTOP_DEFAULT (8) for normal windows and ONTOP_MAX (16) for windows that are
  * specified as being ontop.
  */
void
TwmGetWinLayer(TwmWindow *twin, unsigned *layer)
{
    short ontop = twin->ontoppriority;
    *layer = twin->ontoppriority / 2;
}

/** @brief Set the layer for a window.
  * @param twin - the TWM window
  * @param layer - the layer for the window
  *
  * CTWM does not really have layers; however, it does have an ontop priority
  * that has the same effect as layers.
  */
void
TwmSetWinLayer(TwmWindow *twin, unsigned layer)
{
    switch (layer) {
    case WIN_LAYER_DESKTOP:
    case 1:
    case WIN_LAYER_BELOW:
    case 3:
    case WIN_LAYER_NORMAL:
    case 5:
    case WIN_LAYER_ONTOP:
    case 7:
    case WIN_LAYER_DOCK:
    case 9:
    case WIN_LAYER_ABOVE_DOCK:
    case 11:
    case WIN_LAYER_MENU:
	twin->ontoppriority = layer * 2;
	break;
    default:
	twin->ontoppriority = WIN_LAYER_MENU;
	break;
    }
}

extern int fullOccupation;

/** @brief Get the window state.
  * @param scr - screen
  * @param twin - the TWM window.
  * @param state - where to return the state.
  */
void
TwmGetWinState(ScreenInfo *scr, TwmWindow *twin, unsigned *state)
{
    unsigned flags = twin->wmh.state;

    /* Note that this is not really what sticky means.  Occupy all is a
       different concept.  Sticky means to stick to the viewport (and therefore
       move with it). */

    /* WIN_STATE_STICKY */
    if (twin->occupation == fullOccupation)
	flags |= WIN_STATE_STICKY;
    else
	flags &= ~WIN_STATE_STICKY;

    /* WIN_STATE_MINIMIZED */
    if (twin->isicon)
	flags |= WIN_STATE_MINIMIZED;
    else
	flags &= ~WIN_STATE_MINIMIZED;

    /* WIN_STATE_MAXIMIZED_VERT, WIN_STATE_MAXIMIZED_HORIZ */
    flags &= ~(WIN_STATE_MAXIMIZED_VERT | WIN_STATE_MAXIMIZED_HORIZ);
    switch (twin->zoomed) {
    case ZOOM_NONE:
	break;
    case F_ZOOM:
	flags |= WIN_STATE_MAXIMIZED_VERT;
	break;
    case F_LEFTZOOM:
	flags |= WIN_STATE_MAXIMIZED_VERT;
	break;
    case F_RIGHTZOOM:
	flags |= WIN_STATE_MAXIMIZED_VERT;
	break;
    case F_BOTTOMZOOM:
	flags |= WIN_STATE_MAXIMIZED_HORIZ;
	break;
    case F_TOPZOOM:
	flags |= WIN_STATE_MAXIMIZED_HORIZ;
	break;
    case F_FULLZOOM:
	flags |= WIN_STATE_MAXIMIZED_VERT;
	flags |= WIN_STATE_MAXIMIZED_HORIZ;
	break;
    case F_HORIZOOM:
	flags |= WIN_STATE_MAXIMIZED_HORIZ;
	break;
    }
    /* WIN_STATE_HIDDEN */
    if (twin->list.list.task)
	flags &= ~WIN_STATE_HIDDEN;
    else
	flags |= WIN_STATE_HIDDEN;

    /* WIN_STATE_SHADED */
    if (twin->squeezed)
	flags |= WIN_STATE_SHADED;
    else
	flags &= ~WIN_STATE_SHADED;

    /* WIN_STATE_HID_WORKSPACE */
    if ((twin->vs && twin->vs == scr->currentvs)
	|| (twin->savevs && twin->savevs == scr->currentvs))
	flags &= ~WIN_STATE_HID_WORKSPACE;
    else
	flags |= ~WIN_STATE_HID_WORKSPACE;

    /* WIN_STATE_HID_TRANSIENT */
    if (!twin->transient && !twin->mapped)
	flags &= ~WIN_STATE_HID_TRANSIENT;
    else
	flags |= WIN_STATE_HID_TRANSIENT;

    /* WIN_STATE_FIXED_POSITION */
    if (twin->func.function.move)
	flags &= ~WIN_STATE_FIXED_POSITION;
    else
	flags |= WIN_STATE_FIXED_POSITION;

    /* WIN_STATE_ARRANGE_IGNORE */
    if (twin->list.list.window)
	flags &= ~WIN_STATE_ARRANGE_IGNORE;
    else
	flags |= WIN_STATE_ARRANGE_IGNORE;

    *state = flags;
}

extern void fullzoom(TwmWindow *tmp_win, int flag);

/** @brief Set the window state.
  * @param twin - the TWM window
  * @param mask - mask of the states to set
  * @param state - the states to set
  */
void
TwmSetWinState(ScreenInfo *scr, TwmWindow *twin, unsigned mask, unsigned state)
{
    unsigned m, j, result;
    int current = 0;
    TwmWindow *tmp_win;

    tmp_win = TwmCanChangeWorkspace(twin);

    TwmGetWorkspace(scr, &current);

    result = twin->wmh.state;
    result &= ~mask;
    result |= state;

    /* something must actually change */
    if (!(result ^ twin->wmh.state))
	return;

    for (j = 0, m = 1; j < 31; j++, m <<= 1) {
	/* bit must be specified and actually changed the state */
	if ((mask & m) && ((state ^ twin->wmh.state) & m)) {
	    switch (j) {
	    case WIN_STATE_STICKY_BIT:
#if 0
		if (tmp_win != NULL) {
		    if (state & m) {
			if (twin->occupation != fullOccupation)
			    OccupyAll(tmp_win);
		    } else {
			if (twin->occupation == fullOccupation)
			    ChangeOccupation(tmp_win, 1 << current);
		    }
		}
#endif
		break;
	    case WIN_STATE_MINIMIZED_BIT:
		if (state & m) {
		    if (!twin->isicon)
			Iconify(twin, 0, 0);
		} else {
		    if (twin->isicon)
			DeIconify(twin);
		}
		break;
	    case WIN_STATE_MAXIMIZED_VERT_BIT:
	    case WIN_STATE_MAXIMIZED_HORIZ_BIT:
		switch (result & (WIN_STATE_MAXIMIZED_VERT | WIN_STATE_MAXIMIZED_HORIZ)) {
		case 0:
		    switch (twin->zoomed) {
		    case ZOOM_NONE:
			break;
		    default:
			fullzoom(twin, twin->zoomed);
			break;
		    }
		    break;
		case WIN_STATE_MAXIMIZED_VERT:
		    switch (twin->zoomed) {
		    case F_ZOOM:
		    case F_LEFTZOOM:
		    case F_RIGHTZOOM:
			break;
		    default:
			fullzoom(twin, F_ZOOM);
			break;
		    }
		    break;
		case WIN_STATE_MAXIMIZED_HORIZ:
		    switch (twin->zoomed) {
		    case F_BOTTOMZOOM:
		    case F_TOPZOOM:
		    case F_HORIZOOM:
			break;
		    default:
			fullzoom(twin, F_HORIZOOM);
			break;
		    }
		    break;
		case WIN_STATE_MAXIMIZED_VERT | WIN_STATE_MAXIMIZED_HORIZ:
		    switch (twin->zoomed) {
		    case F_LEFTZOOM:
		    case F_RIGHTZOOM:
		    case F_BOTTOMZOOM:
		    case F_TOPZOOM:
		    case F_FULLZOOM:
			break;
		    default:
			fullzoom(twin, F_FULLZOOM);
			break;
		    }
		    break;
		}
		break;
	    case WIN_STATE_HIDDEN_BIT:
		/* Hidden means not on taskbar but window visible.  What we
		   really want to do here is remove this item from the icon
		   manager. */
		/* TODO: set or reset this as far as CTWM is concerned. */
		twin->list.list.task = (state & m) ? 0 : 1;
		break;
	    case WIN_STATE_SHADED_BIT:
		if (((state & m) && !twin->squeezed) || (!(state & m) && twin->squeezed))
		    Squeeze(twin);
		break;
	    case WIN_STATE_HID_WORKSPACE_BIT:
		/* Can't be set by application: not on current desktop. */
		break;
	    case WIN_STATE_HID_TRANSIENT_BIT:
		/* Can't be set by application: owner of transient is hidden. */
		break;
	    case WIN_STATE_FIXED_POSITION_BIT:
		/* TODO: set or reset this as far as CTWM is concerned. */
		twin->func.function.move = (state & m) ? 0 : 1;
		break;
	    case WIN_STATE_ARRANGE_IGNORE_BIT:
		/* TODO: set or reset this as far as CTWM is concerned. */
		twin->list.list.window = (state & m) ? 0 : 1;
		break;
	    default:
		break;
	    }
	    /* always alter tracking bits to catch states that do not have a
	       coprresponding internal state */
	    if (state & m)
		twin->wmh.state |= m;
	    else
		twin->wmh.state &= ~m;
	}
    }
}

/** @brief Get the workspace assigned to a window.
  * @param twin - the TWM window
  * @param workspace - where to return the workspace
  */
void
TwmGetWinWorkspace(ScreenInfo *scr, TwmWindow *twin, int *workspace)
{
    int n, mask, count = 0;

    if (twin->occupation == fullOccupation) {
	*workspace = -1;
	return;
    }
    TwmGetWorkspaceCount(scr, &count);
    *workspace = count;
    for (n = 0, mask = 1; n < count; n++, mask <<= 1) {
	if (twin->occupation & mask) {
	    *workspace = n;
	    break;
	}
    }
}

/** @brief Set the workspace of a window.
  * @param twin - the TWM window
  * @parma workspace - the workspace for the window.
  *
  * Returns true when the workspace is positively set.
  */
void
TwmSetWinWorkspace(ScreenInfo *scr, TwmWindow *twin, int workspace)
{
    int count = 0;

    if ((twin = TwmCanChangeWorkspace(twin)) == NULL)
	return;

    TwmGetWorkspaceCount(scr, &count);
    if (workspace != -1 && (0 > workspace || workspace >= count))
	return;
    if (workspace == -1)
	OccupyAll(twin);
    else
	ChangeOccupation(twin, 1 << workspace);
}

/** @brief Set the expanded size of a window.
  * @param twin - the TWM window
  * @parma expanded - the expanded size
  */
void
TwmSetExpandedSize(TwmWindow *twin, struct WinGeometry *expanded)
{
    twin->wmh.expanded = *expanded;
    /* TODO: actually do something with this */
}

/** @brief Delete the expanded size of a window.
  * @param twin - the TWM window
  * @parma expanded - the expanded size deleted
  */
void
TwmDelExpandedSize(TwmWindow *twin, struct WinGeometry *expanded)
{
    (void) twin;
    expanded->x = expanded->y = 0;
    expanded->width = expanded->height = -1;
}

/** @brief Set the window hints.
  * @param twin - the TWM window
  * @param hints - the hints to set
  */
void
TwmSetWinHints(TwmWindow *twin, unsigned hints)
{
    unsigned m, j;

    for (j = 0, m = 1; j < 31; j++, m <<= 1) {
	if (hints & m) {
	    switch (j) {
	    case WIN_HINTS_SKIP_FOCUS:
		/* TODO: set this */
		break;
	    case WIN_HINTS_SKIP_WINLIST:
		/* TODO: set this */
		break;
	    case WIN_HINTS_SKIP_TASKBAR:
		/* TODO: set this */
		break;
	    case WIN_HINTS_GROUP_TRANSIENT:
		/* TODO: set this */
		break;
	    case WIN_HINTS_FOCUS_ON_CLICK:
		/* TODO: set this */
		break;
	    case WIN_HINTS_DO_NOT_COVER:
		/* TODO: set this */
		break;
	    default:
		break;
	    }
	    twin->wmh.hints |= m;
	} else {
	    switch (j) {
	    case WIN_HINTS_SKIP_FOCUS:
		/* TODO: reset this */
		break;
	    case WIN_HINTS_SKIP_WINLIST:
		/* TODO: reset this */
		break;
	    case WIN_HINTS_SKIP_TASKBAR:
		/* TODO: reset this */
		break;
	    case WIN_HINTS_GROUP_TRANSIENT:
		/* TODO: reset this */
		break;
	    case WIN_HINTS_FOCUS_ON_CLICK:
		/* TODO: reset this */
		break;
	    case WIN_HINTS_DO_NOT_COVER:
		/* TODO: reset this */
		break;
	    default:
		break;
	    }
	    twin->wmh.hints &= ~m;
	}
    }
}

/** @brief Delete the window hints.
  * @param twin - the TWM window
  * @param hints - the hints deleted
  */
void
TwmDelWinHints(TwmWindow *twin, unsigned *hints)
{
    (void) twin;
    *hints = 0;
}

/** @brief Get the area count.
  * @param count - where to return the count.
  *
  * CTWM does not support large desktops, so the area count is always 1 column
  * by 1 row.
  */
void
TwmGetWinAreaCount(ScreenInfo *scr, struct WinLayout *count)
{
    count->cols = 1;
    count->rows = 1;
}

/** @brief Set the area count.
  * @param cols - columns in the area
  * @param rows - rows in the area
  *
  * CTWM does not support large desktops, so the area count is always 1 column
  * by 1 row, and cannot be changed.
  */
void
TwmSetWinAreaCount(ScreenInfo *scr, struct WinLayout *count)
{
    (void) count;
}

/** @brief Get the active area.
  * @param area - where to return the active area.
  *
  * CTWM does not support large desktops, so the area count is always 1 column
  * by 1 row and the active area is always column 0 and row 0.
  */
void
TwmGetWinArea(ScreenInfo *scr, struct WinArea *area)
{
    area->row = 0;
    area->col = 0;
}

/** @brief Set the active area.
  * @param col - column of the active area
  * @param row - row of the active area
  *
  * CTWM does not support large desktops, so the area count is always 1 column
  * by 1 row and the active area is always column 0 and row 0 and cannot be
  * changed.
  */
void
TwmSetWinArea(ScreenInfo *scr, struct WinArea *area)
{
    (void) area;
}

/** @brief Set the application state of a window.
  * @param twin - the TWM window
  * @param app_state - the application state to set.
  */
void
TwmSetWinAppState(TwmWindow *twin, unsigned app_state)
{
    twin->wmh.app_state = app_state;
    /* TODO: do something with the state */
}

/** @brief Delete the application state of a window.
  * @param twin - the TWM window
  * @param app_state - the application state being deleted.
  */
void
TwmDelWinAppState(TwmWindow *twin, unsigned *app_state)
{
    (void) twin;
    *app_state = 0;
}

/** @brief Set the window icons.
  * @param twin - the TWM window
  * @param icons - the coded list of icons.
  */
void
TwmSetWinIcons(TwmWindow *twin, long *icons)
{
    (void) twin;
    (void) icons;
    /* TODO: do something with the icons */
}

/** @brief Get the work area.
  * @param workarea - where to return the work area.
  */
void
TwmGetWinWorkarea(ScreenInfo *scr, struct WinWorkarea *workarea)
{
#ifdef EWMH
    struct NetGeometry workgeom;
    int current = 0;

    TwmGetCurrentDesktop(scr, &current);
    TwmGetWorkarea(scr, current, &workgeom);
    workarea->x_min = workgeom.x;
    workarea->y_min = workgeom.y;
    workarea->x_max = workgeom.x + workgeom.width;
    workarea->y_max = workgeom.y + workgeom.height;
#else				/* EWMH */
    /* TODO: should make this better by considering windows on the "DOCK" layer 
       or with the WIN_HINTS_DO_NOT_COVER hint.  */
    workarea->x_min = workarea->y_min = 0;
    workarea->x_max = scr->rootw;
    workarea->y_max = scr->rooth;
#endif				/* EWMH */
}

/** @brief Get the workspace mask for a window.
  * @param scr - screen
  * @param twin - TWM window
  * @param mask - where to store list of masks
  * @param masks - where to store count of masks in list
  */
void
TwmGetWMWorkspaces(ScreenInfo *scr, TwmWindow *twin, long **mask, int *masks)
{
    long *list;

    if ((list = calloc(1, sizeof(long))) == NULL)
	return;
    list[0] = (long) (unsigned) twin->occupation;
    *mask = list;
    *masks = 1;
}

/** @brief Set the workspace mask for a window.
  * @param scr- screen
  * @param twin - TWM window
  * @param mask - list of masks
  * @param masks - count of masks in list
  */
void
TwmSetWMWorkspaces(ScreenInfo *scr, TwmWindow *twin, long *mask, int masks)
{
    unsigned occupation;

    if ((twin = TwmCanChangeWorkspace(twin)) == NULL)
	return;

    if (masks < 1 || mask == NULL)
	return;
    occupation = (unsigned) mask[0];
    occupation &= fullOccupation;
    if (occupation == 0)
	return;
    if (occupation == fullOccupation)
	OccupyAll(twin);
    else
	ChangeOccupation(twin, occupation);
}

/** @brief Change the workspace mask for a window.
  * @param scr - screen
  * @param twin - TWM window
  * @param index - index of mask word
  * @param mask - replacement value for indexed mask
  */
void
TwmChgWMWorkspaces(ScreenInfo *scr, TwmWindow *twin, unsigned index, unsigned mask)
{
    unsigned occupation;

    if ((twin = TwmCanChangeWorkspace(twin)) == NULL)
	return;

    if (index != 0)
	return;
    occupation = mask;
    occupation &= fullOccupation;
    if (occupation == 0)
	return;

    if (occupation == fullOccupation)
	OccupyAll(twin);
    else
	ChangeOccupation(twin, occupation);
}

/** @brief Mark client configuration and moving or not.
  * @param twin - TWM window
  * @param moving - truth value indicating moving or not.
  */
void
TwmSetMoving(TwmWindow *twin, Bool moving)
{
}

/** @brief Get maximized geometry for a client.
  * @parma scr - screen
  * @param twin - TWM window
  * @param maxmimized - where to strore retrieved geometry
  */
void
TwmGetMaximizedGeometry(ScreenInfo *scr, TwmWindow *twin, struct WinGeometry *maximized)
{
}

/** @brief Set maximized geometry for a client.
  * @param scr - screen
  * @param twin - TWM window
  * @param maximized - maximized geometry to set
  */
void
TwmSetMaximizedGeometry(ScreenInfo *scr, TwmWindow *twin, struct WinGeometry *maximized)
{
}

/** @brief Get list of virtual roots.
  * @param scr - screen
  * @param vroots - where to store list of virtual root windows
  * @param count - where to store count of virtual roots in the list
  *
  * From code inspection it does not appear that CTWM uses any virtual roots,
  * except possibly on non-Xinerama multi-head captive configurations (is this
  * really supported?).  This function should normall return zero virtual roots.
  */
void
TwmGetSwmVroots(ScreenInfo *scr, Window **vroots, int *count)
{
    int number = 0;
    Window *windows = NULL;
    VirtualScreen *vs;

    for (vs = scr->vScreenList; vs != NULL; vs = vs->next)
	if (vs->window != None && vs->window != scr->XineramaRoot)
	    number++;
    if ((windows = (Window *) calloc(number + 1, sizeof(Window))) != NULL) {
	int i = 0;

	for (vs = scr->vScreenList; vs != NULL; vs = vs->next)
	    if (vs->window != None && vs->window != scr->XineramaRoot)
		windows[i++] = vs->window;
	windows[i++] = None;
    }
    *vroots = windows;
    *count = windows != NULL ? number : 0;
}

// vim: sw=4 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
