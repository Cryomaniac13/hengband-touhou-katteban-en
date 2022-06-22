/* File: main-gcu.c */

/*
 * Copyright (c) 1997 Ben Harrison, and others
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.
 */

/* Purpose: Allow use of Unix "curses" with Angband -BEN- */

/*
 * This file has been modified to use multiple text windows if your screen
 * is larger than 80x25.  By Keldon Jones (keldon@umr.edu).
 */

/*
 * To use this file, you must define "USE_GCU" in the Makefile.
 *
 * Hack -- note that "angband.h" is included AFTER the #ifdef test.
 * This was necessary because of annoying "curses.h" silliness.
 *
 * Note that this file is not "intended" to support non-Unix machines,
 * nor is it intended to support VMS or other bizarre setups.
 *
 * Also, this package assumes that the underlying "curses" handles both
 * the "nonl()" and "cbreak()" commands correctly, see the "OPTION" below.
 *
 * This code should work with most versions of "curses" or "ncurses",
 * and the "main-ncu.c" file (and USE_NCU define) are no longer used.
 *
 * See also "USE_CAP" and "main-cap.c" for code that bypasses "curses"
 * and uses the "termcap" information directly, or even bypasses the
 * "termcap" information and sends direct vt100 escape sequences.
 *
 * XXX XXX XXX Consider the use of "savetty()" and "resetty()".
 */

#include "angband.h"


#ifdef USE_GCU

/*
 * Hack -- play games with "bool"
 */
#undef bool

/*
 * Include the proper "header" file
 */
#ifdef USE_NCURSES
# undef bool
# include <ncurses.h>
#else
# include <curses.h>
#endif

typedef struct term_data term_data;

struct term_data
{
   term t;

   WINDOW *win;
};

#define MAX_TERM_DATA 4

static term_data data[MAX_TERM_DATA];


/*
 * Hack -- try to guess which systems use what commands
 * Hack -- allow one of the "USE_Txxxxx" flags to be pre-set.
 * Mega-Hack -- try to guess when "POSIX" is available.
 * If the user defines two of these, we will probably crash.
 */
#if !defined(USE_TPOSIX)
# if !defined(USE_TERMIO) && !defined(USE_TCHARS)
#  if defined(_POSIX_VERSION)
#   define USE_TPOSIX
#  else
#   if defined(USG) || defined(linux) || defined(SOLARIS)
#    define USE_TERMIO
#   else
#    define USE_TCHARS
#   endif
#  endif
# endif
#endif

/*
 * Hack -- Amiga uses "fake curses" and cannot do any of this stuff
 */
#if defined(AMIGA)
# undef USE_TPOSIX
# undef USE_TERMIO
# undef USE_TCHARS
#endif

/*
 * POSIX stuff
 */
#ifdef USE_TPOSIX
# include <sys/ioctl.h>
# include <termios.h>
#endif

/*
 * One version needs this file
 */
#ifdef USE_TERMIO
# include <sys/ioctl.h>
# include <termio.h>
#endif

/*
 * The other needs this file
 */
#ifdef USE_TCHARS
# include <sys/ioctl.h>
# include <sys/resource.h>
# include <sys/param.h>
# include <sys/file.h>
# include <sys/types.h>
#endif


#include <locale.h>


/*
 * XXX XXX Hack -- POSIX uses "O_NONBLOCK" instead of "O_NDELAY"
 *
 * They should both work due to the "(i != 1)" test below.
 */
#ifndef O_NDELAY
# define O_NDELAY O_NONBLOCK
#endif


/*
 * OPTION: some machines lack "cbreak()"
 * On these machines, we use an older definition
 */
/* #define cbreak() crmode() */


/*
 * OPTION: some machines cannot handle "nonl()" and "nl()"
 * On these machines, we can simply ignore those commands.
 */
/* #define nonl() */
/* #define nl() */


#ifdef USE_SOUND

static cptr ANGBAND_DIR_XTRA_SOUND;

/*
 * Flag set once "sound" has been initialized
 */
static bool can_use_sound = FALSE;

/*
 * An array of sound file names
 */
static cptr sound_file[SOUND_MAX];

#endif /* USE_SOUND */

/*
 * Save the "normal" and "angband" terminal settings
 */

#ifdef USE_TPOSIX

static struct termios  norm_termios;

static struct termios  game_termios;

