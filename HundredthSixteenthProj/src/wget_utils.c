/* Various utility functions.
   Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003,
   2004, 2005, 2006, 2007, 2008 Free Software Foundation, Inc.

This file is part of GNU Wget.

GNU Wget is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

GNU Wget is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wget.  If not, see <http://www.gnu.org/licenses/>.

Additional permission under GNU GPL version 3 section 7

If you modify this program, or any covered work, by linking or
combining it with the OpenSSL project's OpenSSL library (or a
modified version of that library), containing parts covered by the
terms of the OpenSSL or SSLeay licenses, the Free Software Foundation
grants you additional permission to convey the resulting work.
Corresponding Source for a non-source form of such a combination
shall include the source code for the parts of OpenSSL used as well
as that of the covered work.  */

#include <wget_config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef HAVE_MMAP
# include <sys/mman.h>
#endif
#ifdef HAVE_PROCESS_H
# include <process.h>  /* getpid() */
#endif
#ifdef HAVE_UTIME_H
# include <utime.h>
#endif
#ifdef HAVE_SYS_UTIME_H
# include <sys/utime.h>
#endif
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <stdarg.h>
#include <locale.h>

/* For TIOCGWINSZ and friends: */
#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif
#ifdef HAVE_TERMIOS_H
# include <termios.h>
#endif

/* Needed for Unix version of run_with_timeout. */
#include <signal.h>
#include <setjmp.h>

#ifndef HAVE_SIGSETJMP
/* If sigsetjmp is a macro, configure won't pick it up. */
# ifdef sigsetjmp
#  define HAVE_SIGSETJMP
# endif
#endif

#if defined HAVE_SIGSETJMP || defined HAVE_SIGBLOCK
# define USE_SIGNAL_TIMEOUT
#endif

#include "wget_wget.h"
#include "wget_utils.h"
#include "wget_hash.h"

#ifdef TESTING
#include "test.h"
#endif 

/* Utility function: like xstrdup(), but also lowercases S.  */

char *
xstrdup_lower (const char *s)
{
  char *copy = xstrdup (s);
  char *p = copy;
  for (; *p; p++)
    *p = TOLOWER (*p);
  return copy;
}

/* Copy the string formed by two pointers (one on the beginning, other
   on the char after the last char) to a new, malloc-ed location.
   0-terminate it.  */
char *
strdupdelim (const char *beg, const char *end)
{
  char *res = xmalloc (end - beg + 1);
  memcpy (res, beg, end - beg);
  res[end - beg] = '\0';
  return res;
}

/* Parse a string containing comma-separated elements, and return a
   vector of char pointers with the elements.  Spaces following the
   commas are ignored.  */
char **
sepstring (const char *s)
{
  char **res;
  const char *p;
  int i = 0;

  if (!s || !*s)
    return NULL;
  res = NULL;
  p = s;
  while (*s)
    {
      if (*s == ',')
        {
          res = xrealloc (res, (i + 2) * sizeof (char *));
          res[i] = strdupdelim (p, s);
          res[++i] = NULL;
          ++s;
          /* Skip the blanks following the ','.  */
          while (ISSPACE (*s))
            ++s;
          p = s;
        }
      else
        ++s;
    }
  res = xrealloc (res, (i + 2) * sizeof (char *));
  res[i] = strdupdelim (p, s);
  res[i + 1] = NULL;
  return res;
}

/* Like sprintf, but prints into a string of sufficient size freshly
   allocated with malloc, which is returned.  If unable to print due
   to invalid format, returns NULL.  Inability to allocate needed
   memory results in abort, as with xmalloc.  This is in spirit
   similar to the GNU/BSD extension asprintf, but somewhat easier to
   use.

   Internally the function either calls vasprintf or loops around
   vsnprintf until the correct size is found.  Since Wget also ships a
   fallback implementation of vsnprintf, this should be portable.  */

char *
aprintf (const char *fmt, ...)
{
#if defined HAVE_VASPRINTF && !defined DEBUG_MALLOC
  /* Use vasprintf. */
  int ret;
  va_list args;
  char *str;
  va_start (args, fmt);
  ret = vasprintf (&str, fmt, args);
  va_end (args);
  if (ret < 0 && errno == ENOMEM)
    abort ();                   /* for consistency with xmalloc/xrealloc */
  else if (ret < 0)
    return NULL;
  return str;
#else  /* not HAVE_VASPRINTF */

  /* vasprintf is unavailable.  snprintf into a small buffer and
     resize it as necessary. */
  int size = 32;
  char *str = xmalloc (size);

  /* #### This code will infloop and eventually abort in xrealloc if
     passed a FMT that causes snprintf to consistently return -1.  */

  while (1)
    {
      int n;
      va_list args;

      va_start (args, fmt);
      n = vsnprintf (str, size, fmt, args);
      va_end (args);

      /* If the printing worked, return the string. */
      if (n > -1 && n < size)
        return str;

      /* Else try again with a larger buffer. */
      if (n > -1)               /* C99 */
        size = n + 1;           /* precisely what is needed */
      else
        size <<= 1;             /* twice the old size */
      str = xrealloc (str, size);
    }
#endif /* not HAVE_VASPRINTF */
}

/* Concatenate the NULL-terminated list of string arguments into
   freshly allocated space.  */

char *
concat_strings (const char *str0, ...)
{
  va_list args;
  int saved_lengths[5];         /* inspired by Apache's apr_pstrcat */
  char *ret, *p;

  const char *next_str;
  int total_length = 0;
  int argcount;

  /* Calculate the length of and allocate the resulting string. */

  argcount = 0;
  va_start (args, str0);
  for (next_str = str0; next_str != NULL; next_str = va_arg (args, char *))
    {
      int len = strlen (next_str);
      if (argcount < countof (saved_lengths))
        saved_lengths[argcount++] = len;
      total_length += len;
    }
  va_end (args);
  p = ret = xmalloc (total_length + 1);

  /* Copy the strings into the allocated space. */

  argcount = 0;
  va_start (args, str0);
  for (next_str = str0; next_str != NULL; next_str = va_arg (args, char *))
    {
      int len;
      if (argcount < countof (saved_lengths))
        len = saved_lengths[argcount++];
      else
        len = strlen (next_str);
      memcpy (p, next_str, len);
      p += len;
    }
  va_end (args);
  *p = '\0';

  return ret;
}

/* Format the provided time according to the specified format.  The
   format is a string with format elements supported by strftime.  */

static char *
fmttime (time_t t, const char *fmt)
{
  static char output[32];
  struct tm *tm = localtime(&t);
  if (!tm)
    abort ();
  if (!strftime(output, sizeof(output), fmt, tm))
    abort ();
  return output;
}

/* Return pointer to a static char[] buffer in which zero-terminated
   string-representation of TM (in form hh:mm:ss) is printed.

   If TM is NULL, the current time will be used.  */

char *
time_str (time_t t)
{
  return fmttime(t, "%H:%M:%S");
}

/* Like the above, but include the date: YYYY-MM-DD hh:mm:ss.  */

char *
datetime_str (time_t t)
{
  return fmttime(t, "%Y-%m-%d %H:%M:%S");
}

/* The Windows versions of the following two functions are defined in
   mswindows.c. On MSDOS this function should never be called. */

#if !defined(WINDOWS) && !defined(MSDOS)
void
fork_to_background (void)
{
  pid_t pid;
  /* Whether we arrange our own version of opt.lfilename here.  */
  bool logfile_changed = false;

  if (!opt.lfilename)
    {
      /* We must create the file immediately to avoid either a race
         condition (which arises from using unique_name and failing to
         use fopen_excl) or lying to the user about the log file name
         (which arises from using unique_name, printing the name, and
         using fopen_excl later on.)  */
      FILE *new_log_fp = unique_create (DEFAULT_LOGFILE, false, &opt.lfilename);
      if (new_log_fp)
        {
          logfile_changed = true;
          fclose (new_log_fp);
        }
    }
  pid = fork ();
  if (pid < 0)
    {
      /* parent, error */
      perror ("fork");
      exit (1);
    }
  else if (pid != 0)
    {
      /* parent, no error */
      printf (_("Continuing in background, pid %d.\n"), (int) pid);
      if (logfile_changed)
        printf (_("Output will be written to `%s'.\n"), opt.lfilename);
      exit (0);                 /* #### should we use _exit()? */
    }

  /* child: give up the privileges and keep running. */
  setsid ();
  freopen ("/dev/null", "r", stdin);
  freopen ("/dev/null", "w", stdout);
  freopen ("/dev/null", "w", stderr);
}
#endif /* !WINDOWS && !MSDOS */

