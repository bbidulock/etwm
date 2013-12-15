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

Window
TwmMwmInfo(ScreenInfo *scr)
{
    if (scr->mwmh.info.wm_window != None)
	return scr->mwmh.info.wm_window;
    return scr->ManagerWindow;
}

extern int CanChangeOccupation(TwmWindow **twm_winp);

static TwmWindow *
TwmMwmCanChangeWorkspace(TwmWindow *twin)
{
    if (!CanChangeOccupation(&twin))
	return (NULL);
    return (twin);
}

void
TwmGetMwmInfo(ScreenInfo *scr, MwmInfo *info)
{
    info->flags = MWM_STARTUP_CUSTOM;
    info->wm_window = TwmMwmInfo(scr);
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

/** @brief Initialize the motif hints for a window.
  * @param scr - screen
  * @param twin - TWM window
  * @param hints - the hints to set
  *
  * Called to initialize a newly managed window with an existing _MOTIF_WM_HINTS
  * property.  This will set the functions and decorations bits in the window.
  *
  * The function bits basically decide whether to allow various actions on the
  * window and whether to enable specific titlebar icons.   The bit settings
  * here are basically the same as NET_WM_ACTION_RESIZE, _NET_WM_ACTION_MOVE,
  * _NET_WM_ACTION_MINIMIZE, _NET_WM_ACTION_FULLSCREEN,
  * _NET_WM_ACTION_MAXIMIZE_HORZ, _NET_WM_ACTION_MAXIMIZE_VERT,
  * _NET_WM_ACTION_CLOSE.
  */
void
TwmIniMwmHints(ScreenInfo *scr, TwmWindow *twin, MwmHints *hints)
{
    if (hints->flags & MWM_HINTS_FUNCTIONS) {
	twin->func.function.resize =
	    (hints->functions & (MWM_FUNC_ALL | MWM_FUNC_RESIZE)) ? 1 : 0;
	twin->func.function.move =
	    (hints->functions & (MWM_FUNC_ALL | MWM_FUNC_MOVE)) ? 1 : 0;
	twin->func.function.minimize =
	    (hints->functions & (MWM_FUNC_ALL | MWM_FUNC_MINIMIZE)) ? 1 : 0;
	twin->func.function.maximize_vert = twin->func.function.maximize_horz =
	    twin->func.function.maximize =
	    (hints->functions & (MWM_FUNC_ALL | MWM_FUNC_MAXIMIZE)) ? 1 : 0;
	twin->func.function.close =
	    (hints->functions & (MWM_FUNC_ALL | MWM_FUNC_CLOSE)) ? 1 : 0;
	twin->func.function.shade =
	    (hints->functions & (MWM_FUNC_ALL | MWM_FUNC_SHADE)) ? 1 : 0;
	twin->func.function.stick =
	    (hints->functions & (MWM_FUNC_ALL | MWM_FUNC_STICK)) ? 1 : 0;
	twin->func.function.fullscreen =
	    (hints->functions & (MWM_FUNC_ALL | MWM_FUNC_FULLSCREEN)) ? 1 : 0;
	twin->func.function.above =
	    (hints->functions & (MWM_FUNC_ALL | MWM_FUNC_ABOVE)) ? 1 : 0;
	twin->func.function.below =
	    (hints->functions & (MWM_FUNC_ALL | MWM_FUNC_BELOW)) ? 1 : 0;
	twin->func.function.maximus_left = twin->func.function.maximus_right =
	    twin->func.function.maximus_top = twin->func.function.maximus_bottom =
	    twin->func.function.maximus =
	    (hints->functions & (MWM_FUNC_ALL | MWM_FUNC_MAXIMUS)) ? 1 : 0;
    }
    if (hints->flags & MWM_HINTS_DECORATIONS) {
	twin->decor.decoration.border =
	    (hints->decorations & (MWM_DECOR_ALL | MWM_DECOR_BORDER)) ? 1 : 0;
	twin->decor.decoration.resizeh =
	    (hints->decorations & (MWM_DECOR_ALL | MWM_DECOR_RESIZEH)) ? 1 : 0;
	twin->decor.decoration.title =
	    (hints->decorations & (MWM_DECOR_ALL | MWM_DECOR_TITLE)) ? 1 : 0;
	twin->decor.decoration.menu =
	    (hints->decorations & (MWM_DECOR_ALL | MWM_DECOR_MENU)) ? 1 : 0;
	twin->decor.decoration.minimize =
	    (hints->decorations & (MWM_DECOR_ALL | MWM_DECOR_MINIMIZE)) ? 1 : 0;
	twin->decor.decoration.maximize =
	    (hints->decorations & (MWM_DECOR_ALL | MWM_DECOR_MAXIMIZE)) ? 1 : 0;
	twin->decor.decoration.close =
	    (hints->decorations & (MWM_DECOR_ALL | MWM_DECOR_CLOSE)) ? 1 : 0;
	twin->decor.decoration.resize =
	    (hints->decorations & (MWM_DECOR_ALL | MWM_DECOR_RESIZE)) ? 1 : 0;
	twin->decor.decoration.shade =
	    (hints->decorations & (MWM_DECOR_ALL | MWM_DECOR_SHADE)) ? 1 : 0;
	twin->decor.decoration.stick =
	    (hints->decorations & (MWM_DECOR_ALL | MWM_DECOR_STICK)) ? 1 : 0;
	twin->decor.decoration.maximus =
	    (hints->decorations & (MWM_DECOR_ALL | MWM_DECOR_MAXIMUS)) ? 1 : 0;

    }
    /* FIXME: handle tearoff menus */
}

/** @brief Set the motif hints for a window.
  * @param scr - screen
  * @param twin - TWM window
  * @param hints - the hints to set
  */
void
TwmSetMwmHints(ScreenInfo *scr, TwmWindow *twin, MwmHints *hints)
{
}

/** @brief Initialize the desktop hints for a window.
  * @param twin - TWM window
  * @param hints - the hints to set
  *
  * The function bits basically decide whether we are allowed to change the
  * workspaces that the window occupies or not.  This is basically the same as
  * _NET_WM_ACTION_CHANGE_DESKTOP.
  */
void
TwmIniDtWmHints(TwmWindow *twin, DtWmHints *hints)
{
    if (hints->flags & DTWM_HINTS_FUNCTIONS) {
	twin->func.function.change_desktop =
	    (hints->functions & (DTWM_FUNCTION_ALL | DTWM_FUNCTION_OCCUPY_WS)) ? 1 : 0;
    }
    /* TODO: handle behavior bits */
}

/** @brief Set the desktop hints for a window.
  * @param twin - TWM window
  * @param hints - the hints to set
  */
void
TwmSetDtWmHints(TwmWindow *twin, DtWmHints *hints)
{
    /* FIXME */
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

/** @brief Initialize the workspaces for a window.
  * @param scr - screen
  * @param twin - TWM window
  * @param wshints - DTWM workspace hints
  *
  * This function is called when initially managing a top-level window and it
  * contains a _DT_WORKSPACE_HINTS property.  Note that this only sets the
  * twin->occupation variable and does not actually change the workspace.
  */
void
TwmIniWorkspaceHints(ScreenInfo *scr, TwmWindow *twin, DtWmWorkspaceHints *wshints)
{
    WorkSpace *ws;
    long n;

    if (wshints->flags & DT_WORKSPACE_HINTS_WSFLAGS)
	if (wshints->wsflags & DT_WORKSPACE_FLAGS_OCCUPY_ALL)
	    twin->occupation |= fullOccupation;

    if (wshints->flags & DT_WORKSPACE_HINTS_WORKSPACES)
	for (n = 0; n < wshints->numWorkspaces; n++)
	    for (ws = scr->workSpaceMgr.workSpaceList; ws != NULL; ws = ws->next)
		if (ws->atom == wshints->workspaces[n])
		    twin->occupation |= (1 << ws->number);
}

/** @brief Set the workspaces for a window.
  * @param scr - screen
  * @param twin - TWM window
  * @param wshints - DTWM workspace hints
  *
  * This function is called whenever the _DT_WORKSPACE_HINTS property is updated
  * on a managed top-level window.  This changes the occupation of the window.
  */
void
TwmSetWorkspaceHints(ScreenInfo *scr, TwmWindow *twin, DtWmWorkspaceHints *wshints)
{
    WorkSpace *ws;
    long n;
    unsigned occupation = 0;

    if ((twin = TwmMwmCanChangeWorkspace(twin)) == NULL)
	return;

    if (wshints->flags & DT_WORKSPACE_HINTS_WSFLAGS)
	if (wshints->wsflags & DT_WORKSPACE_FLAGS_OCCUPY_ALL)
	    occupation |= fullOccupation;

    if (wshints->flags & DT_WORKSPACE_HINTS_WORKSPACES)
	for (n = 0; n < wshints->numWorkspaces; n++)
	    for (ws = scr->workSpaceMgr.workSpaceList; ws != NULL; ws = ws->next)
		if (ws->atom == wshints->workspaces[n])
		    occupation |= (1 << ws->number);

    if (occupation == fullOccupation)
	OccupyAll(twin);
    else
	ChangeOccupation(twin, occupation);
}

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
    if ((vs = scr->vScreenList) == NULL) {
	fprintf(stderr, "ERROR: First virtual screen is NULL!\n");
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