#endif

#ifdef USE_TERMIO

static struct termio  norm_termio;

static struct termio  game_termio;

#endif

#ifdef USE_TCHARS

static struct ltchars norm_speciax_chars;
static struct sgttyb  norm_ttyb;
static struct tchars  norm_tchars;
static int            norm_locax_chars;

static struct ltchars game_speciax_chars;
static struct sgttyb  game_ttyb;
static struct tchars  game_tchars;
static int            game_locax_chars;

#endif



/*
 * Hack -- Number of initialized "term" structures
 */
static int active = 0;



#ifdef A_COLOR

/*
 * Hack -- define "A_BRIGHT" to be "A_BOLD", because on many
 * machines, "A_BRIGHT" produces ugly "inverse" video.
 */
#ifndef A_BRIGHT
# define A_BRIGHT A_BOLD
#endif

/*
 * Software flag -- we are allowed to use color
 */
static int can_use_color = FALSE;

/*
 * Simple Angband to Curses color conversion table
 */
static int colortable[16];

/*
 * If TRUE, use A_BRIGHT with colors on 88 and 256 color terminals.
 */
static int bold_extended = FALSE;

/*
 * If TRUE, don't change the terminal's color table, even if otherwise
 * allowed.
 */
static int keep_terminal_colors = FALSE;

/*
 * The background color we should draw with.
 */
static int bg_color = COLOR_BLACK;

#define PAIR_WHITE 0
#define PAIR_RED 1
#define PAIR_GREEN 2
#define PAIR_YELLOW 3
#define PAIR_BLUE 4
#define PAIR_MAGENTA 5
#define PAIR_CYAN 6
#define PAIR_BLACK 7

#endif



/*
 * Place the "keymap" into its "normal" state
 */
static void keymap_norm(void)
{

#ifdef USE_TPOSIX

   /* restore the saved values of the special chars */
   (void)tcsetattr(0, TCSAFLUSH, &norm_termios);

#endif

#ifdef USE_TERMIO

   /* restore the saved values of the special chars */
   (void)ioctl(0, TCSETA, (char *)&norm_termio);

#endif

#ifdef USE_TCHARS

   /* restore the saved values of the special chars */
   (void)ioctl(0, TIOCSLTC, (char *)&norm_speciax_chars);
   (void)ioctl(0, TIOCSETP, (char *)&norm_ttyb);
   (void)ioctl(0, TIOCSETC, (char *)&norm_tchars);
   (void)ioctl(0, TIOCLSET, (char *)&norm_locax_chars);

#endif

}


/*
 * Place the "keymap" into the "game" state
 */
static void keymap_game(void)
{

#ifdef USE_TPOSIX

   /* restore the saved values of the special chars */
   (void)tcsetattr(0, TCSAFLUSH, &game_termios);

#endif

#ifdef USE_TERMIO

   /* restore the saved values of the special chars */
   (void)ioctl(0, TCSETA, (char *)&game_termio);

#endif

#ifdef USE_TCHARS

   /* restore the saved values of the special chars */
   (void)ioctl(0, TIOCSLTC, (char *)&game_speciax_chars);
   (void)ioctl(0, TIOCSETP, (char *)&game_ttyb);
   (void)ioctl(0, TIOCSETC, (char *)&game_tchars);
   (void)ioctl(0, TIOCLSET, (char *)&game_locax_chars);

#endif

}


/*
 * Save the normal keymap
 */
static void keymap_norm_prepare(void)
{

#ifdef USE_TPOSIX

   /* Get the normal keymap */
   tcgetattr(0, &norm_termios);

#endif

#ifdef USE_TERMIO

   /* Get the normal keymap */
   (void)ioctl(0, TCGETA, (char *)&norm_termio);

#endif

#ifdef USE_TCHARS

   /* Get the normal keymap */
   (void)ioctl(0, TIOCGETP, (char *)&norm_ttyb);
   (void)ioctl(0, TIOCGLTC, (char *)&norm_speciax_chars);
   (void)ioctl(0, TIOCGETC, (char *)&norm_tchars);
   (void)ioctl(0, TIOCLGET, (char *)&norm_locax_chars);

#endif

}


/*
 * Save the keymaps (normal and game)
 */
