/****************************************************************************
 * Copyright (c) 2009,2010 Free Software Foundation, Inc.                   *
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
/*
 * $Id: test_addchstr.c,v 1.6 2010/05/01 19:13:46 tom Exp $
 *
 * Demonstrate the waddchstr() and waddch functions.
 * Thomas Dickey - 2009/9/12
 */

#include <test.priv.h>

#include <linedata.h>

#undef MvAddStr
#undef MvWAddStr

#define AddNStr    addchnstr
#define AddStr     addchstr
#define MvAddNStr  (void) mvaddchnstr
#define MvAddStr   (void) mvaddchstr
#define MvWAddNStr (void) mvwaddchnstr
#define MvWAddStr  (void) mvwaddchstr
#define WAddNStr   waddchnstr
#define WAddStr    waddchstr

#define AddCh      addch
#define WAddCh     waddch

#define MY_TABSIZE 8

typedef enum {
    oDefault = 0,
    oMove = 1,
    oWindow = 2,
    oMoveWindow = 3
} Options;

static bool m_opt = FALSE;
static bool w_opt = FALSE;
static int n_opt = -1;

static chtype *temp_buffer;
static size_t temp_length;

#define TempBuffer(source_cast)

static chtype *
ChStr(const char *source)
{
    if (source != 0) {
	size_t need = strlen(source) + 1;
	wchar_t have[2];
	int n = 0;

	if (need > temp_length) {
	    temp_length = need * 2;
	    temp_buffer = typeRealloc(chtype, temp_length, temp_buffer);
	}
	have[0] = 0;
	have[1] = 0;
	do {
	    temp_buffer[n++] = UChar(*source++);
	} while (have[0] != 0);
    } else if (temp_buffer != 0) {
	free(temp_buffer);
	temp_buffer = 0;
	temp_length = 0;
    }
    return temp_buffer;
}

static void
legend(WINDOW *win, int level, Options state, char *buffer, int length)
{
    NCURSES_CONST char *showstate;

    switch (state) {
    default:
    case oDefault:
	showstate = "";
	break;
    case oMove:
	showstate = " (mvXXX)";
	break;
    case oWindow:
	showstate = " (winXXX)";
	break;
    case oMoveWindow:
	showstate = " (mvwinXXX)";
	break;
    }

    wmove(win, 0, 0);
    wprintw(win,
	    "The Strings/Chars displays should match.  Enter any characters, except:\n");
    wprintw(win,
	    "down-arrow or ^N to repeat on next line, 'w' for inner window, 'q' to exit.\n");
    wclrtoeol(win);
    wprintw(win, "Level %d,%s added %d characters <%s>", level,
	    showstate, length, buffer);
}

static int
ColOf(char *buffer, int length, int margin)
{
    int n;
    int result;

    for (n = 0, result = margin + 1; n < length; ++n) {
	int ch = UChar(buffer[n]);
	switch (ch) {
	case '\n':
	    /* actually newline should clear the remainder of the line
	     * and move to the next line - but that seems a little awkward
	     * in this example.
	     */
	case '\r':
	    result = 0;
	    break;
	case '\b':
	    if (result > 0)
		--result;
	    break;
	case '\t':
	    result += (MY_TABSIZE - (result % MY_TABSIZE));
	    break;
	case '\177':
	    result += 2;
	    break;
	default:
	    ++result;
	    if (ch < 32)
		++result;
	    break;
	}
    }
    return result;
}

