#include <errno.h>
#include <limits.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int errno;

void die(const char *msg, int status, ...) {
  va_list args;
  va_start(args, status);
  vfprintf(stderr, msg, args);
  va_end(args);

  exit(status);
}

#define BUFF_SIZE 16384
#define PRETTY_MAX 1024
#define NOTBOOL_SZ 6

typedef struct {
  const char *pretty_name; // remember to use PRETTY_MAX when strcpy'ing!
  const char *in_dir;      // remember to use PATH_MAX when strcpy'ing!
  const char *out_dir;     // remember to use PATH_MAX when strcpy'ing!
  const char *log_dir;     // remember to use PATH_MAX when strcpy'ing!
  const char *lang;
  int do_subs;
  bool extract_fonts;
  unsigned int audio_srate;
  unsigned int audio_brate;
  unsigned int video_brate;
  unsigned int max_brate;
} batch;

typedef struct {
  FILE    *conf_file;
  char    buffer[BUFF_SIZE + 1];
  char    *lex_limit;
  char    *lex_cursor;
  char    *lex_marker;
  char    *lex_token;
  size_t  lex_eof;
} parserdata;

inline static int lex_feed(parserdata *pdata) {
  if (pdata->lex_eof)
    return 1;

  const size_t free = pdata->lex_token - pdata->buffer;
  if (free < 1)
    return 2;

  memmove(pdata->buffer, pdata->lex_token, pdata->lex_limit - pdata->lex_token);
  pdata->lex_limit -= free;
  pdata->lex_cursor -= free;
  pdata->lex_marker -= free;
  pdata->lex_token -= free;
  fprintf(stderr, "nom nom\n");
  pdata->lex_limit += fread(pdata->lex_limit, 1, free, pdata->conf_file);
  pdata->lex_limit[0] = 0;
  pdata->lex_eof |= pdata->lex_limit < pdata->buffer + BUFF_SIZE;
  return 0;
}

void batch_nullify_sflags(batch *b) {
  b->pretty_name = NULL;
  b->in_dir      = NULL;
  b->out_dir     = NULL;
  b->log_dir     = NULL;
  b->lang        = NULL;
}

parserdata *lex_init(void) {
  parserdata *pdata;

  pdata->lex_cursor
    = pdata->lex_marker
    = pdata->lex_token
    = pdata->lex_limit
    = pdata->buffer + BUFF_SIZE
  ;
  pdata->lex_eof = 0;

  lex_feed(pdata);

  return pdata;
}

char *tag_snip(const char *begin, const char *cur) {
  char *retval;
  memcpy(retval, begin, (size_t)(cur - begin));
  return retval;
}

/*!maxnmatch:re2c*/
int lex_dispatch(FILE *conf_file) {
  parserdata *pdata = lex_init();

  /*!stags:re2c format = 'const char *@@;'; */
lex_loop: {
  const char *section_name, *in_dir, *out_dir, *log_dir, *lang,  // strings
             *hardsubs, *extract_fonts,   // Fake bools
             *audio_srate_og, *audio_brate_og,  // Optional og dumpster
             *audio_srate, *audio_brate, *video_brate, *max_brate;  // "uints"

  section_name = in_dir = out_dir = log_dir = lang = hardsubs = extract_fonts
    = audio_srate_og = audio_brate_og = audio_srate = audio_brate
    = video_brate = max_brate = NULL;

    pdata->lex_token = pdata->lex_cursor;

    /*!re2c
      re2c:tags:expression = "@@";
      re2c:eof = 0;
      re2c:api:style = free-form;
      re2c:define:YYCTYPE = char;
      re2c:flags:tags = 1;

      re2c:define:YYCURSOR = pdata->lex_cursor;
      re2c:define:YYMARKER = pdata->lex_marker;
      re2c:define:YYLIMIT = pdata->lex_limit;
      re2c:define:YYFILL = "lex_feed(pdata) == 0";

      // Operators and separators.
      sp = " ";
      tab = "\t";
      wsp = (sp | tab)*;
      eq = wsp "=" wsp;
      quo = ["];
      _eol = [\n];
      eol = _eol+;

      // Data types
      fakebool = ('true' | 'false');
      num = [0-9]+;
      str = ([^] \ _eol)+;
      og = "original";

      // Parameters
      idir   = wsp "in_dir"               eq @in_dir         str      wsp eol;
      odir   = wsp "out_dir"              eq @out_dir        str      wsp eol;
      logdir = wsp "log_dir"              eq @log_dir        str      wsp eol;
      lang   = wsp "lang"                 eq @lang           str      wsp eol;
      subs   = wsp "hardsubs"             eq @hardsubs       fakebool wsp eol;
      fonts  = wsp "extract_fonts"        eq @extract_fonts  fakebool wsp eol;
      vr     = wsp "video_target_bitrate" eq @video_brate    num      wsp eol;
      abr    = wsp "audio_target_bitrate" eq @audio_brate    num      wsp eol;
      abr_og = wsp "audio_target_bitrate" eq @audio_brate_og og       wsp eol;
      asr_og = wsp "audio_sample_rate"    eq @audio_srate_og og       wsp eol;
      asr    = wsp "audio_sample_rate"    eq @audio_srate    num      wsp eol;
      maxr   = wsp "max_bitrate"          eq @max_brate      num      wsp eol;

      param = (idir | odir | logdir | lang | subs | fonts | vr | abr | abr_og
              | asr_og | asr | maxr);

      // Section
      section_hdr = wsp "[" @section_name str "]" wsp eol;
      section = section_hdr param+;

      *       { return -1; }
      $       { return 0; }

      section {
        //batch *parsed_batch;

        //die("%s", 1, (video_brate - audio_srate));
        //sscanf(audio_srate, "%u", batch->audio_srate);

        //goto lex_loop;
      }

     */
  }
  return -2;
}

int main(int argc, char *argv[]) {
  FILE *conf_file;
  if (argc < 2) {
    die("Usage:  %s CONF_FILE\n", 1, argv[0]);
  } else if (strncmp("-", argv[1], _POSIX_ARG_MAX) == 0) {
    conf_file = stdin;
  } else {
    conf_file = fopen(argv[1], "r");
  }

  if (conf_file == NULL) {
    die("%s: Error %d, can't open \"%s\": %s!\n",
        errno, argv[0], errno, argv[1], strerror(errno));
  }

  {
    int lexstat = lex_dispatch(conf_file);
    if (lexstat != 0) {
      die("Error parsing config file named \"%s\"\n", lexstat, argv[1]);
    } else {
      return lexstat;
    }
  }
}