static void keymap_game_prepare(void)
{

#ifdef USE_TPOSIX

   /* Acquire the current mapping */
   tcgetattr(0, &game_termios);

   /* Force "Ctrl-C" to interupt */
   game_termios.c_cc[VINTR] = (char)3;

   /* Force "Ctrl-Z" to suspend */
   game_termios.c_cc[VSUSP] = (char)26;

   /* Hack -- Leave "VSTART/VSTOP" alone */

   /* Disable the standard control characters */
   game_termios.c_cc[VQUIT] = (char)-1;
   game_termios.c_cc[VERASE] = (char)-1;
   game_termios.c_cc[VKILL] = (char)-1;
   game_termios.c_cc[VEOF] = (char)-1;
   game_termios.c_cc[VEOL] = (char)-1;

   /* Normally, block until a character is read */
   game_termios.c_cc[VMIN] = 1;
   game_termios.c_cc[VTIME] = 0;

#endif

#ifdef USE_TERMIO

   /* Acquire the current mapping */
   (void)ioctl(0, TCGETA, (char *)&game_termio);

   /* Force "Ctrl-C" to interupt */
   game_termio.c_cc[VINTR] = (char)3;

   /* Force "Ctrl-Z" to suspend */
   game_termio.c_cc[VSUSP] = (char)26;

   /* Hack -- Leave "VSTART/VSTOP" alone */

   /* Disable the standard control characters */
   game_termio.c_cc[VQUIT] = (char)-1;
   game_termio.c_cc[VERASE] = (char)-1;
   game_termio.c_cc[VKILL] = (char)-1;
   game_termio.c_cc[VEOF] = (char)-1;
   game_termio.c_cc[VEOL] = (char)-1;

#if 0
   /* Disable the non-posix control characters */
   game_termio.c_cc[VEOL2] = (char)-1;
   game_termio.c_cc[VSWTCH] = (char)-1;
   game_termio.c_cc[VDSUSP] = (char)-1;
   game_termio.c_cc[VREPRINT] = (char)-1;
   game_termio.c_cc[VDISCARD] = (char)-1;
   game_termio.c_cc[VWERASE] = (char)-1;
   game_termio.c_cc[VLNEXT] = (char)-1;
   game_termio.c_cc[VSTATUS] = (char)-1;
#endif

   /* Normally, block until a character is read */
   game_termio.c_cc[VMIN] = 1;
   game_termio.c_cc[VTIME] = 0;

#endif

#ifdef USE_TCHARS

   /* Get the default game characters */
   (void)ioctl(0, TIOCGETP, (char *)&game_ttyb);
   (void)ioctl(0, TIOCGLTC, (char *)&game_speciax_chars);
   (void)ioctl(0, TIOCGETC, (char *)&game_tchars);
   (void)ioctl(0, TIOCLGET, (char *)&game_locax_chars);

   /* Force suspend (^Z) */
   game_speciax_chars.t_suspc = (char)26;

   /* Cancel some things */
   game_speciax_chars.t_dsuspc = (char)-1;
   game_speciax_chars.t_rprntc = (char)-1;
   game_speciax_chars.t_flushc = (char)-1;
   game_speciax_chars.t_werasc = (char)-1;
   game_speciax_chars.t_lnextc = (char)-1;

   /* Force interupt (^C) */
   game_tchars.t_intrc = (char)3;

   /* Force start/stop (^Q, ^S) */
   game_tchars.t_startc = (char)17;
   game_tchars.t_stopc = (char)19;

   /* Cancel some things */
   game_tchars.t_quitc = (char)-1;
   game_tchars.t_eofc = (char)-1;
   game_tchars.t_brkc = (char)-1;

#endif

}




/*
 * Suspend/Resume
 */
static errr Term_xtra_gcu_alive(int v)
{
   /* Suspend */
   if (!v)
   {
      /* Go to normal keymap mode */
      keymap_norm();

      /* Restore modes */
      nocbreak();
      echo();
      nl();

      /* Hack -- make sure the cursor is visible */
      Term_xtra(TERM_XTRA_SHAPE, 1);

      /* Flush the curses buffer */
      (void)refresh();

#ifdef SPECIAL_BSD
      /* this moves curses to bottom right corner */
      mvcur(curscr->cury, curscr->curx, LINES - 1, 0);
#else
      /* this moves curses to bottom right corner */
      mvcur(getcury(curscr), getcurx(curscr), LINES - 1, 0);
#endif

      /* Exit curses */
      endwin();

      /* Flush the output */
      (void)fflush(stdout);
   }

   /* Resume */
   else
   {
      /* Refresh */
      /* (void)touchwin(curscr); */
      /* (void)wrefresh(curscr); */

      /* Restore the settings */
      cbreak();
      noecho();
      nonl();

      /* Go to angband keymap mode */
      keymap_game();
   }

   /* Success */
   return (0);
}