/* "Touch" FILE, i.e. make its mtime ("modified time") equal the time
   specified with TM.  The atime ("access time") is set to the current
   time.  */

void
touch (const char *file, time_t tm)
{
#ifdef HAVE_STRUCT_UTIMBUF
  struct utimbuf times;
#else
  struct {
    time_t actime;
    time_t modtime;
  } times;
#endif
  times.modtime = tm;
  times.actime = time (NULL);
  if (utime (file, &times) == -1)
    logprintf (LOG_NOTQUIET, "utime(%s): %s\n", file, strerror (errno));
}

/* Checks if FILE is a symbolic link, and removes it if it is.  Does
   nothing under MS-Windows.  */
int
remove_link (const char *file)
{
  int err = 0;
  struct_stat st;

  if (lstat (file, &st) == 0 && S_ISLNK (st.st_mode))
    {
      DEBUGP (("Unlinking %s (symlink).\n", file));
      err = unlink (file);
      if (err != 0)
        logprintf (LOG_VERBOSE, _("Failed to unlink symlink `%s': %s\n"),
                   file, strerror (errno));
    }
  return err;
}

/* Does FILENAME exist?  This is quite a lousy implementation, since
   it supplies no error codes -- only a yes-or-no answer.  Thus it
   will return that a file does not exist if, e.g., the directory is
   unreadable.  I don't mind it too much currently, though.  The
   proper way should, of course, be to have a third, error state,
   other than true/false, but that would introduce uncalled-for
   additional complexity to the callers.  */
bool
file_exists_p (const char *filename)
{
#ifdef HAVE_ACCESS
  return access (filename, F_OK) >= 0;
#else
  struct_stat buf;
  return stat (filename, &buf) >= 0;
#endif
}

/* Returns 0 if PATH is a directory, 1 otherwise (any kind of file).
   Returns 0 on error.  */
bool
file_non_directory_p (const char *path)
{
  struct_stat buf;
  /* Use lstat() rather than stat() so that symbolic links pointing to
     directories can be identified correctly.  */
  if (lstat (path, &buf) != 0)
    return false;
  return S_ISDIR (buf.st_mode) ? false : true;
}

/* Return the size of file named by FILENAME, or -1 if it cannot be
   opened or seeked into. */
wgint
file_size (const char *filename)
{
#if defined(HAVE_FSEEKO) && defined(HAVE_FTELLO)
  wgint size;
  /* We use fseek rather than stat to determine the file size because
     that way we can also verify that the file is readable without
     explicitly checking for permissions.  Inspired by the POST patch
     by Arnaud Wylie.  */
  FILE *fp = fopen (filename, "rb");
  if (!fp)
    return -1;
  fseeko (fp, 0, SEEK_END);
  size = ftello (fp);
  fclose (fp);
  return size;
#else
  struct_stat st;
  if (stat (filename, &st) < 0)
    return -1;
  return st.st_size;
#endif
}

/* stat file names named PREFIX.1, PREFIX.2, etc., until one that
   doesn't exist is found.  Return a freshly allocated copy of the
   unused file name.  */

static char *
unique_name_1 (const char *prefix)
{
  int count = 1;
  int plen = strlen (prefix);
  char *template = (char *)alloca (plen + 1 + 24);
  char *template_tail = template + plen;

  memcpy (template, prefix, plen);
  *template_tail++ = '.';

  do
    number_to_string (template_tail, count++);
  while (file_exists_p (template));

  return xstrdup (template);
}

/* Return a unique file name, based on FILE.

   More precisely, if FILE doesn't exist, it is returned unmodified.
   If not, FILE.1 is tried, then FILE.2, etc.  The first FILE.<number>
   file name that doesn't exist is returned.

   The resulting file is not created, only verified that it didn't
   exist at the point in time when the function was called.
   Therefore, where security matters, don't rely that the file created
   by this function exists until you open it with O_EXCL or
   equivalent.

   If ALLOW_PASSTHROUGH is 0, it always returns a freshly allocated
   string.  Otherwise, it may return FILE if the file doesn't exist
   (and therefore doesn't need changing).  */

char *
unique_name (const char *file, bool allow_passthrough)
{
  /* If the FILE itself doesn't exist, return it without
     modification. */
  if (!file_exists_p (file))
    return allow_passthrough ? (char *)file : xstrdup (file);

  /* Otherwise, find a numeric suffix that results in unused file name
     and return it.  */
  return unique_name_1 (file);
}

/* Create a file based on NAME, except without overwriting an existing
   file with that name.  Providing O_EXCL is correctly implemented,
   this function does not have the race condition associated with
   opening the file returned by unique_name.  */

FILE *
unique_create (const char *name, bool binary, char **opened_name)
{
  /* unique file name, based on NAME */
  char *uname = unique_name (name, false);
  FILE *fp;
  while ((fp = fopen_excl (uname, binary)) == NULL && errno == EEXIST)
    {
      xfree (uname);
      uname = unique_name (name, false);
    }
  if (opened_name && fp != NULL)
    {
      if (fp)
        *opened_name = uname;
      else
        {
          *opened_name = NULL;
          xfree (uname);
        }
    }
  else
    xfree (uname);
  return fp;
}

/* Open the file for writing, with the addition that the file is
   opened "exclusively".  This means that, if the file already exists,
   this function will *fail* and errno will be set to EEXIST.  If
   BINARY is set, the file will be opened in binary mode, equivalent
   to fopen's "wb".

   If opening the file fails for any reason, including the file having
   previously existed, this function returns NULL and sets errno
   appropriately.  */
   
FILE *
fopen_excl (const char *fname, bool binary)
{
  int fd;
#ifdef O_EXCL
  int flags = O_WRONLY | O_CREAT | O_EXCL;
# ifdef O_BINARY
  if (binary)
    flags |= O_BINARY;
# endif
  fd = open (fname, flags, 0666);
  if (fd < 0)
    return NULL;
  return fdopen (fd, binary ? "wb" : "w");
#else  /* not O_EXCL */
  /* Manually check whether the file exists.  This is prone to race
     conditions, but systems without O_EXCL haven't deserved
     better.  */
  if (file_exists_p (fname))
    {
      errno = EEXIST;
      return NULL;
    }
  return fopen (fname, binary ? "wb" : "w");
#endif /* not O_EXCL */
}

/* Create DIRECTORY.  If some of the pathname components of DIRECTORY
   are missing, create them first.  In case any mkdir() call fails,
   return its error status.  Returns 0 on successful completion.

   The behaviour of this function should be identical to the behaviour
   of `mkdir -p' on systems where mkdir supports the `-p' option.  */
int
make_directory (const char *directory)
{
  int i, ret, quit = 0;
  char *dir;

  /* Make a copy of dir, to be able to write to it.  Otherwise, the
     function is unsafe if called with a read-only char *argument.  */
  STRDUP_ALLOCA (dir, directory);

  /* If the first character of dir is '/', skip it (and thus enable
     creation of absolute-pathname directories.  */
  for (i = (*dir == '/'); 1; ++i)
    {
      for (; dir[i] && dir[i] != '/'; i++)
        ;
      if (!dir[i])
        quit = 1;
      dir[i] = '\0';
      /* Check whether the directory already exists.  Allow creation of
         of intermediate directories to fail, as the initial path components
         are not necessarily directories!  */
      if (!file_exists_p (dir))
        ret = mkdir (dir, 0777);
      else
        ret = 0;
      if (quit)
        break;
      else
        dir[i] = '/';
    }
  return ret;
}

/* Merge BASE with FILE.  BASE can be a directory or a file name, FILE
   should be a file name.

   file_merge("/foo/bar", "baz")  => "/foo/baz"
   file_merge("/foo/bar/", "baz") => "/foo/bar/baz"
   file_merge("foo", "bar")       => "bar"

   In other words, it's a simpler and gentler version of uri_merge.  */

char *
file_merge (const char *base, const char *file)
{
  char *result;
  const char *cut = (const char *)strrchr (base, '/');

  if (!cut)
    return xstrdup (file);

  result = xmalloc (cut - base + 1 + strlen (file) + 1);
  memcpy (result, base, cut - base);
  result[cut - base] = '/';
  strcpy (result + (cut - base) + 1, file);

  return result;
}

/* Like fnmatch, but performs a case-insensitive match.  */