#define LEN(n) ((length - (n) > n_opt) ? n_opt : (length - (n)))
static void
test_adds(int level)
{
    static bool first = TRUE;

    int ch;
    int limit;
    int row = 1;
    int col;
    int row2, col2;
    int length;
    char buffer[BUFSIZ];
    WINDOW *look = 0;
    WINDOW *work = 0;
    WINDOW *show = 0;
    int margin = (2 * MY_TABSIZE) - 1;
    Options option = (Options) ((unsigned) (m_opt
					    ? oMove
					    : oDefault)
				| (unsigned) ((w_opt || (level > 0))
					      ? oWindow
					      : oDefault));

    if (first) {
	static char cmd[80];
	setlocale(LC_ALL, "");

	putenv(strcpy(cmd, "TABSIZE=8"));

	initscr();
	(void) cbreak();	/* take input chars one at a time, no wait for \n */
	(void) noecho();	/* don't echo input */
	keypad(stdscr, TRUE);
    }

    limit = LINES - 5;
    if (level > 0) {
	look = newwin(limit, COLS - (2 * (level - 1)), 0, level - 1);
	work = newwin(limit - 2, COLS - (2 * level), 1, level);
	show = newwin(4, COLS, limit + 1, 0);
	box(look, 0, 0);
	wnoutrefresh(look);
	limit -= 2;
    } else {
	work = stdscr;
	show = derwin(stdscr, 4, COLS, limit + 1, 0);
    }
    keypad(work, TRUE);

    for (col = margin + 1; col < COLS; col += MY_TABSIZE)
	MvWVLine(work, row, col, '.', limit - 2);

    MvWVLine(work, row, margin, ACS_VLINE, limit - 2);
    MvWVLine(work, row, margin + 1, ACS_VLINE, limit - 2);
    limit /= 2;

    MvWAddChStr(work, 1, 2, ChStr("String"));
    MvWAddChStr(work, limit + 1, 2, ChStr("Chars"));
    wnoutrefresh(work);

    buffer[length = 0] = '\0';
    legend(show, level, option, buffer, length);
    wnoutrefresh(show);

    doupdate();

    /*
     * Show the characters added in color, to distinguish from those that
     * are shifted.
     */
    if (has_colors()) {
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_BLUE);
	wbkgdset(work, COLOR_PAIR(1) | ' ');
    }

    while ((ch = read_linedata(work)) != ERR && !isQUIT(ch)) {
	wmove(work, row, margin + 1);
	switch (ch) {
	case key_RECUR:
	    test_adds(level + 1);

	    touchwin(look);
	    touchwin(work);
	    touchwin(show);

	    wnoutrefresh(look);
	    wnoutrefresh(work);
	    wnoutrefresh(show);

	    doupdate();
	    break;
	case key_NEWLINE:
	    if (row < limit) {
		++row;
		/* put the whole string in, all at once */
		col2 = margin + 1;
		switch (option) {
		case oDefault:
		    if (n_opt > 1) {
			for (col = 0; col < length; col += n_opt) {
			    col2 = ColOf(buffer, col, margin);
			    if (move(row, col2) != ERR) {
				AddNStr(ChStr(buffer + col), LEN(col));
			    }
			}
		    } else {
			if (move(row, col2) != ERR) {
			    AddStr(ChStr(buffer));
			}
		    }
		    break;
		case oMove:
		    if (n_opt > 1) {
			for (col = 0; col < length; col += n_opt) {
			    col2 = ColOf(buffer, col, margin);
			    MvAddNStr(row, col2, ChStr(buffer + col), LEN(col));
			}
		    } else {
			MvAddStr(row, col2, ChStr(buffer));
		    }
		    break;
		case oWindow:
		    if (n_opt > 1) {
			for (col = 0; col < length; col += n_opt) {
			    col2 = ColOf(buffer, col, margin);
			    if (wmove(work, row, col2) != ERR) {
				WAddNStr(work, ChStr(buffer + col), LEN(col));
			    }
			}
		    } else {
			if (wmove(work, row, col2) != ERR) {
			    WAddStr(work, ChStr(buffer));
			}
		    }
		    break;
		case oMoveWindow:
		    if (n_opt > 1) {
			for (col = 0; col < length; col += n_opt) {
			    col2 = ColOf(buffer, col, margin);
			    MvWAddNStr(work, row, col2, ChStr(buffer + col),
				       LEN(col));
			}
		    } else {
			MvWAddStr(work, row, col2, ChStr(buffer));
		    }
		    break;
		}

		/* do the corresponding single-character add */
		row2 = limit + row;
		for (col = 0; col < length; ++col) {
		    col2 = ColOf(buffer, col, margin);
		    switch (option) {
		    case oDefault:
			if (move(row2, col2) != ERR) {
			    AddCh(UChar(buffer[col]));
			}
			break;
		    case oMove:
			MvAddCh(row2, col2, UChar(buffer[col]));
			break;
		    case oWindow:
			if (wmove(work, row2, col2) != ERR) {
			    WAddCh(work, UChar(buffer[col]));
			}
			break;
		    case oMoveWindow:
			MvWAddCh(work, row2, col2, UChar(buffer[col]));
			break;
		    }
		}
	    } else {
		beep();
	    }
	    break;
	case KEY_BACKSPACE:
	    ch = '\b';
	    /* FALLTHRU */
	default:
	    if (ch <= 0 || ch > 255) {
		beep();
		break;
	    }
	    buffer[length++] = (char) ch;
	    buffer[length] = '\0';

	    /* put the string in, one character at a time */
	    col = ColOf(buffer, length - 1, margin);
	    switch (option) {
	    case oDefault:
		if (move(row, col) != ERR) {
		    AddStr(ChStr(buffer + length - 1));
		}
		break;
	    case oMove:
		MvAddStr(row, col, ChStr(buffer + length - 1));
		break;
	    case oWindow:
		if (wmove(work, row, col) != ERR) {
		    WAddStr(work, ChStr(buffer + length - 1));
		}
		break;
	    case oMoveWindow:
		MvWAddStr(work, row, col, ChStr(buffer + length - 1));
		break;
	    }

	    /* do the corresponding single-character add */
	    switch (option) {
	    case oDefault:
		if (move(limit + row, col) != ERR) {
		    AddCh(UChar(ch));
		}
		break;
	    case oMove:
		MvAddCh(limit + row, col, UChar(ch));
		break;
	    case oWindow:
		if (wmove(work, limit + row, col) != ERR) {
		    WAddCh(work, UChar(ch));
		}
		break;
	    case oMoveWindow:
		MvWAddCh(work, limit + row, col, UChar(ch));
		break;
	    }

	    wnoutrefresh(work);

	    legend(show, level, option, buffer, length);
	    wnoutrefresh(show);

	    doupdate();
	    break;
	}
    }
    if (level > 0) {
	delwin(show);
	delwin(work);
	delwin(look);
    }
}

