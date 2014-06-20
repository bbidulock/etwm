/*****************************************************************************/
/**       Copyright 1988 by Evans & Sutherland Computer Corporation,        **/
/**                          Salt Lake City, Utah                           **/
/**  Portions Copyright 1989 by the Massachusetts Institute of Technology   **/
/**                        Cambridge, Massachusetts                         **/
/**                                                                         **/
/**                           All Rights Reserved                           **/
/**                                                                         **/
/**    Permission to use, copy, modify, and distribute this software and    **/
/**    its documentation  for  any  purpose  and  without  fee is hereby    **/
/**    granted, provided that the above copyright notice appear  in  all    **/
/**    copies and that both  that  copyright  notice  and  this  permis-    **/
/**    sion  notice appear in supporting  documentation,  and  that  the    **/
/**    names of Evans & Sutherland and M.I.T. not be used in advertising    **/
/**    in publicity pertaining to distribution of the  software  without    **/
/**    specific, written prior permission.                                  **/
/**                                                                         **/
/**    EVANS & SUTHERLAND AND M.I.T. DISCLAIM ALL WARRANTIES WITH REGARD    **/
/**    TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES  OF  MERCHANT-    **/
/**    ABILITY  AND  FITNESS,  IN  NO  EVENT SHALL EVANS & SUTHERLAND OR    **/
/**    M.I.T. BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL  DAM-    **/
/**    AGES OR  ANY DAMAGES WHATSOEVER  RESULTING FROM LOSS OF USE, DATA    **/
/**    OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER    **/
/**    TORTIOUS ACTION, ARISING OUT OF OR IN  CONNECTION  WITH  THE  USE    **/
/**    OR PERFORMANCE OF THIS SOFTWARE.                                     **/
/*****************************************************************************/
/* 
 *  [ etwm ]
 *
 *  Copyright 1992 Claude Lecommandeur.
 *            
 * Permission to use, copy, modify  and distribute this software  [etwm] and
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


/***********************************************************************
 *
 * $XConsortium: util.h,v 1.10 89/12/10 17:47:04 jim Exp $
 *
 * utility routines header file
 *
 * 28-Oct-87 Thomas E. LaStrange		File created
 *
 ***********************************************************************/

#include <X11/Intrinsic.h>
#include <X11/Xproto.h>
#include "types.h"

#ifndef _UTIL_
#define _UTIL_

#if defined(VMS) || defined(ultrix) || defined(sequent) || defined(sony_news) || defined(riscix)
#   define strdup(s) ((char*) strcpy ((char*) malloc (strlen (s) + 1), s))
#endif

#ifndef MAX
#define MAX(x,y) ((x)>(y)?(x):(y))
#endif
#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif
#ifndef ABS
#define ABS(x) ((x)<0?-(x):(x))
#endif

extern int Animating;
extern Bool AnimationActive;
extern Bool MaybeAnimate;
extern int AnimationSpeed;
#ifdef USE_SIGNALS
extern Bool AnimationPending;
#else
extern struct timeval AnimateTimeout;
#endif /* USE_SIGNALS */

extern void	Zoom(Window wf, Window wt);
extern void	ZoomTo(Window wf, TwmWindow *twm_win);
extern void	ZoomFrom(TwmWindow *twm_win, Window wt);
extern void	ZoomIconToFrame(TwmWindow *ftwm, TwmWindow *ttwm);
extern void	ZoomFrameToIcon(TwmWindow *ftwm, TwmWindow *ttwm);
extern void	ZoomIconToIcon(TwmWindow *ftwm, TwmWindow *ttwm);
extern void	MoveOutline(Window root,
			    int x, int y, int width, int height,
			    int bw, int th);
extern Pixmap	GetBitmap(char *name);
extern Pixmap	FindBitmap(char *name,
			   unsigned int *widthp, unsigned int *heightp);
extern void	GetUnknownIcon(char *name);
extern char	*ExpandFilename(char *name);
extern char	*ExpandFilePath(char *path);
extern char	*ExpandPixmapPath (char *name);

void MaskScreen (char *file);
void UnmaskScreen (void);
void StartAnimation (void);
void StopAnimation (void);
void SetAnimationSpeed (int speed);
void ModifyAnimationSpeed (int incr);
void Animate (void);
void TryToAnimate (void);
void InsertRGBColormap (Atom a, XStandardColormap *maps, int nmaps,
			Bool replace);
void RemoveRGBColormap (Atom a);
void LocateStandardColormaps (void);
void GetColor (int kind, Pixel *what, char *name);
void GetShadeColors (ColorPair *cp);
void GetFont(MyFont *font);
Bool UpdateFont(MyFont *font, int height);
void SetFocusVisualAttributes (TwmWindow *tmp_win, Bool focus);
void move_to_after (TwmWindow *t, TwmWindow *after);
void SetFocus (TwmWindow *tmp_win, Time tim);
Pixmap CreateMenuIcon(int height, unsigned int *widthp, unsigned int *heightp);
Pixmap Create3DMenuIcon (unsigned int height,
			 unsigned int *widthp, unsigned int *heightp,
			 ColorPair cp);
Pixmap Create3DIconManagerIcon (ColorPair cp);
void Draw3DBorder (Window w,
		   int x, int y, int width, int height, int bw,
		   ColorPair cp,
		   int state, int fill, int forcebw);
void Draw3DCorner (Window w,
		   int x, int y, int width, int height, int thick, int bw,
		   ColorPair cp,
		   int type);
void PaintBorders (TwmWindow *tmp_win, Bool focus);
void PaintTitle (TwmWindow *tmp_win);
void PaintIcon (TwmWindow *tmp_win);
void PaintTitleButton (TwmWindow *tmp_win, TBWindow  *tbw);
void AdoptWindow (void);
void DebugTrace (char *file);
void SetBorderCursor (TwmWindow *tmp_win, int x, int y);

extern int HotX, HotY;

struct Image {
    Pixmap pixmap;
    Pixmap mask;
    int    width;
    int    height;
    Image *next;
};

extern Image *GetImage (char *name, ColorPair cp);

extern unsigned char *GetWMPropertyString(Window w, Atom prop);
extern void FreeWMPropertyString(char *prop);
extern void ConstrainByBorders1 (int *left, int width, int *top, int height);
extern void ConstrainByBorders (TwmWindow *twmwin,
				int *left, int width,
				int *top, int height);

#endif /* _UTIL_ */