int
fnmatch_nocase (const char *pattern, const char *string, int flags)
{
#ifdef FNM_CASEFOLD
  /* The FNM_CASEFOLD flag started as a GNU extension, but it is now
     also present on *BSD platforms, and possibly elsewhere.  */
  return fnmatch (pattern, string, flags | FNM_CASEFOLD);
#else
  /* Turn PATTERN and STRING to lower case and call fnmatch on them. */
  char *patcopy = (char *) alloca (strlen (pattern) + 1);
  char *strcopy = (char *) alloca (strlen (string) + 1);
  char *p;
  for (p = patcopy; *pattern; pattern++, p++)
    *p = TOLOWER (*pattern);
  *p = '\0';
  for (p = strcopy; *string; string++, p++)
    *p = TOLOWER (*string);
  *p = '\0';
  return fnmatch (patcopy, strcopy, flags);
#endif
}

static bool in_acclist (const char *const *, const char *, bool);

/* Determine whether a file is acceptable to be followed, according to
   lists of patterns to accept/reject.  */
bool
acceptable (const char *s)
{
  int l = strlen (s);

  while (l && s[l] != '/')
    --l;
  if (s[l] == '/')
    s += (l + 1);
  if (opt.accepts)
    {
      if (opt.rejects)
        return (in_acclist ((const char *const *)opt.accepts, s, true)
                && !in_acclist ((const char *const *)opt.rejects, s, true));
      else
        return in_acclist ((const char *const *)opt.accepts, s, true);
    }
  else if (opt.rejects)
    return !in_acclist ((const char *const *)opt.rejects, s, true);
  return true;
}

/* Check if D2 is a subdirectory of D1.  E.g. if D1 is `/something', subdir_p()
   will return true if and only if D2 begins with `/something/' or is exactly 
   '/something'.  */
bool
subdir_p (const char *d1, const char *d2)
{
  if (*d1 == '\0')
    return true;
  if (!opt.ignore_case)
    for (; *d1 && *d2 && (*d1 == *d2); ++d1, ++d2)
      ;
  else
    for (; *d1 && *d2 && (TOLOWER (*d1) == TOLOWER (*d2)); ++d1, ++d2)
      ;
  
  return *d1 == '\0' && (*d2 == '\0' || *d2 == '/');
}

/* Iterate through DIRLIST (which must be NULL-terminated), and return the
   first element that matches DIR, through wildcards or front comparison (as
   appropriate).  */
static bool
dir_matches_p (char **dirlist, const char *dir)
{
  char **x;
  int (*matcher) (const char *, const char *, int)
    = opt.ignore_case ? fnmatch_nocase : fnmatch;

  for (x = dirlist; *x; x++)
    {
      /* Remove leading '/' */
      char *p = *x + (**x == '/');
      if (has_wildcards_p (p))
        {
          if (matcher (p, dir, FNM_PATHNAME) == 0)
            break;
        }
      else
        {
          if (subdir_p (p, dir))
            break;
        }
    }
      
  return *x ? true : false;
}

/* Returns whether DIRECTORY is acceptable for download, wrt the
   include/exclude lists.

   The leading `/' is ignored in paths; relative and absolute paths
   may be freely intermixed.  */

bool
accdir (const char *directory)
{
  /* Remove starting '/'.  */
  if (*directory == '/')
    ++directory;
  if (opt.includes)
    {
      if (!dir_matches_p (opt.includes, directory))
        return false;
    }
  if (opt.excludes)
    {
      if (dir_matches_p (opt.excludes, directory))
        return false;
    }
  return true;
}

/* Return true if STRING ends with TAIL.  For instance:

   match_tail ("abc", "bc", false)  -> 1
   match_tail ("abc", "ab", false)  -> 0
   match_tail ("abc", "abc", false) -> 1

   If FOLD_CASE is true, the comparison will be case-insensitive.  */

bool
match_tail (const char *string, const char *tail, bool fold_case)
{
  int i, j;

  /* We want this to be fast, so we code two loops, one with
     case-folding, one without. */

  if (!fold_case)
    {
      for (i = strlen (string), j = strlen (tail); i >= 0 && j >= 0; i--, j--)
        if (string[i] != tail[j])
          break;
    }
  else
    {
      for (i = strlen (string), j = strlen (tail); i >= 0 && j >= 0; i--, j--)
        if (TOLOWER (string[i]) != TOLOWER (tail[j]))
          break;
    }

  /* If the tail was exhausted, the match was succesful.  */
  if (j == -1)
    return true;
  else
    return false;
}

/* Checks whether string S matches each element of ACCEPTS.  A list
   element are matched either with fnmatch() or match_tail(),
   according to whether the element contains wildcards or not.

   If the BACKWARD is false, don't do backward comparison -- just compare
   them normally.  */
static bool
in_acclist (const char *const *accepts, const char *s, bool backward)
{
  for (; *accepts; accepts++)
    {
      if (has_wildcards_p (*accepts))
        {
          int res = opt.ignore_case
            ? fnmatch_nocase (*accepts, s, 0) : fnmatch (*accepts, s, 0);
          /* fnmatch returns 0 if the pattern *does* match the string.  */
          if (res == 0)
            return true;
        }
      else
        {
          if (backward)
            {
              if (match_tail (s, *accepts, opt.ignore_case))
                return true;
            }
          else
            {
              int cmp = opt.ignore_case
                ? strcasecmp (s, *accepts) : strcmp (s, *accepts);
              if (cmp == 0)
                return true;
            }
        }
    }
  return false;
}

/* Return the location of STR's suffix (file extension).  Examples:
   suffix ("foo.bar")       -> "bar"
   suffix ("foo.bar.baz")   -> "baz"
   suffix ("/foo/bar")      -> NULL
   suffix ("/foo.bar/baz")  -> NULL  */
char *
suffix (const char *str)
{
  int i;

  for (i = strlen (str); i && str[i] != '/' && str[i] != '.'; i--)
    ;

  if (str[i++] == '.')
    return (char *)str + i;
  else
    return NULL;
}

/* Return true if S contains globbing wildcards (`*', `?', `[' or
   `]').  */

bool
has_wildcards_p (const char *s)
{
  for (; *s; s++)
    if (*s == '*' || *s == '?' || *s == '[' || *s == ']')
      return true;
  return false;
}

/* Return true if FNAME ends with a typical HTML suffix.  The
   following (case-insensitive) suffixes are presumed to be HTML
   files:
   
     html
     htm
     ?html (`?' matches one character)

   #### CAVEAT.  This is not necessarily a good indication that FNAME
   refers to a file that contains HTML!  */
bool
has_html_suffix_p (const char *fname)
{
  char *suf;

  if ((suf = suffix (fname)) == NULL)
    return false;
  if (!strcasecmp (suf, "html"))
    return true;
  if (!strcasecmp (suf, "htm"))
    return true;
  if (suf[0] && !strcasecmp (suf + 1, "html"))
    return true;
  return false;
}

/* Read a line from FP and return the pointer to freshly allocated
   storage.  The storage space is obtained through malloc() and should
   be freed with free() when it is no longer needed.

   The length of the line is not limited, except by available memory.
   The newline character at the end of line is retained.  The line is
   terminated with a zero character.

   After end-of-file is encountered without anything being read, NULL
   is returned.  NULL is also returned on error.  To distinguish
   between these two cases, use the stdio function ferror().  */

char *
read_whole_line (FILE *fp)
{
  int length = 0;
  int bufsize = 82;
  char *line = xmalloc (bufsize);

  while (fgets (line + length, bufsize - length, fp))
    {
      length += strlen (line + length);
      if (length == 0)
        /* Possible for example when reading from a binary file where
           a line begins with \0.  */
        continue;

      if (line[length - 1] == '\n')
        break;

      /* fgets() guarantees to read the whole line, or to use up the
         space we've given it.  We can double the buffer
         unconditionally.  */
      bufsize <<= 1;
      line = xrealloc (line, bufsize);
    }
  if (length == 0 || ferror (fp))
    {
      xfree (line);
      return NULL;
    }
  if (length + 1 < bufsize)
    /* Relieve the memory from our exponential greediness.  We say
       `length + 1' because the terminating \0 is not included in
       LENGTH.  We don't need to zero-terminate the string ourselves,
       though, because fgets() does that.  */
    line = xrealloc (line, length + 1);
  return line;
}

/* Read FILE into memory.  A pointer to `struct file_memory' are
   returned; use struct element `content' to access file contents, and
   the element `length' to know the file length.  `content' is *not*
   zero-terminated, and you should *not* read or write beyond the [0,
   length) range of characters.

   After you are done with the file contents, call read_file_free to
   release the memory.

   Depending on the operating system and the type of file that is
   being read, read_file() either mmap's the file into memory, or
   reads the file into the core using read().

   If file is named "-", fileno(stdin) is used for reading instead.
   If you want to read from a real file named "-", use "./-" instead.  */

