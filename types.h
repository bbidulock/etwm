/* 
 *  [ etwm ]
 *
 *  Copyright 2004 Richard Levitte.
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
 * Author:  Richard Levitte [ richard@levitte.org ][ June 2004 ]
 */

#ifndef _TYPES_
#define _TYPES_

/* From icons.h */
typedef struct Icon Icon;
typedef struct IconRegion IconRegion;
typedef struct IconEntry IconEntry;

/* From menus.h */
typedef struct MenuItem MenuItem;
typedef struct MenuRoot MenuRoot;
typedef struct MouseButton MouseButton;
typedef struct FuncButton FuncButton;
typedef struct FuncKey FuncKey;

/* From iconmgr.h */
typedef struct WList WList;
typedef struct IconMgr IconMgr;

/* From list.h */
typedef struct name_list name_list;

/* From screen.h */
typedef struct StdCmap StdCmap;
typedef struct TitlebarPixmaps TitlebarPixmaps;
typedef struct ScreenInfo ScreenInfo;

/* from twm.h */
typedef struct MyFont MyFont;
typedef struct ColorPair ColorPair;
typedef struct TitleButtonFunc TitleButtonFunc;
typedef struct TitleButton TitleButton;
typedef struct TBWindow TBWindow;
typedef struct SqueezeInfo SqueezeInfo;
typedef struct TwmColormap TwmColormap;
typedef struct ColormapWindow ColormapWindow;
typedef struct Colormaps Colormaps;
typedef struct WindowRegion WindowRegion;
typedef struct WindowEntry WindowEntry;
typedef struct WindowBox WindowBox;
typedef struct TwmWindow TwmWindow;
typedef struct TWMWinConfigEntry TWMWinConfigEntry;

/* From util.h */
typedef struct Image Image;

/* From vscreen.h */
typedef struct VirtualScreen VirtualScreen;

/* From workmgr.h */
typedef struct winList *WinList;
typedef struct WorkSpaceMgr WorkSpaceMgr;
typedef struct WorkSpace WorkSpace;
typedef struct MapSubwindow MapSubwindow;
typedef struct ButtonSubwindow ButtonSubwindow;
typedef struct WorkSpaceWindow WorkSpaceWindow;
typedef struct OccupyWindow OccupyWindow;
typedef struct CaptiveETWM CaptiveETWM;

#endif /* _TYPES_ */
