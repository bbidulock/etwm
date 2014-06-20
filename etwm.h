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

Bool	EtwmIsRunning			(Display *display, int scrnum);
char	**EtwmListWorkspaces		(Display *display, int scrnum);
char	*EtwmCurrentWorkspace		(Display *display, int scrnum);
int	EtwmChangeWorkspace		(Display *display, int scrnum,
					 char	*workspace);
char	**EtwmCurrentOccupation		(Display *display, Window window);
int	EtwmSetOccupation		(Display *display, Window window,
					 char **occupation);
int	EtwmAddToCurrentWorkspace	(Display *display, Window window);

#ifdef USEM4
extern int KeepTmpFile;			/* JMO 3/28/90 for m4 */
extern char *keepM4_filename;		/* Keep M4 output here */
extern int GoThroughM4;
#endif
char *captivename;

extern Bool HasXinerama;
extern Bool HasXrandr;