struct file_memory *
read_file (const char *file)
{
  int fd;
  struct file_memory *fm;
  long size;
  bool inhibit_close = false;

  /* Some magic in the finest tradition of Perl and its kin: if FILE
     is "-", just use stdin.  */
  if (HYPHENP (file))
    {
      fd = fileno (stdin);
      inhibit_close = true;
      /* Note that we don't inhibit mmap() in this case.  If stdin is
         redirected from a regular file, mmap() will still work.  */
    }
  else
    fd = open (file, O_RDONLY);
  if (fd < 0)
    return NULL;
  fm = xnew (struct file_memory);

#ifdef HAVE_MMAP
  {
    struct_fstat buf;
    if (fstat (fd, &buf) < 0)
      goto mmap_lose;
    fm->length = buf.st_size;
    /* NOTE: As far as I know, the callers of this function never
       modify the file text.  Relying on this would enable us to
       specify PROT_READ and MAP_SHARED for a marginal gain in
       efficiency, but at some cost to generality.  */
    fm->content = mmap (NULL, fm->length, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE, fd, 0);
    if (fm->content == (char *)MAP_FAILED)
      goto mmap_lose;
    if (!inhibit_close)
      close (fd);

    fm->mmap_p = 1;
    return fm;
  }

 mmap_lose:
  /* The most common reason why mmap() fails is that FD does not point
     to a plain file.  However, it's also possible that mmap() doesn't
     work for a particular type of file.  Therefore, whenever mmap()
     fails, we just fall back to the regular method.  */
#endif /* HAVE_MMAP */

  fm->length = 0;
  size = 512;                   /* number of bytes fm->contents can
                                   hold at any given time. */
  fm->content = xmalloc (size);
  while (1)
    {
      wgint nread;
      if (fm->length > size / 2)
        {
          /* #### I'm not sure whether the whole exponential-growth
             thing makes sense with kernel read.  On Linux at least,
             read() refuses to read more than 4K from a file at a
             single chunk anyway.  But other Unixes might optimize it
             better, and it doesn't *hurt* anything, so I'm leaving
             it.  */

          /* Normally, we grow SIZE exponentially to make the number
             of calls to read() and realloc() logarithmic in relation
             to file size.  However, read() can read an amount of data
             smaller than requested, and it would be unreasonable to
             double SIZE every time *something* was read.  Therefore,
             we double SIZE only when the length exceeds half of the
             entire allocated size.  */
          size <<= 1;
          fm->content = xrealloc (fm->content, size);
        }
      nread = read (fd, fm->content + fm->length, size - fm->length);
      if (nread > 0)
        /* Successful read. */
        fm->length += nread;
      else if (nread < 0)
        /* Error. */
        goto lose;
      else
        /* EOF */
        break;
    }
  if (!inhibit_close)
    close (fd);
  if (size > fm->length && fm->length != 0)
    /* Due to exponential growth of fm->content, the allocated region
       might be much larger than what is actually needed.  */
    fm->content = xrealloc (fm->content, fm->length);
  fm->mmap_p = 0;
  return fm;

 lose:
  if (!inhibit_close)
    close (fd);
  xfree (fm->content);
  xfree (fm);
  return NULL;
}

/* Release the resources held by FM.  Specifically, this calls
   munmap() or xfree() on fm->content, depending whether mmap or
   malloc/read were used to read in the file.  It also frees the
   memory needed to hold the FM structure itself.  */

void
read_file_free (struct file_memory *fm)
{
#ifdef HAVE_MMAP
  if (fm->mmap_p)
    {
      munmap (fm->content, fm->length);
    }
  else
#endif
    {
      xfree (fm->content);
    }
  xfree (fm);
}

/* Free the pointers in a NULL-terminated vector of pointers, then
   free the pointer itself.  */
void
free_vec (char **vec)
{
  if (vec)
    {
      char **p = vec;
      while (*p)
        xfree (*p++);
      xfree (vec);
    }
}

/* Append vector V2 to vector V1.  The function frees V2 and
   reallocates V1 (thus you may not use the contents of neither
   pointer after the call).  If V1 is NULL, V2 is returned.  */
char **
merge_vecs (char **v1, char **v2)
{
  int i, j;

  if (!v1)
    return v2;
  if (!v2)
    return v1;
  if (!*v2)
    {
      /* To avoid j == 0 */
      xfree (v2);
      return v1;
    }
  /* Count v1.  */
  for (i = 0; v1[i]; i++)
    ;
  /* Count v2.  */
  for (j = 0; v2[j]; j++)
    ;
  /* Reallocate v1.  */
  v1 = xrealloc (v1, (i + j + 1) * sizeof (char **));
  memcpy (v1 + i, v2, (j + 1) * sizeof (char *));
  xfree (v2);
  return v1;
}

/* Append a freshly allocated copy of STR to VEC.  If VEC is NULL, it
   is allocated as needed.  Return the new value of the vector. */

char **
vec_append (char **vec, const char *str)
{
  int cnt;                      /* count of vector elements, including
                                   the one we're about to append */
  if (vec != NULL)
    {
      for (cnt = 0; vec[cnt]; cnt++)
        ;
      ++cnt;
    }
  else
    cnt = 1;
  /* Reallocate the array to fit the new element and the NULL. */
  vec = xrealloc (vec, (cnt + 1) * sizeof (char *));
  /* Append a copy of STR to the vector. */
  vec[cnt - 1] = xstrdup (str);
  vec[cnt] = NULL;
  return vec;
}

/* Sometimes it's useful to create "sets" of strings, i.e. special
   hash tables where you want to store strings as keys and merely
   query for their existence.  Here is a set of utility routines that
   makes that transparent.  */

void
string_set_add (struct hash_table *ht, const char *s)
{
  /* First check whether the set element already exists.  If it does,
     do nothing so that we don't have to free() the old element and
     then strdup() a new one.  */
  if (hash_table_contains (ht, s))
    return;

  /* We use "1" as value.  It provides us a useful and clear arbitrary
     value, and it consumes no memory -- the pointers to the same
     string "1" will be shared by all the key-value pairs in all `set'
     hash tables.  */
  hash_table_put (ht, xstrdup (s), "1");
}

/* Synonym for hash_table_contains... */

int
string_set_contains (struct hash_table *ht, const char *s)
{
  return hash_table_contains (ht, s);
}

/* Convert the specified string set to array.  ARRAY should be large
   enough to hold hash_table_count(ht) char pointers.  */

void string_set_to_array (struct hash_table *ht, char **array)
{
  hash_table_iterator iter;
  for (hash_table_iterate (ht, &iter); hash_table_iter_next (&iter); )
    *array++ = iter.key;
}

/* Free the string set.  This frees both the storage allocated for
   keys and the actual hash table.  (hash_table_destroy would only
   destroy the hash table.)  */

void
string_set_free (struct hash_table *ht)
{
  hash_table_iterator iter;
  for (hash_table_iterate (ht, &iter); hash_table_iter_next (&iter); )
    xfree (iter.key);
  hash_table_destroy (ht);
}

/* Utility function: simply call xfree() on all keys and values of HT.  */

void
free_keys_and_values (struct hash_table *ht)
{
  hash_table_iterator iter;
  for (hash_table_iterate (ht, &iter); hash_table_iter_next (&iter); )
    {
      xfree (iter.key);
      xfree (iter.value);
    }
}

/* Get digit grouping data for thousand separors by calling
   localeconv().  The data includes separator string and grouping info
   and is cached after the first call to the function.

   In locales that don't set a thousand separator (such as the "C"
   locale), this forces it to be ",".  We are now only showing
   thousand separators in one place, so this shouldn't be a problem in
   practice.  */

static void
get_grouping_data (const char **sep, const char **grouping)
{
  static const char *cached_sep;
  static const char *cached_grouping;
  static bool initialized;
  if (!initialized)
    {
      /* Get the grouping info from the locale. */
      struct lconv *lconv = localeconv ();
      cached_sep = lconv->thousands_sep;
      cached_grouping = lconv->grouping;
#if ! USE_NLS_PROGRESS_BAR
      /* We can't count column widths, so ensure that the separator
       * is single-byte only (let check below determine what byte). */
      if (strlen(cached_sep) > 1)
        cached_sep = "";
#endif
      if (!*cached_sep)
        {
          /* Many locales (such as "C" or "hr_HR") don't specify
             grouping, which we still want to use it for legibility.
             In those locales set the sep char to ',', unless that
             character is used for decimal point, in which case set it
             to ".".  */
          if (*lconv->decimal_point != ',')
            cached_sep = ",";
          else
            cached_sep = ".";
          cached_grouping = "\x03";
        }
      initialized = true;
    }
  *sep = cached_sep;
  *grouping = cached_grouping;
}