/*
 * Check for existance of a file
 */
static bool check_file(cptr s)
{
   FILE *fff;

   fff = fopen(s, "r");
   if (!fff) return (FALSE);

   fclose(fff);
   return (TRUE);
}



#ifdef USE_SOUND

/*
 * Initialize sound
 */
static bool init_sound(void)
{
   /* Initialize once */
   if (!can_use_sound)
   {
      int i;

      char wav[128];
      char buf[1024];

      /* Prepare the sounds */
      for (i = 1; i < SOUND_MAX; i++)
      {
	 /* Extract name of sound file */
	 sprintf(wav, "%s.wav", angband_sound_name[i]);

	 /* Access the sound */
	 path_build(buf, sizeof(buf), ANGBAND_DIR_XTRA_SOUND, wav);

	 /* Save the sound filename, if it exists */
	 if (check_file(buf)) sound_file[i] = string_make(buf);
      }

      /* Sound available */
      can_use_sound = TRUE;
   }

   /* Result */
   return (can_use_sound);
}

#endif /* USE_SOUND */



/*
 * Init the "curses" system
 */
static void Term_init_gcu(term *t)
{
   term_data *td = (term_data *)(t->data);

   /* Count init's, handle first */
   if (active++ != 0) return;

   /* Erase the screen */
   (void)wclear(td->win);

   /* Reset the cursor */
   (void)wmove(td->win, 0, 0);

   /* Flush changes */
   (void)wrefresh(td->win);

   /* Game keymap */
   keymap_game();
}


/*
 * Nuke the "curses" system
 */
static void Term_nuke_gcu(term *t)
{
   term_data *td = (term_data *)(t->data);

   /* Delete this window */
   delwin(td->win);

   /* Count nuke's, handle last */
   if (--active != 0) return;

   /* Hack -- make sure the cursor is visible */
   Term_xtra(TERM_XTRA_SHAPE, 1);

#ifdef A_COLOR
  /* Reset colors to defaults */
  start_color();
#endif

#ifdef SPECIAL_BSD
   /* This moves curses to bottom right corner */
   mvcur(curscr->cury, curscr->curx, LINES - 1, 0);
#else
   /* This moves curses to bottom right corner */
   mvcur(getcury(curscr), getcurx(curscr), LINES - 1, 0);
#endif

   /* Flush the curses buffer */
   (void)refresh();

   /* Exit curses */
   endwin();

   /* Flush the output */
   (void)fflush(stdout);

   /* Normal keymap */
   keymap_norm();
}

#ifdef USE_GETCH

/*
 * Process events, with optional wait
 */
static errr Term_xtra_gcu_event(int v)
{
   int i, k;

   /* Wait */
   if (v)
   {
      /* Paranoia -- Wait for it */
      nodelay(stdscr, FALSE);

      /* Get a keypress */
      i = getch();

      /* Mega-Hack -- allow graceful "suspend" */
      for (k = 0; (k < 10) && (i == ERR); k++) i = getch();

      /* Broken input is special */
      if (i == ERR) exit_game_panic();
      if (i == EOF) exit_game_panic();
   }

   /* Do not wait */
   else
   {
      /* Do not wait for it */
      nodelay(stdscr, TRUE);

      /* Check for keypresses */
      i = getch();

      /* Wait for it next time */
      nodelay(stdscr, FALSE);

      /* None ready */
      if (i == ERR) return (1);
      if (i == EOF) return (1);
   }

   /* Enqueue the keypress */
   Term_keypress(i);

   /* Success */
   return (0);
}

#else /* USE_GETCH */

/*
 * Process events (with optional wait)
 */
