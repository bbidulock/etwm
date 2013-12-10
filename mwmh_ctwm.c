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

/** @brief Set the motif hints for a window.
  * @param scr - screen
  * @param twin - TWM window
  * @param hints - the hints to set
  *
  * Note that this function is called whether the property exists on the client
  * window or not.  The hints structure will be set to all zeros when the
  * property does not exits.
  *
  * The values return here wind up being set against twin->mwmh.hints regardless
  * of whether the property exists on the window or not.  The function bits
  * basically decide whether to allow various actions on the window and whether
  * to enable specific titlebar icons.   The bit settings here are basically the
  * same as NET_WM_ACTION_RESIZE, _NET_WM_ACTION_MOVE, _NET_WM_ACTION_MINIMIZE,
  * _NET_WM_ACTION_FULLSCREEN, _NET_WM_ACTION_MAXIMIZE_HORZ,
  * _NET_WM_ACTION_MAXIMIZE_VERT, _NET_WM_ACTION_CLOSE.
  */
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

/** @brief Set the desktop hints for a window.
  * @param twin - TWM window
  * @param hints - the hints to set
  *
  * Note that this function is called whether the property exists on the client
  * window or not.  The hints structure will be set to all zeroes when the
  * property does not exist.
  *
  * The values returned here wind up being set agaist twin->mwmh.dthints
  * regardless of whether the property exists on the window or not.  The
  * function bits basically decide whether we are allowed to change the
  * workspaces that the window occupies or not.  This is basically the same as
  * _NET_WM_ACTION_CHANGE_DESKTOP.
  */
void
TwmSetDtWmHints(TwmWindow *twin, struct DtWmHints *hints)
{
    /* munge this a bit */
    if (!(hints->flags & DTWM_HINTS_FUNCTIONS)) {
	hints->flags |= DTWM_HINTS_FUNCTIONS;
	hints->functions = (DTWM_FUNCTION_ALL|DTWM_FUNCTION_OCCUPY_WS);
    }
    if(!(hints->flags & DTWM_HINTS_BEHAVIORS)) {
	hints->flags |= DTWM_HINTS_BEHAVIORS;
	hints->behaviors = 0;
    }
    if (!(hints->flags & DTWM_HINTS_ATTACH_WINDOW))
	hints->attachWindow = None;
}

void
TwmGetMwmOffset(ScreenInfo *scr, TwmWindow *twin, long *gravity, long *bw, long *x_off,
		long *y_off)
{
    *gravity = NorthWestGravity;
    *bw = twin->frame_bw + twin->frame_bw3D;
    *x_off = *bw;
    *y_off = *bw + twin->title_height;
}

static Atom
workspace_atom(WorkSpace *ws)
{
    if (ws == NULL)
	return XInternAtom(dpy, "workspace", False);
    if (ws->atom != None)
	return ws->atom;
    if (ws->label != NULL)
	return XInternAtom(dpy, ws->label, False);
    if (ws->name != NULL)
	return XInternAtom(dpy, ws->name, False);
    return XInternAtom(dpy, "workspace", False);
}

extern int fullOccupation;

/** @brief Get the presence workspace atoms for a window.
  * @param scr - screen
  * @param twin - TWM window
  * @param presence - where to store the workspace atom list
  * @param count - where to store the count of atoms in the list
  *
  * This function is used to obtain the list of workspace atoms representing the
  * workspaces on which a window is present.  This is use when setting the
  * _DT_WORKSPACE_PRESENCE property for a client window.
  */
void
TwmGetWorkspacePresence(ScreenInfo *scr, TwmWindow *twin, Atom **presence, int *count)
{
    WorkSpace *ws = NULL;
    unsigned long mask;
    int number = 0, i, n = 0;
    Atom *list = NULL;

    mask = twin->occupation & fullOccupation;
    for (i = 0; i < 32; i++)
	if (mask & (1 << i))
	    n++;
    if ((list = calloc(n + 1, sizeof(Atom))) != NULL) {
	list[number] = None;
	for (ws = scr->workSpaceMgr.workSpaceList; ws != NULL; ws = ws->next) {
	    if (mask & (1 << ws->number)) {
		list[number++] = workspace_atom(ws);
		list[number] = None;
	    }
	}
    }
    *presence = list;
    *count = list != NULL ? number : 0;
}

/** @brief Get the current workspace atom for screen.
  * @param scr - screen
  * @param current - where to store the current workspace atom
  *
  * This function is used to obtain the current workspace atom when setting the
  * _DT_WORKSPACE_CURRENT property on the MWM info window.
  */
void
TwmGetWorkspaceCurrent(ScreenInfo *scr, Atom *current)
{
    VirtualScreen *vs;
    WorkSpace *ws = NULL;

    if (!scr->workSpaceManagerActive) {
	*current = workspace_atom(ws);
	return;
    }
    if (scr == NULL) {
	fprintf(stderr, "ERROR: Screen is NULL!\n");
	*current = None;
	return;
    }
    if ((vs = scr->currentvs) == NULL && (vs = scr->vScreenList) == NULL) {
	fprintf(stderr, "ERROR: Current virtual screen is NULL!\n");
	*current = None;
	return;
    }
    if (vs->wsw == NULL) {
	fprintf(stderr, "ERROR: Workspace window is NULL!\n");
	*current = None;
	return;
    }
    if ((ws = vs->wsw->currentwspc) == NULL) {
	fprintf(stderr, "ERROR: Current workspace is NULL!\n");
	*current = None;
	return;
    }

    *current = workspace_atom(ws);
}

/** @brief Get the list of workswpace atoms for screen.
  * @param scr - screen
  * @param list - where to store the list of workspace atoms
  *
  * This function is used to obtain the list of workspace atoms for the
  * specified sceen, use when setting the _DT_WORKSPACE_LIST property on the MWM
  * info window.
  */
void
TwmGetWorkspaceList(ScreenInfo *scr, Atom **list, int *count)
{
    WorkSpace *ws = NULL;
    int number = 0, n = 0;
    Atom *atoms = NULL;
    char *name;

    /* There are no desktop atoms unless the workspace manager is active.  CTWM
       reverse to TWM behaviour otherwise. */
    if (!scr->workSpaceManagerActive) {
	n = 1;
	if ((atoms = calloc(n + 1, sizeof(Atom))) != NULL) {
	    atoms[number++] = workspace_atom(ws);
	    atoms[number] = None;
	}
    } else {
	for (ws = scr->workSpaceMgr.workSpaceList; ws != NULL; ws = ws->next)
	    n++;
	if ((atoms = calloc(n + 1, sizeof(Atom))) != NULL) {
	    for (ws = scr->workSpaceMgr.workSpaceList; ws != NULL; ws = ws->next) {
		atoms[number++] = workspace_atom(ws);
		atoms[number] = None;
	    }
	}
    }
    *list = atoms;
    *count = list == NULL ? 0 : number;
}

// vim: sw=4 tw=80 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