/* Return a printed representation of N with thousand separators.
   This should respect locale settings, with the exception of the "C"
   locale which mandates no separator, but we use one anyway.

   Unfortunately, we cannot use %'d (in fact it would be %'j) to get
   the separators because it's too non-portable, and it's hard to test
   for this feature at configure time.  Besides, it wouldn't display
   separators in the "C" locale, still used by many Unix users.  */

const char *
with_thousand_seps (wgint n)
{
  static char outbuf[48];
  char *p = outbuf + sizeof outbuf;

  /* Info received from locale */
  const char *grouping, *sep;
  int seplen;

  /* State information */
  int i = 0, groupsize;
  const char *atgroup;

  bool negative = n < 0;

  /* Initialize grouping data. */
  get_grouping_data (&sep, &grouping);
  seplen = strlen (sep);
  atgroup = grouping;
  groupsize = *atgroup++;

  /* This would overflow on WGINT_MIN, but printing negative numbers
     is not an important goal of this fuinction.  */
  if (negative)
    n = -n;

  /* Write the number into the buffer, backwards, inserting the
     separators as necessary.  */
  *--p = '\0';
  while (1)
    {
      *--p = n % 10 + '0';
      n /= 10;
      if (n == 0)
        break;
      /* Prepend SEP to every groupsize'd digit and get new groupsize.  */
      if (++i == groupsize)
        {
          if (seplen == 1)
            *--p = *sep;
          else
            memcpy (p -= seplen, sep, seplen);
          i = 0;
          if (*atgroup)
            groupsize = *atgroup++;
        }
    }
  if (negative)
    *--p = '-';

  return p;
}

/* N, a byte quantity, is converted to a human-readable abberviated
   form a la sizes printed by `ls -lh'.  The result is written to a
   static buffer, a pointer to which is returned.

   Unlike `with_thousand_seps', this approximates to the nearest unit.
   Quoting GNU libit: "Most people visually process strings of 3-4
   digits effectively, but longer strings of digits are more prone to
   misinterpretation.  Hence, converting to an abbreviated form
   usually improves readability."

   This intentionally uses kilobyte (KB), megabyte (MB), etc. in their
   original computer-related meaning of "powers of 1024".  We don't
   use the "*bibyte" names invented in 1998, and seldom used in
   practice.  Wikipedia's entry on "binary prefix" discusses this in
   some detail.  */

char *
human_readable (HR_NUMTYPE n)
{
  /* These suffixes are compatible with those of GNU `ls -lh'. */
  static char powers[] =
    {
      'K',                      /* kilobyte, 2^10 bytes */
      'M',                      /* megabyte, 2^20 bytes */
      'G',                      /* gigabyte, 2^30 bytes */
      'T',                      /* terabyte, 2^40 bytes */
      'P',                      /* petabyte, 2^50 bytes */
      'E',                      /* exabyte,  2^60 bytes */
    };
  static char buf[8];
  int i;

  /* If the quantity is smaller than 1K, just print it. */
  if (n < 1024)
    {
      snprintf (buf, sizeof (buf), "%d", (int) n);
      return buf;
    }

  /* Loop over powers, dividing N with 1024 in each iteration.  This
     works unchanged for all sizes of wgint, while still avoiding
     non-portable `long double' arithmetic.  */
  for (i = 0; i < countof (powers); i++)
    {
      /* At each iteration N is greater than the *subsequent* power.
         That way N/1024.0 produces a decimal number in the units of
         *this* power.  */
      if ((n / 1024) < 1024 || i == countof (powers) - 1)
        {
          double val = n / 1024.0;
          /* Print values smaller than 10 with one decimal digits, and
             others without any decimals.  */
          snprintf (buf, sizeof (buf), "%.*f%c",
                    val < 10 ? 1 : 0, val, powers[i]);
          return buf;
        }
      n /= 1024;
    }
  return NULL;                  /* unreached */
}

/* Count the digits in the provided number.  Used to allocate space
   when printing numbers.  */

int
numdigit (wgint number)
{
  int cnt = 1;
  if (number < 0)
    ++cnt;                      /* accomodate '-' */
  while ((number /= 10) != 0)
    ++cnt;
  return cnt;
}

#define PR(mask) *p++ = n / (mask) + '0'

/* DIGITS_<D> is used to print a D-digit number and should be called
   with mask==10^(D-1).  It prints n/mask (the first digit), reducing
   n to n%mask (the remaining digits), and calling DIGITS_<D-1>.
   Recursively this continues until DIGITS_1 is invoked.  */

#define DIGITS_1(mask) PR (mask)
#define DIGITS_2(mask) PR (mask), n %= (mask), DIGITS_1 ((mask) / 10)
#define DIGITS_3(mask) PR (mask), n %= (mask), DIGITS_2 ((mask) / 10)
#define DIGITS_4(mask) PR (mask), n %= (mask), DIGITS_3 ((mask) / 10)
#define DIGITS_5(mask) PR (mask), n %= (mask), DIGITS_4 ((mask) / 10)
#define DIGITS_6(mask) PR (mask), n %= (mask), DIGITS_5 ((mask) / 10)
#define DIGITS_7(mask) PR (mask), n %= (mask), DIGITS_6 ((mask) / 10)
#define DIGITS_8(mask) PR (mask), n %= (mask), DIGITS_7 ((mask) / 10)
#define DIGITS_9(mask) PR (mask), n %= (mask), DIGITS_8 ((mask) / 10)
#define DIGITS_10(mask) PR (mask), n %= (mask), DIGITS_9 ((mask) / 10)

/* DIGITS_<11-20> are only used on machines with 64-bit wgints. */

#define DIGITS_11(mask) PR (mask), n %= (mask), DIGITS_10 ((mask) / 10)
#define DIGITS_12(mask) PR (mask), n %= (mask), DIGITS_11 ((mask) / 10)
#define DIGITS_13(mask) PR (mask), n %= (mask), DIGITS_12 ((mask) / 10)
#define DIGITS_14(mask) PR (mask), n %= (mask), DIGITS_13 ((mask) / 10)
#define DIGITS_15(mask) PR (mask), n %= (mask), DIGITS_14 ((mask) / 10)
#define DIGITS_16(mask) PR (mask), n %= (mask), DIGITS_15 ((mask) / 10)
#define DIGITS_17(mask) PR (mask), n %= (mask), DIGITS_16 ((mask) / 10)
#define DIGITS_18(mask) PR (mask), n %= (mask), DIGITS_17 ((mask) / 10)
#define DIGITS_19(mask) PR (mask), n %= (mask), DIGITS_18 ((mask) / 10)

/* Shorthand for casting to wgint. */
#define W wgint

/* Print NUMBER to BUFFER in base 10.  This is equivalent to
   `sprintf(buffer, "%lld", (long long) number)', only typically much
   faster and portable to machines without long long.

   The speedup may make a difference in programs that frequently
   convert numbers to strings.  Some implementations of sprintf,
   particularly the one in some versions of GNU libc, have been known
   to be quite slow when converting integers to strings.

   Return the pointer to the location where the terminating zero was
   printed.  (Equivalent to calling buffer+strlen(buffer) after the
   function is done.)

   BUFFER should be large enough to accept as many bytes as you expect
   the number to take up.  On machines with 64-bit wgints the maximum
   needed size is 24 bytes.  That includes the digits needed for the
   largest 64-bit number, the `-' sign in case it's negative, and the
   terminating '\0'.  */