static errr Term_xtra_gcu_event(int v)
{
   int i, k;

   char buf[2];

   /* Wait */
   if (v)
   {
      /* Wait for one byte */
      i = read(0, buf, 1);

      /* Hack -- Handle bizarre "errors" */
      if ((i <= 0) && (errno != EINTR)) exit_game_panic();
   }

   /* Do not wait */
   else
   {
      /* Get the current flags for stdin */
      k = fcntl(0, F_GETFL, 0);

      /* Oops */
      if (k < 0) return (1);

      /* Tell stdin not to block */
      if (fcntl(0, F_SETFL, k | O_NDELAY) < 0) return (1);

      /* Read one byte, if possible */
      i = read(0, buf, 1);

      /* Replace the flags for stdin */
      if (fcntl(0, F_SETFL, k)) return (1);
   }

   /* Ignore "invalid" keys */
   if ((i != 1) || (!buf[0])) return (1);

   /* Enqueue the keypress */
   Term_keypress(buf[0]);

   /* Success */
   return (0);
}

#endif   /* USE_GETCH */

static int scale_color(int i, int j, int scale)
{
   return (angband_color_table[i][j] * (scale - 1) + 127) / 255;
}

static int create_color(int i, int scale)
{
   int r = scale_color(i, 1, scale);
   int g = scale_color(i, 2, scale);
   int b = scale_color(i, 3, scale);
   int rgb = 16 + scale * scale * r + scale * g + b;

   /* In the case of white and black we need to use the ANSI colors */
   if (r == g && g == b) {
      if (b == 0) rgb = 0;
      if (b == scale) rgb = 15;
   }

   return rgb;
}


/*
 * Adjust the color tables if there's more than 16 available.
 */
static void handle_extended_color_tables(void)
{
#ifdef A_COLOR
   if (COLORS == 256 || COLORS == 88)
   {
      int isbold = bold_extended ? A_BRIGHT : A_NORMAL;
      int i;

      if (keep_terminal_colors) {
         /*
          * If we have more than 16 colors, find the best
          * matches in the terminal's default color table.
          * These numbers correspond to xterm/rxvt's builtin
          * color numbers--they do not correspond to curses'
          * constants OR with curses' color pairs.
          *
          * XTerm has 216 (6*6*6) RGB colors, with each RGB
          * setting 0-5.
          * RXVT has 64 (4*4*4) RGB colors, with each RGB
          * setting 0-3.
          *
          * Both also have the basic 16 ANSI colors, plus some
          * extra grayscale colors which we do not use.
          */
         int scale = COLORS == 256 ? 6 : 4;

         bg_color = create_color(TERM_DARK, scale);
         for (i = 0; i < 16; i++)
         {
            int fg = create_color(i, scale);
            init_pair(i + 1, fg, bg_color);
            colortable[i] = COLOR_PAIR(i + 1) | isbold;
         }
      } else {
         bg_color = 0;
         for (i = 0; i < 16; i++)
         {
            /*
             * Scale components to a range of 0 - 1000 per
             * init_color()'s documentation.
             */
            init_color(i,
               (angband_color_table[i][1] * 1001) / 256,
               (angband_color_table[i][2] * 1001) / 256,
               (angband_color_table[i][3] * 1001) / 256);
            init_pair(i + 1, i, bg_color);
            colortable[i] = COLOR_PAIR(i + 1) | isbold;
         }
      }

      for (i = 0; i < MAX_TERM_DATA; ++i) {
         if (data[i].win) {
            wbkgdset(data[i].win, ' ' | colortable[TERM_DARK]);
         }
      }
      if (data[0].win) {
         /*
          * Adjust the background color on the standard screen
          * as well so separators between the terminals have
          * the same background as the rest.
          */
         chtype term0_bkg = getbkgd(data[0].win);

         if (getbkgd(stdscr) != term0_bkg) {
            wbkgd(stdscr, term0_bkg);
            wrefresh(stdscr);
         }
      }
   }
#endif
}


#ifdef USE_SOUND

/*
 * Hack -- make a sound
 */
