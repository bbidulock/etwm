/* 
 *  [ ctwm ]
 *
 *  Copyright 1992 Claude Lecommandeur.
 *            
 * Permission to use, copy, modify  and distribute this software  [ctwm] and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above  copyright notice appear  in all copies and that both that
 * copyright notice and this permission notice appear in supporting documen-
 * tation, and that the name of  Claude Lecommandeur not be used in adverti-
 * sing or  publicity  pertaining to  distribution of  the software  without
 * specific, written prior permission. Claude Lecommandeur make no represen-
 * tations  about the suitability  of this software  for any purpose.  It is
 * provided "as is" without express or implied warranty.
 *
 * Claude Lecommandeur DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL  IMPLIED WARRANTIES OF  MERCHANTABILITY AND FITNESS.  IN NO
 * EVENT SHALL  Claude Lecommandeur  BE LIABLE FOR ANY SPECIAL,  INDIRECT OR
 * CONSEQUENTIAL  DAMAGES OR ANY  DAMAGES WHATSOEVER  RESULTING FROM LOSS OF
 * USE, DATA  OR PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR
 * OTHER  TORTIOUS ACTION,  ARISING OUT OF OR IN  CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Claude Lecommandeur [ lecom@sic.epfl.ch ][ April 1992 ]
 */

#include <X11/Xatom.h>
#include <stdio.h>
#include "twm.h"
#include "cursor.h"
#include "screen.h"

extern Bool HasXrandr;
extern Bool HasXinerama;

extern void twmrc_error_prefix(void);

void InitVirtualScreens (ScreenInfo *scr) {
  Cursor cursor;
  unsigned long valuemask, attrmask;
  XSetWindowAttributes attributes;
  name_list *nptr;
  Atom _XA_WM_VIRTUALROOT = XInternAtom (dpy, "WM_VIRTUALROOT", False);
  Atom _XA__SWM_VROOT = XInternAtom(dpy, "__SWM_VROOT", False);
  Bool userealroot = True;
  int n;

  NewFontCursor (&cursor, "X_cursor");

  attrmask  = ColormapChangeMask | EnterWindowMask | PropertyChangeMask | 
              SubstructureRedirectMask | KeyPressMask | ButtonPressMask |
              ButtonReleaseMask;

  valuemask = CWBackingStore | CWSaveUnder | CWBackPixel | CWOverrideRedirect |
              CWEventMask | CWCursor;
  attributes.backing_store     = NotUseful;
  attributes.save_under	       = False;
  attributes.override_redirect = True;
  attributes.event_mask	       = attrmask;
  attributes.cursor    	       = cursor;
  attributes.background_pixel  = Scr->Black;

  scr->vScreenList = NULL;

  /* If we have, prefferably XRANDR, or alternately Xinerama, information we
     use that instead of VirtualScreens from the configuration file. */

#if 0
#ifdef USE_XRANDR
  if (HasXrandr && scr->crtcs > 1) {
    fprintf(stderr, "Creating %d virtual screens from XRANDR\n", scr->crtcs);
    fflush(stderr);
    for (n = 0; n < scr->crtcs; n++) {
      XRRCrtcInfo *crtc;
      VirtualScreen *vs;
      long data;

      if ((crtc = scr->crtc[n]) == NULL)
	continue;
      vs = calloc (1, sizeof(*vs));
      vs->monitor = n;
      vs->x = crtc->x;
      vs->y = crtc->y;
      vs->w = crtc->width;
      vs->h = crtc->height;
      fprintf(stderr, "Creating vitual screen %d : %dx%d+%d+%d\n", vs->monitor, vs->w, vs->h, vs->x, vs->y);
      vs->window = XCreateWindow (dpy, scr->Root, vs->x, vs->y, vs->w, vs->h, 0,
				  CopyFromParent, (unsigned int)CopyFromParent,
				  (Visual *)CopyFromParent, valuemask, &attributes);
      vs->wsw = NULL;
      XSync (dpy, False);
      XMapWindow (dpy, vs->window);
      XChangeProperty (dpy, vs->window, _XA_WM_VIRTUALROOT, XA_STRING, 8,
		       PropModeReplace, (unsigned char *) "Yes", 4);
      data = vs->window;
      XChangeProperty (dpy, vs->window, _XA__SWM_VROOT, XA_WINDOW, 32,
		       PropModeReplace, (unsigned char *) &data, 1);
      vs->next = scr->vScreenList;
      scr->vScreenList = vs;
    }
    return;
  }
#endif				/* USE_XRANDR */
#ifdef USE_XINERAMA
  if (HasXinerama && scr->heads > 1) {
    fprintf(stderr, "Creating %d virtual screens from Xinerama\n", scr->heads);
    fflush(stderr);
    for (n = 0; n < scr->heads; n++) {
      XineramaScreenInfo *head;
      VirtualScreen *vs;
      long data;

      head = &scr->head[n];
      vs = calloc (1, sizeof(*vs));
      vs->monitor = head->screen_number;
      vs->x = head->x_org;
      vs->y = head->y_org;
      vs->w = head->width;
      vs->h = head->height;
      fprintf(stderr, "Creating vitual screen %d : %dx%d+%d+%d\n", vs->monitor, vs->w, vs->h, vs->x, vs->y);
      vs->window = XCreateWindow (dpy, scr->Root, vs->x, vs->y, vs->w, vs->h, 0,
				  CopyFromParent, (unsigned int)CopyFromParent,
				  (Visual *)CopyFromParent, valuemask, &attributes);
      vs->wsw = NULL;
      XSync (dpy, False);
      XMapWindow (dpy, vs->window);
      XChangeProperty (dpy, vs->window, _XA_WM_VIRTUALROOT, XA_STRING, 8,
		       PropModeReplace, (unsigned char *) "Yes", 4);
      data = vs->window;
      XChangeProperty (dpy, vs->window, _XA__SWM_VROOT, XA_WINDOW, 32,
		       PropModeReplace, (unsigned char *) &data, 1);
      vs->next = scr->vScreenList;
      scr->vScreenList = vs;
    }
    return;
  }
#endif				/* USE_XINERAMA */
#endif

  if (scr->VirtualScreens == NULL) {
    if (userealroot) {
      VirtualScreen *vs = (VirtualScreen*) malloc (sizeof (VirtualScreen));

      vs->x      = 0;
      vs->y      = 0;
      vs->w      = scr->rootw;
      vs->h      = scr->rooth;
      vs->window = scr->Root;
      vs->next   = NULL;
      vs->wsw	 = 0;
      scr->vScreenList = vs;
      scr->currentvs   = vs;
      return;
    } else {
      scr->VirtualScreens = (name_list*) malloc (sizeof (name_list));
      scr->VirtualScreens->next = NULL;
      scr->VirtualScreens->name = (char*) malloc (64);
      sprintf (scr->VirtualScreens->name, "%dx%d+0+0", scr->rootw, scr->rooth);
    }
  }

  for (n = 0, nptr = Scr->VirtualScreens; nptr != NULL; nptr = nptr->next) {
    VirtualScreen *vs;
    char *geometry = (char*) nptr->name;
    int x = 0, y = 0;
    unsigned int w = 0, h = 0;
    long data;

    XParseGeometry (geometry, &x, &y, &w, &h);

    if ((x < 0) || (y < 0) || (w > scr->rootw) || (h > scr->rooth)) {
      twmrc_error_prefix ();
      fprintf (stderr, "InitVirtualScreens : invalid geometry : %s\n", geometry);
      continue;
    }
    vs = (VirtualScreen*) malloc (sizeof (VirtualScreen));
    vs->monitor = n++;
    vs->x = x;
    vs->y = y;
    vs->w = w;
    vs->h = h;
    vs->window = XCreateWindow (dpy, Scr->Root, x, y, w, h,
			       0, CopyFromParent, (unsigned int) CopyFromParent,
			       (Visual *) CopyFromParent, valuemask, &attributes);
    vs->wsw = 0;

    XSync (dpy, 0);
    XMapWindow (dpy, vs->window);
    XChangeProperty (dpy, vs->window, _XA_WM_VIRTUALROOT, XA_STRING, 8, 
		     PropModeReplace, (unsigned char *) "Yes", 4);
    data = vs->window;
    XChangeProperty (dpy, vs->window, _XA__SWM_VROOT, XA_WINDOW, 32,
		     PropModeReplace, (unsigned char *) &data, 1);

    vs->next = scr->vScreenList;
    scr->vScreenList = vs;
  }

  if (scr->vScreenList == NULL) {
    twmrc_error_prefix ();
    fprintf (stderr, "no valid VirtualScreens found, exiting...\n");
    exit (1);
  }
  /* XXX Should setup scr->{currentvs,Root{,x,y,w,h}} as if the
   * _correct_ virtual screen is entered with the mouse.
   * See HandleEnterNotify().
   */
}

