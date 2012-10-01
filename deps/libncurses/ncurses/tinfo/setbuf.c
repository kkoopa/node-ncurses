/****************************************************************************
 * Copyright (c) 1998-2011,2012 Free Software Foundation, Inc.              *
 *                                                                          *
 * Permission is hereby granted, free of charge, to any person obtaining a  *
 * copy of this software and associated documentation files (the            *
 * "Software"), to deal in the Software without restriction, including      *
 * without limitation the rights to use, copy, modify, merge, publish,      *
 * distribute, distribute with modifications, sublicense, and/or sell       *
 * copies of the Software, and to permit persons to whom the Software is    *
 * furnished to do so, subject to the following conditions:                 *
 *                                                                          *
 * The above copyright notice and this permission notice shall be included  *
 * in all copies or substantial portions of the Software.                   *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  *
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF               *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   *
 * IN NO EVENT SHALL THE ABOVE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR    *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR    *
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.                               *
 *                                                                          *
 * Except as contained in this notice, the name(s) of the above copyright   *
 * holders shall not be used in advertising or otherwise to promote the     *
 * sale, use or other dealings in this Software without prior written       *
 * authorization.                                                           *
 ****************************************************************************/

/****************************************************************************
 *  Author: Zeyd M. Ben-Halim <zmbenhal@netcom.com> 1992,1995               *
 *     and: Eric S. Raymond <esr@snark.thyrsus.com>                         *
 *     and: Thomas E. Dickey                        1996-on                 *
 *     and: Juergen Pfeifer                         2008                    *
 ****************************************************************************/

/*
**	setbuf.c
**
**	Support for set_term(), reset_shell_mode(), reset_prog_mode().
**
*/

#include <curses.priv.h>

MODULE_ID("$Id: setbuf.c,v 1.19 2012/08/25 20:48:05 tom Exp $")

/*
 * Obsolete entrypoint retained for binary compatbility.
 */
NCURSES_EXPORT(void)
NCURSES_SP_NAME(_nc_set_buffer) (NCURSES_SP_DCLx FILE *ofp, int buffered)
{
#if NCURSES_SP_FUNCS
    (void) SP_PARM;
#endif
    (void) ofp;
    (void) buffered;
}

#if NCURSES_SP_FUNCS
NCURSES_EXPORT(void)
_nc_set_buffer(FILE *ofp, int buffered)
{
    NCURSES_SP_NAME(_nc_set_buffer) (CURRENT_SCREEN, ofp, buffered);
}
#endif