static errr Term_xtra_gcu_sound(int v)
{
   char buf[1024];

   /* Sound disabled */
   if (!use_sound) return (1);

   /* Illegal sound */
   if ((v < 0) || (v >= SOUND_MAX)) return (1);

   /* Unknown sound */
   if (!sound_file[v]) return (1);

   sprintf(buf,"./gcusound.sh %s\n", sound_file[v]);
   
   return (system(buf) < 0);

#if 0
   char *argv[4];
   pid_t pid;

   /* Sound disabled */
   if (!use_sound) return (1);

   /* Illegal sound */
   if ((v < 0) || (v >= SOUND_MAX)) return (1);

   /* Unknown sound */
   if (!sound_file[v]) return (1);

   pid=fork();

   /* cannot fork? */
   if (pid==-1) return (1);

   if (pid==0)
   {
      char *argv[4];
      argv[0]="sh";
      argv[1]="-c";
      argv[2]="./gcusound.sh";
      strcpy(argv[3],sound_file[v]);
      execvp(argv[0], argv);
      exit(0);
   }
#endif
   return (0);

}
#endif


/*
 * Handle a "special request"
 */
static errr Term_xtra_gcu(int n, int v)
{
   term_data *td = (term_data *)(Term->data);

   /* Analyze the request */
   switch (n)
   {
      /* Clear screen */
      case TERM_XTRA_CLEAR:
      touchwin(td->win);
      (void)wclear(td->win);
      return (0);

      /* Make a noise */
      case TERM_XTRA_NOISE:
      return write(1, "\007", 1) != 1;

#ifdef USE_SOUND
      /* Make a special sound */
      case TERM_XTRA_SOUND:
	 return (Term_xtra_gcu_sound(v));
#endif

      /* Flush the Curses buffer */
      case TERM_XTRA_FRESH:
      (void)wrefresh(td->win);
      return (0);

      /* Change the cursor visibility */
      case TERM_XTRA_SHAPE:
      curs_set(v);
      return (0);

      /* Suspend/Resume curses */
      case TERM_XTRA_ALIVE:
      return (Term_xtra_gcu_alive(v));

      /* Process events */
      case TERM_XTRA_EVENT:
      return (Term_xtra_gcu_event(v));

      /* Flush events */
      case TERM_XTRA_FLUSH:
      while (!Term_xtra_gcu_event(FALSE));
      return (0);

      /* Delay */
      case TERM_XTRA_DELAY:
      usleep(1000 * v);
      return (0);

      /* React to events */
      case TERM_XTRA_REACT:
      handle_extended_color_tables();
      return (0);

   }


   /* Unknown */
   return (1);
}


/*
 * Actually MOVE the hardware cursor
 */
static errr Term_curs_gcu(int x, int y)
{
   term_data *td = (term_data *)(Term->data);

   /* Literally move the cursor */
   wmove(td->win, y, x);

   /* Success */
   return (0);
}


/*
 * Erase a grid of space
 * Hack -- try to be "semi-efficient".
 */
static errr Term_wipe_gcu(int x, int y, int n)
{
   term_data *td = (term_data *)(Term->data);

   /* Place cursor */
   wmove(td->win, y, x);

   /* Clear to end of line */
   if (x + n >= td->t.wid)
   {
      wclrtoeol(td->win);
   }

   /* Clear some characters */
   else
   {
#ifdef A_COLOR
      if (can_use_color) wattrset(td->win, colortable[TERM_DARK]);
#endif
      whline(td->win, ' ', n);
#ifdef A_COLOR
      if (can_use_color) wattrset(td->win, WA_NORMAL);
#endif
   }

   /* Success */
   return (0);
}

#ifdef USE_NCURSES_ACS
/*
 * this function draws some ACS characters on the screen
 * for DOS-based users: these are the graphical chars (blocks, lines etc)
 *
 * unix-gurus: before you start adding other attributes like A_REVERSE
 * think hard about how map_info() in cave.c should handle the color
 * of something that we here draw in reverse. It's not so simple, alas.
 */
static void Term_acs_text_gcu(int x, int y, int n, byte a, cptr s)
{
   term_data *td = (term_data *)(Term->data);
   int i;

   /* position the cursor */
   wmove(td->win, y, x);

#ifdef A_COLOR
   /* Set the color */
   if (can_use_color) wattrset(td->win, colortable[a & 0x0F]);
#endif

   for (i=0; i < n; i++)
   {
      /* add acs_map of a */
      waddch(td->win, acs_map[(int)s[i]]);
   }

#ifdef A_COLOR
   if (can_use_color) wattrset(td->win, WA_NORMAL);
#endif
}
#endif

/*
 * Place some text on the screen using an attribute
 */