char *
number_to_string (char *buffer, wgint number)
{
  char *p = buffer;
  wgint n = number;

  int last_digit_char = 0;

#if (SIZEOF_WGINT != 4) && (SIZEOF_WGINT != 8)
  /* We are running in a very strange environment.  Leave the correct
     printing to sprintf.  */
  p += sprintf (buf, "%j", (intmax_t) (n));
#else  /* (SIZEOF_WGINT == 4) || (SIZEOF_WGINT == 8) */

  if (n < 0)
    {
      if (n < -WGINT_MAX)
        {
          /* n = -n would overflow because -n would evaluate to a
             wgint value larger than WGINT_MAX.  Need to make n
             smaller and handle the last digit separately.  */
          int last_digit = n % 10;
          /* The sign of n%10 is implementation-defined. */
          if (last_digit < 0)
            last_digit_char = '0' - last_digit;
          else
            last_digit_char = '0' + last_digit;
          /* After n is made smaller, -n will not overflow. */
          n /= 10;
        }

      *p++ = '-';
      n = -n;
    }

  /* Use the DIGITS_ macro appropriate for N's number of digits.  That
     way printing any N is fully open-coded without a loop or jump.
     (Also see description of DIGITS_*.)  */

  if      (n < 10)                       DIGITS_1 (1);
  else if (n < 100)                      DIGITS_2 (10);
  else if (n < 1000)                     DIGITS_3 (100);
  else if (n < 10000)                    DIGITS_4 (1000);
  else if (n < 100000)                   DIGITS_5 (10000);
  else if (n < 1000000)                  DIGITS_6 (100000);
  else if (n < 10000000)                 DIGITS_7 (1000000);
  else if (n < 100000000)                DIGITS_8 (10000000);
  else if (n < 1000000000)               DIGITS_9 (100000000);
#if SIZEOF_WGINT == 4
  /* wgint is 32 bits wide: no number has more than 10 digits. */
  else                                   DIGITS_10 (1000000000);
#else
  /* wgint is 64 bits wide: handle numbers with 9-19 decimal digits.
     Constants are constructed by compile-time multiplication to avoid
     dealing with different notations for 64-bit constants
     (nL/nLL/nI64, depending on the compiler and architecture).  */
  else if (n < 10*(W)1000000000)         DIGITS_10 (1000000000);
  else if (n < 100*(W)1000000000)        DIGITS_11 (10*(W)1000000000);
  else if (n < 1000*(W)1000000000)       DIGITS_12 (100*(W)1000000000);
  else if (n < 10000*(W)1000000000)      DIGITS_13 (1000*(W)1000000000);
  else if (n < 100000*(W)1000000000)     DIGITS_14 (10000*(W)1000000000);
  else if (n < 1000000*(W)1000000000)    DIGITS_15 (100000*(W)1000000000);
  else if (n < 10000000*(W)1000000000)   DIGITS_16 (1000000*(W)1000000000);
  else if (n < 100000000*(W)1000000000)  DIGITS_17 (10000000*(W)1000000000);
  else if (n < 1000000000*(W)1000000000) DIGITS_18 (100000000*(W)1000000000);
  else                                   DIGITS_19 (1000000000*(W)1000000000);
#endif

  if (last_digit_char)
    *p++ = last_digit_char;

  *p = '\0';
#endif /* (SIZEOF_WGINT == 4) || (SIZEOF_WGINT == 8) */

  return p;
}

#undef PR
#undef W
#undef SPRINTF_WGINT
#undef DIGITS_1
#undef DIGITS_2
#undef DIGITS_3
#undef DIGITS_4
#undef DIGITS_5
#undef DIGITS_6
#undef DIGITS_7
#undef DIGITS_8
#undef DIGITS_9
#undef DIGITS_10
#undef DIGITS_11
#undef DIGITS_12
#undef DIGITS_13
#undef DIGITS_14
#undef DIGITS_15
#undef DIGITS_16
#undef DIGITS_17
#undef DIGITS_18
#undef DIGITS_19

#define RING_SIZE 3

/* Print NUMBER to a statically allocated string and return a pointer
   to the printed representation.

   This function is intended to be used in conjunction with printf.
   It is hard to portably print wgint values:
    a) you cannot use printf("%ld", number) because wgint can be long
       long on 32-bit machines with LFS.
    b) you cannot use printf("%lld", number) because NUMBER could be
       long on 32-bit machines without LFS, or on 64-bit machines,
       which do not require LFS.  Also, Windows doesn't support %lld.
    c) you cannot use printf("%j", (int_max_t) number) because not all
       versions of printf support "%j", the most notable being the one
       on Windows.
    d) you cannot #define WGINT_FMT to the appropriate format and use
       printf(WGINT_FMT, number) because that would break translations
       for user-visible messages, such as printf("Downloaded: %d
       bytes\n", number).

   What you should use instead is printf("%s", number_to_static_string
   (number)).

   CAVEAT: since the function returns pointers to static data, you
   must be careful to copy its result before calling it again.
   However, to make it more useful with printf, the function maintains
   an internal ring of static buffers to return.  That way things like
   printf("%s %s", number_to_static_string (num1),
   number_to_static_string (num2)) work as expected.  Three buffers
   are currently used, which means that "%s %s %s" will work, but "%s
   %s %s %s" won't.  If you need to print more than three wgints,
   bump the RING_SIZE (or rethink your message.)  */

char *
number_to_static_string (wgint number)
{
  static char ring[RING_SIZE][24];
  static int ringpos;
  char *buf = ring[ringpos];
  number_to_string (buf, number);
  ringpos = (ringpos + 1) % RING_SIZE;
  return buf;
}

/* Determine the width of the terminal we're running on.  If that's
   not possible, return 0.  */

int
determine_screen_width (void)
{
  /* If there's a way to get the terminal size using POSIX
     tcgetattr(), somebody please tell me.  */
#ifdef TIOCGWINSZ
  int fd;
  struct winsize wsz;

  if (opt.lfilename != NULL)
    return 0;

  fd = fileno (stderr);
  if (ioctl (fd, TIOCGWINSZ, &wsz) < 0)
    return 0;                   /* most likely ENOTTY */

  return wsz.ws_col;
#elif defined(WINDOWS)
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  if (!GetConsoleScreenBufferInfo (GetStdHandle (STD_ERROR_HANDLE), &csbi))
    return 0;
  return csbi.dwSize.X;
#else  /* neither TIOCGWINSZ nor WINDOWS */
  return 0;
#endif /* neither TIOCGWINSZ nor WINDOWS */
}

/* Whether the rnd system (either rand or [dl]rand48) has been
   seeded.  */
static int rnd_seeded;

/* Return a random number between 0 and MAX-1, inclusive.

   If the system does not support lrand48 and MAX is greater than the
   value of RAND_MAX+1 on the system, the returned value will be in
   the range [0, RAND_MAX].  This may be fixed in a future release.
   The random number generator is seeded automatically the first time
   it is called.

   This uses lrand48 where available, rand elsewhere.  DO NOT use it
   for cryptography.  It is only meant to be used in situations where
   quality of the random numbers returned doesn't really matter.  */

int
random_number (int max)
{
#ifdef HAVE_DRAND48
  if (!rnd_seeded)
    {
      srand48 ((long) time (NULL) ^ (long) getpid ());
      rnd_seeded = 1;
    }
  return lrand48 () % max;
#else  /* not HAVE_DRAND48 */

  double bounded;
  int rnd;
  if (!rnd_seeded)
    {
      srand ((unsigned) time (NULL) ^ (unsigned) getpid ());
      rnd_seeded = 1;
    }
  rnd = rand ();

  /* Like rand() % max, but uses the high-order bits for better
     randomness on architectures where rand() is implemented using a
     simple congruential generator.  */

  bounded = (double) max * rnd / (RAND_MAX + 1.0);
  return (int) bounded;

#endif /* not HAVE_DRAND48 */
}

/* Return a random uniformly distributed floating point number in the
   [0, 1) range.  Uses drand48 where available, and a really lame
   kludge elsewhere.  */

double
random_float (void)
{
#ifdef HAVE_DRAND48
  if (!rnd_seeded)
    {
      srand48 ((long) time (NULL) ^ (long) getpid ());
      rnd_seeded = 1;
    }
  return drand48 ();
#else  /* not HAVE_DRAND48 */
  return (  random_number (10000) / 10000.0
          + random_number (10000) / (10000.0 * 10000.0)
          + random_number (10000) / (10000.0 * 10000.0 * 10000.0)
          + random_number (10000) / (10000.0 * 10000.0 * 10000.0 * 10000.0));
#endif /* not HAVE_DRAND48 */
}

/* Implementation of run_with_timeout, a generic timeout-forcing
   routine for systems with Unix-like signal handling.  */

#ifdef USE_SIGNAL_TIMEOUT
# ifdef HAVE_SIGSETJMP
#  define SETJMP(env) sigsetjmp (env, 1)

static sigjmp_buf run_with_timeout_env;

static void
abort_run_with_timeout (int sig)
{
  assert (sig == SIGALRM);
  siglongjmp (run_with_timeout_env, -1);
}
# else /* not HAVE_SIGSETJMP */
#  define SETJMP(env) setjmp (env)

static jmp_buf run_with_timeout_env;