VirtualScreen *findIfVScreenOf (int x, int y)
{
  VirtualScreen *vs;
  for (vs = Scr->vScreenList; vs != NULL; vs = vs->next) {

    if ((x >= vs->x) && ((x - vs->x) < vs->w) &&
	(y >= vs->y) && ((y - vs->y) < vs->h)) {
        return vs;
    }
  }
  return NULL;
}

VirtualScreen *getVScreenOf (int x, int y)
{
  VirtualScreen *vs;
  if ((vs = findIfVScreenOf(x, y)))
	return vs;
  return Scr->vScreenList;
}

/*
 * Returns the order that virtual screens are displayed for the vscreen
 * list.  This is stored this way so everything ends up in the right place
 * on a ctwm restart.
 */
Bool CtwmGetVScreenMap (Display *display, Window rootw,
			char *outbuf, int *outbuf_len)
{
    unsigned char       *prop;
    unsigned long       bytesafter;
    unsigned long       len;
    Atom                actual_type;
    int                 actual_format;

    if (_XA_WM_CTWM_VSCREENMAP == None) return (False);
    if (XGetWindowProperty (display, rootw, _XA_WM_CTWM_VSCREENMAP, 0L, 512, 
                        False, XA_STRING, &actual_type, &actual_format, &len,
                        &bytesafter, &prop) != Success) return (False);
    if (len == 0) return (False);
    *outbuf_len = (len>=*outbuf_len)?*outbuf_len-1:len;
    memcpy(outbuf, prop, *outbuf_len);
    outbuf[*outbuf_len] = '\0';
    return (True); 
}

Bool CtwmSetVScreenMap(Display *display, Window rootw,
		       struct VirtualScreen *firstvs)
{
    char			buf[1024];
    int				tally = 0;
    struct VirtualScreen	*vs;

    if(_XA_WM_CTWM_VSCREENMAP == None) return(False);

    memset(buf, 0, sizeof(buf));
    for (vs = firstvs; vs; vs = vs->next) {
	if (tally) strcat(buf, ",");
	if (vs->wsw && vs->wsw->currentwspc && vs->wsw->currentwspc->name) {
	    strcat(buf, vs->wsw->currentwspc->name);
	    tally++;
	}
    }

    if(! tally) return(False);

    XChangeProperty(display, rootw, _XA_WM_CTWM_VSCREENMAP, XA_STRING, 8,
	PropModeReplace, (unsigned char *)buf, strlen(buf));
    return(True);
}