static errr Term_text_gcu(int x, int y, int n, byte a, cptr s)
{
   term_data *td = (term_data *)(Term->data);

#ifdef USE_NCURSES_ACS
   /* do we have colors + 16 ? */
   /* then call special routine for drawing special characters */
   if (a & 0x10)
   {
      Term_acs_text_gcu(x, y, n, a, s);
      return(0);
   }
#endif

   /* Move the cursor and dump the string */
   wmove(td->win, y, x);

#ifdef A_COLOR
   /* Set the color */
   if (can_use_color) wattrset(td->win, colortable[a & 0x0F]);
#endif

   /* Add the text */
   waddnstr(td->win, s, n);

#ifdef A_COLOR
   if (can_use_color) wattrset(td->win, WA_NORMAL);
#endif

   /* Success */
   return (0);
}



static errr term_data_init(term_data *td, int rows, int cols, int y, int x)
{
   term *t = &td->t;

   /* Make sure the window has a positive size */
   if (rows <= 0 || cols <= 0) return (0);

   /* Create a window */
   td->win = newwin(rows, cols, y, x);

   /* Make sure we succeed */
   if (!td->win)
   {
      plog("Failed to setup curses window.");
      return (-1);
   }

   /* Initialize the term */
   term_init(t, cols, rows, 256);

   /* Avoid the bottom right corner */
   t->icky_corner = TRUE;

   /* Erase with "white space" */
   t->attr_blank = TERM_WHITE;
   t->char_blank = ' ';

   /* Set some hooks */
   t->init_hook = Term_init_gcu;
   t->nuke_hook = Term_nuke_gcu;

   /* Set some more hooks */
   t->text_hook = Term_text_gcu;
   t->wipe_hook = Term_wipe_gcu;
   t->curs_hook = Term_curs_gcu;
   t->xtra_hook = Term_xtra_gcu;

   /* Save the data */
   t->data = td;

   /* Activate it */
   Term_activate(t);


   /* Success */
   return (0);
}


static void hook_quit(cptr str)
{
	/* Unused */
	(void)str;

       /* Exit curses */
       endwin();
}


/*
 * Prepare "curses" for use by the file "term.c"
 *
 * Installs the "hook" functions defined above, and then activates
 * the main screen "term", which clears the screen and such things.
 *
 * Someone should really check the semantics of "initscr()"
 */