static void
abort_run_with_timeout (int sig)
{
  assert (sig == SIGALRM);
  /* We don't have siglongjmp to preserve the set of blocked signals;
     if we longjumped out of the handler at this point, SIGALRM would
     remain blocked.  We must unblock it manually. */
  int mask = siggetmask ();
  mask &= ~sigmask (SIGALRM);
  sigsetmask (mask);

  /* Now it's safe to longjump. */
  longjmp (run_with_timeout_env, -1);
}
# endif /* not HAVE_SIGSETJMP */

/* Arrange for SIGALRM to be delivered in TIMEOUT seconds.  This uses
   setitimer where available, alarm otherwise.

   TIMEOUT should be non-zero.  If the timeout value is so small that
   it would be rounded to zero, it is rounded to the least legal value
   instead (1us for setitimer, 1s for alarm).  That ensures that
   SIGALRM will be delivered in all cases.  */

static void
alarm_set (double timeout)
{
#ifdef ITIMER_REAL
  /* Use the modern itimer interface. */
  struct itimerval itv;
  xzero (itv);
  itv.it_value.tv_sec = (long) timeout;
  itv.it_value.tv_usec = 1000000 * (timeout - (long)timeout);
  if (itv.it_value.tv_sec == 0 && itv.it_value.tv_usec == 0)
    /* Ensure that we wait for at least the minimum interval.
       Specifying zero would mean "wait forever".  */
    itv.it_value.tv_usec = 1;
  setitimer (ITIMER_REAL, &itv, NULL);
#else  /* not ITIMER_REAL */
  /* Use the old alarm() interface. */
  int secs = (int) timeout;
  if (secs == 0)
    /* Round TIMEOUTs smaller than 1 to 1, not to zero.  This is
       because alarm(0) means "never deliver the alarm", i.e. "wait
       forever", which is not what someone who specifies a 0.5s
       timeout would expect.  */
    secs = 1;
  alarm (secs);
#endif /* not ITIMER_REAL */
}

/* Cancel the alarm set with alarm_set. */

static void
alarm_cancel (void)
{
#ifdef ITIMER_REAL
  struct itimerval disable;
  xzero (disable);
  setitimer (ITIMER_REAL, &disable, NULL);
#else  /* not ITIMER_REAL */
  alarm (0);
#endif /* not ITIMER_REAL */
}

/* Call FUN(ARG), but don't allow it to run for more than TIMEOUT
   seconds.  Returns true if the function was interrupted with a
   timeout, false otherwise.

   This works by setting up SIGALRM to be delivered in TIMEOUT seconds
   using setitimer() or alarm().  The timeout is enforced by
   longjumping out of the SIGALRM handler.  This has several
   advantages compared to the traditional approach of relying on
   signals causing system calls to exit with EINTR:

     * The callback function is *forcibly* interrupted after the
       timeout expires, (almost) regardless of what it was doing and
       whether it was in a syscall.  For example, a calculation that
       takes a long time is interrupted as reliably as an IO
       operation.

     * It works with both SYSV and BSD signals because it doesn't
       depend on the default setting of SA_RESTART.

     * It doesn't require special handler setup beyond a simple call
       to signal().  (It does use sigsetjmp/siglongjmp, but they're
       optional.)

   The only downside is that, if FUN allocates internal resources that
   are normally freed prior to exit from the functions, they will be
   lost in case of timeout.  */

bool
run_with_timeout (double timeout, void (*fun) (void *), void *arg)
{
  int saved_errno;

  if (timeout == 0)
    {
      fun (arg);
      return false;
    }

  signal (SIGALRM, abort_run_with_timeout);
  if (SETJMP (run_with_timeout_env) != 0)
    {
      /* Longjumped out of FUN with a timeout. */
      signal (SIGALRM, SIG_DFL);
      return true;
    }
  alarm_set (timeout);
  fun (arg);

  /* Preserve errno in case alarm() or signal() modifies it. */
  saved_errno = errno;
  alarm_cancel ();
  signal (SIGALRM, SIG_DFL);
  errno = saved_errno;

  return false;
}

#else  /* not USE_SIGNAL_TIMEOUT */

#ifndef WINDOWS
/* A stub version of run_with_timeout that just calls FUN(ARG).  Don't
   define it under Windows, because Windows has its own version of
   run_with_timeout that uses threads.  */

bool
run_with_timeout (double timeout, void (*fun) (void *), void *arg)
{
  fun (arg);
  return false;
}
#endif /* not WINDOWS */
#endif /* not USE_SIGNAL_TIMEOUT */

#ifndef WINDOWS

/* Sleep the specified amount of seconds.  On machines without
   nanosleep(), this may sleep shorter if interrupted by signals.  */

void
xsleep (double seconds)
{
#ifdef HAVE_NANOSLEEP
  /* nanosleep is the preferred interface because it offers high
     accuracy and, more importantly, because it allows us to reliably
     restart receiving a signal such as SIGWINCH.  (There was an
     actual Debian bug report about --limit-rate malfunctioning while
     the terminal was being resized.)  */
  struct timespec sleep, remaining;
  sleep.tv_sec = (long) seconds;
  sleep.tv_nsec = 1000000000 * (seconds - (long) seconds);
  while (nanosleep (&sleep, &remaining) < 0 && errno == EINTR)
    /* If nanosleep has been interrupted by a signal, adjust the
       sleeping period and return to sleep.  */
    sleep = remaining;
#elif defined(HAVE_USLEEP)
  /* If usleep is available, use it in preference to select.  */
  if (seconds >= 1)
    {
      /* On some systems, usleep cannot handle values larger than
         1,000,000.  If the period is larger than that, use sleep
         first, then add usleep for subsecond accuracy.  */
      sleep (seconds);
      seconds -= (long) seconds;
    }
  usleep (seconds * 1000000);
#else /* fall back select */
  /* Note that, although Windows supports select, it can't be used to
     implement sleeping because Winsock's select doesn't implement
     timeout when it is passed NULL pointers for all fd sets.  (But it
     does under Cygwin, which implements Unix-compatible select.)  */
  struct timeval sleep;
  sleep.tv_sec = (long) seconds;
  sleep.tv_usec = 1000000 * (seconds - (long) seconds);
  select (0, NULL, NULL, NULL, &sleep);
  /* If select returns -1 and errno is EINTR, it means we were
     interrupted by a signal.  But without knowing how long we've
     actually slept, we can't return to sleep.  Using gettimeofday to
     track sleeps is slow and unreliable due to clock skew.  */
#endif
}

#endif /* not WINDOWS */

/* Encode the octets in DATA of length LENGTH to base64 format,
   storing the result to DEST.  The output will be zero-terminated,
   and must point to a writable buffer of at least
   1+BASE64_LENGTH(length) bytes.  The function returns the length of
   the resulting base64 data, not counting the terminating zero.

   This implementation does not emit newlines after 76 characters of
   base64 data.  */

int
base64_encode (const void *data, int length, char *dest)
{
  /* Conversion table.  */
  static const char tbl[64] = {
    'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
    'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
    'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
    'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
  };
  /* Access bytes in DATA as unsigned char, otherwise the shifts below
     don't work for data with MSB set. */
  const unsigned char *s = data;
  /* Theoretical ANSI violation when length < 3. */
  const unsigned char *end = (const unsigned char *) data + length - 2;
  char *p = dest;

  /* Transform the 3x8 bits to 4x6 bits, as required by base64.  */
  for (; s < end; s += 3)
    {
      *p++ = tbl[s[0] >> 2];
      *p++ = tbl[((s[0] & 3) << 4) + (s[1] >> 4)];
      *p++ = tbl[((s[1] & 0xf) << 2) + (s[2] >> 6)];
      *p++ = tbl[s[2] & 0x3f];
    }

  /* Pad the result if necessary...  */
  switch (length % 3)
    {
    case 1:
      *p++ = tbl[s[0] >> 2];
      *p++ = tbl[(s[0] & 3) << 4];
      *p++ = '=';
      *p++ = '=';
      break;
    case 2:
      *p++ = tbl[s[0] >> 2];
      *p++ = tbl[((s[0] & 3) << 4) + (s[1] >> 4)];
      *p++ = tbl[((s[1] & 0xf) << 2)];
      *p++ = '=';
      break;
    }
  /* ...and zero-terminate it.  */
  *p = '\0';

  return p - dest;
}

/* Store in C the next non-whitespace character from the string, or \0
   when end of string is reached.  */
#define NEXT_CHAR(c, p) do {                    \
  c = (unsigned char) *p++;                     \
} while (ISSPACE (c))

#define IS_ASCII(c) (((c) & 0x80) == 0)