static void
usage(void)
{
    static const char *tbl[] =
    {
	"Usage: test_addchstr [options]"
	,""
	,"Options:"
	,"  -f FILE read data from given file"
	,"  -n NUM  limit string-adds to NUM bytes on ^N replay"
	,"  -m      perform wmove/move separately from add-functions"
	,"  -w      use window-parameter even when stdscr would be implied"
    };
    unsigned n;
    for (n = 0; n < SIZEOF(tbl); ++n)
	fprintf(stderr, "%s\n", tbl[n]);
    ExitProgram(EXIT_FAILURE);
}

int
main(int argc GCC_UNUSED, char *argv[]GCC_UNUSED)
{
    int ch;

    setlocale(LC_ALL, "");

    while ((ch = getopt(argc, argv, "f:mn:w")) != -1) {
	switch (ch) {
	case 'f':
	    init_linedata(optarg);
	    break;
	case 'm':
	    m_opt = TRUE;
	    break;
	case 'n':
	    n_opt = atoi(optarg);
	    if (n_opt == 0)
		n_opt = -1;
	    break;
	case 'w':
	    w_opt = TRUE;
	    break;
	default:
	    usage();
	    break;
	}
    }
    if (optind < argc)
	usage();

    test_adds(0);
    endwin();
    ExitProgram(EXIT_SUCCESS);
}