errr init_gcu(int argc, char *argv[])
{
   int i;

   int num_term = 4, next_win = 0;
   char path[1024];

   /* Parse args */
   for (i = 1; i < argc; i++)
   {
      if (prefix(argv[i], "-B")) {
         bold_extended = TRUE;
      } else if (prefix(argv[i], "-K")) {
         keep_terminal_colors = TRUE;
      }
   }

   setlocale(LC_ALL, "");

#ifdef USE_SOUND

   /* Build the "sound" path */
   path_build(path, sizeof(path), ANGBAND_DIR_XTRA, "sound");

   /* Allocate the path */
   ANGBAND_DIR_XTRA_SOUND = string_make(path);

#endif

   /* Extract the normal keymap */
   keymap_norm_prepare();

#if defined(USG)
   /* Initialize for USG Unix */
   if (initscr() == NULL) return (-1);
#else
   /* Initialize for others systems */
   if (initscr() == (WINDOW*)ERR) return (-1);
#endif

   /* Activate hooks */
   quit_aux = hook_quit;
   core_aux = hook_quit;

   /* Hack -- Require large screen, or Quit with message */
   i = ((LINES < 24) || (COLS < 80));
   if (i) quit("Angband needs an 80x24 'curses' screen");


#ifdef A_COLOR

   /*** Init the Color-pairs and set up a translation table ***/

   /* Do we have color, and enough color, available? */
   can_use_color = ((start_color() != ERR) && has_colors() &&
		    (COLORS >= 8) && (COLOR_PAIRS >= 8));

   if (!can_change_color()) keep_terminal_colors = TRUE;

   if (can_use_color)
   {
      /* Prepare the color pairs */
      /* PAIR_WHITE (pair 0) is *always* WHITE on BLACK */
      init_pair(PAIR_RED, COLOR_RED, bg_color);
      init_pair(PAIR_GREEN, COLOR_GREEN, bg_color);
      init_pair(PAIR_YELLOW, COLOR_YELLOW, bg_color);
      init_pair(PAIR_BLUE, COLOR_BLUE, bg_color);
      init_pair(PAIR_MAGENTA, COLOR_MAGENTA, bg_color);
      init_pair(PAIR_CYAN, COLOR_CYAN, bg_color);
      init_pair(PAIR_BLACK, COLOR_BLACK, bg_color);

      /* Prepare the colors */
      colortable[TERM_DARK] = (COLOR_PAIR(PAIR_BLACK));
      colortable[TERM_WHITE] = (COLOR_PAIR(PAIR_WHITE) | A_BRIGHT);
      colortable[TERM_SLATE] = (COLOR_PAIR(PAIR_WHITE));
      colortable[TERM_ORANGE] = (COLOR_PAIR(PAIR_YELLOW) | A_BRIGHT);
      colortable[TERM_RED] = (COLOR_PAIR(PAIR_RED));
      colortable[TERM_GREEN] = (COLOR_PAIR(PAIR_GREEN));
      colortable[TERM_BLUE] = (COLOR_PAIR(PAIR_BLUE));
      colortable[TERM_UMBER] = (COLOR_PAIR(PAIR_YELLOW));
      colortable[TERM_L_DARK] = (COLOR_PAIR(PAIR_BLACK) | A_BRIGHT);
      colortable[TERM_L_WHITE] = (COLOR_PAIR(PAIR_WHITE));
      colortable[TERM_VIOLET] = (COLOR_PAIR(PAIR_MAGENTA));
      colortable[TERM_YELLOW] = (COLOR_PAIR(PAIR_YELLOW) | A_BRIGHT);
      colortable[TERM_L_RED] = (COLOR_PAIR(PAIR_MAGENTA) | A_BRIGHT);
      colortable[TERM_L_GREEN] = (COLOR_PAIR(PAIR_GREEN) | A_BRIGHT);
      colortable[TERM_L_BLUE] = (COLOR_PAIR(PAIR_BLUE) | A_BRIGHT);
      colortable[TERM_L_UMBER] = (COLOR_PAIR(PAIR_YELLOW));
      handle_extended_color_tables();
   }
#endif

#ifdef USE_SOUND
   /* Handle "arg_sound" */
   if (use_sound != arg_sound)
   {
      /* Initialize (if needed) */
      if (arg_sound && !init_sound())
      {
	 /* Warning */
	 plog("Cannot initialize sound!");

	 /* Cannot enable */
	 arg_sound = FALSE;
      }

      /* Change setting */
      use_sound = arg_sound;
   }
#endif

#ifdef USE_GRAPHICS

   /* Try graphics */
   if (arg_graphics)
   {
      /* if USE_NCURSES_ACS is defined, we can do something with graphics in curses! */
#ifdef USE_NCURSES_ACS
      use_graphics = TRUE;
#endif
   }

#endif /* USE_GRAPHICS */



   /*** Low level preparation ***/

#ifdef USE_GETCH

   /* Paranoia -- Assume no waiting */
   nodelay(stdscr, FALSE);

#endif

   /* Prepare */
   cbreak();
   noecho();
   nonl();
   raw();

   /* Extract the game keymap */
   keymap_game_prepare();


   /*** Now prepare the term(s) ***/
   for (i = 0; i < num_term; i++)
   {
      int rows, cols;
      int y, x;

      switch (i)
      {
	 /* Upper left */
	 case 0: rows = 24;
	    cols = 80;
	    y = x = 0;
	    break;
	 /* Lower left */
	 case 1: rows = LINES - 25;
	    cols = 80;
	    y = 24;
	    x = 0;
	    break;
	 /* Upper right */
	 case 2: rows = 24;
	    cols = COLS - 81;
	    y = 0;
	    x = 81;
	    break;
	 /* Lower right */
	 case 3: rows = LINES - 25;
	    cols = COLS - 81;
	    y = 24;
	    x = 81;
	    break;
	 /* XXX */
	 default: rows = cols = 0;
	     y = x = 0;
	     break;
      }

      /* No non-windows */
      if (rows <= 0 || cols <= 0) continue;

      /* Initialize */
      term_data_init(&data[next_win], rows, cols, y, x);

      /* Store */
      angband_term[next_win] = Term;

      next_win++;
   }

   /* Activate the "Angband" window screen */
   Term_activate(&data[0].t);

   /* Store */
   term_screen = &data[0].t;

   /* Success */
   return (0);
}


#endif /* USE_GCU */