/* Decode data from BASE64 (a null-terminated string) into memory
   pointed to by DEST.  DEST is assumed to be large enough to
   accomodate the decoded data, which is guaranteed to be no more than
   3/4*strlen(base64).

   Since DEST is assumed to contain binary data, it is not
   NUL-terminated.  The function returns the length of the data
   written to TO.  -1 is returned in case of error caused by malformed
   base64 input.

   This function originates from Free Recode.  */

int
base64_decode (const char *base64, void *dest)
{
  /* Table of base64 values for first 128 characters.  Note that this
     assumes ASCII (but so does Wget in other places).  */
  static const signed char base64_char_to_value[128] =
    {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  /*   0-  9 */
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  /*  10- 19 */
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  /*  20- 29 */
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  /*  30- 39 */
      -1,  -1,  -1,  62,  -1,  -1,  -1,  63,  52,  53,  /*  40- 49 */
      54,  55,  56,  57,  58,  59,  60,  61,  -1,  -1,  /*  50- 59 */
      -1,  -1,  -1,  -1,  -1,  0,   1,   2,   3,   4,   /*  60- 69 */
      5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  /*  70- 79 */
      15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  /*  80- 89 */
      25,  -1,  -1,  -1,  -1,  -1,  -1,  26,  27,  28,  /*  90- 99 */
      29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  /* 100-109 */
      39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  /* 110-119 */
      49,  50,  51,  -1,  -1,  -1,  -1,  -1             /* 120-127 */
    };
#define BASE64_CHAR_TO_VALUE(c) ((int) base64_char_to_value[c])
#define IS_BASE64(c) ((IS_ASCII (c) && BASE64_CHAR_TO_VALUE (c) >= 0) || c == '=')

  const char *p = base64;
  char *q = dest;

  while (1)
    {
      unsigned char c;
      unsigned long value;

      /* Process first byte of a quadruplet.  */
      NEXT_CHAR (c, p);
      if (!c)
        break;
      if (c == '=' || !IS_BASE64 (c))
        return -1;              /* illegal char while decoding base64 */
      value = BASE64_CHAR_TO_VALUE (c) << 18;

      /* Process second byte of a quadruplet.  */
      NEXT_CHAR (c, p);
      if (!c)
        return -1;              /* premature EOF while decoding base64 */
      if (c == '=' || !IS_BASE64 (c))
        return -1;              /* illegal char while decoding base64 */
      value |= BASE64_CHAR_TO_VALUE (c) << 12;
      *q++ = value >> 16;

      /* Process third byte of a quadruplet.  */
      NEXT_CHAR (c, p);
      if (!c)
        return -1;              /* premature EOF while decoding base64 */
      if (!IS_BASE64 (c))
        return -1;              /* illegal char while decoding base64 */

      if (c == '=')
        {
          NEXT_CHAR (c, p);
          if (!c)
            return -1;          /* premature EOF while decoding base64 */
          if (c != '=')
            return -1;          /* padding `=' expected but not found */
          continue;
        }

      value |= BASE64_CHAR_TO_VALUE (c) << 6;
      *q++ = 0xff & value >> 8;

      /* Process fourth byte of a quadruplet.  */
      NEXT_CHAR (c, p);
      if (!c)
        return -1;              /* premature EOF while decoding base64 */
      if (c == '=')
        continue;
      if (!IS_BASE64 (c))
        return -1;              /* illegal char while decoding base64 */

      value |= BASE64_CHAR_TO_VALUE (c);
      *q++ = 0xff & value;
    }
#undef IS_BASE64
#undef BASE64_CHAR_TO_VALUE

  return q - (char *) dest;
}

#undef IS_ASCII
#undef NEXT_CHAR

/* Simple merge sort for use by stable_sort.  Implementation courtesy
   Zeljko Vrba with additional debugging by Nenad Barbutov.  */

static void
mergesort_internal (void *base, void *temp, size_t size, size_t from, size_t to,
                    int (*cmpfun) (const void *, const void *))
{
#define ELT(array, pos) ((char *)(array) + (pos) * size)
  if (from < to)
    {
      size_t i, j, k;
      size_t mid = (to + from) / 2;
      mergesort_internal (base, temp, size, from, mid, cmpfun);
      mergesort_internal (base, temp, size, mid + 1, to, cmpfun);
      i = from;
      j = mid + 1;
      for (k = from; (i <= mid) && (j <= to); k++)
        if (cmpfun (ELT (base, i), ELT (base, j)) <= 0)
          memcpy (ELT (temp, k), ELT (base, i++), size);
        else
          memcpy (ELT (temp, k), ELT (base, j++), size);
      while (i <= mid)
        memcpy (ELT (temp, k++), ELT (base, i++), size);
      while (j <= to)
        memcpy (ELT (temp, k++), ELT (base, j++), size);
      for (k = from; k <= to; k++)
        memcpy (ELT (base, k), ELT (temp, k), size);
    }
#undef ELT
}

/* Stable sort with interface exactly like standard library's qsort.
   Uses mergesort internally, allocating temporary storage with
   alloca.  */

void
stable_sort (void *base, size_t nmemb, size_t size,
             int (*cmpfun) (const void *, const void *))
{
  if (size > 1)
    {
      void *temp = alloca (nmemb * size * sizeof (void *));
      mergesort_internal (base, temp, size, 0, nmemb - 1, cmpfun);
    }
}

/* Print a decimal number.  If it is equal to or larger than ten, the
   number is rounded.  Otherwise it is printed with one significant
   digit without trailing zeros and with no more than three fractional
   digits total.  For example, 0.1 is printed as "0.1", 0.035 is
   printed as "0.04", 0.0091 as "0.009", and 0.0003 as simply "0".

   This is useful for displaying durations because it provides
   order-of-magnitude information without unnecessary clutter --
   long-running downloads are shown without the fractional part, and
   short ones still retain one significant digit.  */

const char *
print_decimal (double number)
{
  static char buf[32];
  double n = number >= 0 ? number : -number;

  if (n >= 9.95)
    /* Cut off at 9.95 because the below %.1f would round 9.96 to
       "10.0" instead of "10".  OTOH 9.94 will print as "9.9".  */
    snprintf (buf, sizeof buf, "%.0f", number);
  else if (n >= 0.95)
    snprintf (buf, sizeof buf, "%.1f", number);
  else if (n >= 0.001)
    snprintf (buf, sizeof buf, "%.1g", number);
  else if (n >= 0.0005)
    /* round [0.0005, 0.001) to 0.001 */
    snprintf (buf, sizeof buf, "%.3f", number);
  else
    /* print numbers close to 0 as 0, not 0.000 */
    strcpy (buf, "0");

  return buf;
}

#ifdef TESTING

const char *
test_subdir_p()
{
  int i;
  struct {
    char *d1;
    char *d2;
    bool result;
  } test_array[] = {
    { "/somedir", "/somedir", true },
    { "/somedir", "/somedir/d2", true },
    { "/somedir/d1", "/somedir", false },
  };
  
  for (i = 0; i < countof(test_array); ++i) 
    {
      bool res = subdir_p (test_array[i].d1, test_array[i].d2);

      mu_assert ("test_subdir_p: wrong result", 
                 res == test_array[i].result);
    }

  return NULL;
}

const char *
test_dir_matches_p()
{
  int i;
  struct {
    char *dirlist[3];
    char *dir;
    bool result;
  } test_array[] = {
    { { "/somedir", "/someotherdir", NULL }, "somedir", true },
    { { "/somedir", "/someotherdir", NULL }, "anotherdir", false },
    { { "/somedir", "/*otherdir", NULL }, "anotherdir", true },
    { { "/somedir/d1", "/someotherdir", NULL }, "somedir/d1", true },
    { { "*/*d1", "/someotherdir", NULL }, "somedir/d1", true },
    { { "/somedir/d1", "/someotherdir", NULL }, "d1", false },
    { { "!COMPLETE", NULL, NULL }, "!COMPLETE", true },
    { { "*COMPLETE", NULL, NULL }, "!COMPLETE", true },
    { { "*/!COMPLETE", NULL, NULL }, "foo/!COMPLETE", true },
    { { "*COMPLETE", NULL, NULL }, "foo/!COMPLETE", false },
    { { "*/*COMPLETE", NULL, NULL }, "foo/!COMPLETE", true },
    { { "/dir with spaces", NULL, NULL }, "dir with spaces", true },
    { { "/dir*with*spaces", NULL, NULL }, "dir with spaces", true },
  };
  
  for (i = 0; i < countof(test_array); ++i) 
    {
      bool res = dir_matches_p (test_array[i].dirlist, test_array[i].dir);
      
      mu_assert ("test_dir_matches_p: wrong result", 
                 res == test_array[i].result);
    }

  return NULL;
}

#endif /* TESTING */

