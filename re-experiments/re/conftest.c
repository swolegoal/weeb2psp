#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Uncomment to add debug printing of parsed batch vars (slow)
// #define DEBUG_PRINT

extern int errno;

void die(const char *msg, int status, ...) {
  va_list args;
  va_start(args, status);
  vfprintf(stderr, msg, args);
  va_end(args);

  exit(status);
}

/*!max:re2c*/
static const size_t BUFF_SIZE = 4096;

typedef struct {
  char *pretty_name;
  char *in_dir;
  char *out_dir;
  char *log_dir;
  char *lang;
  int do_subs;  // -1 is undef (will set to user config/program defaults)
  bool extract_fonts;
  unsigned int audio_srate;
  unsigned int audio_brate;
  unsigned int video_brate;
  unsigned int max_brate;
} batch_t;

typedef struct {
  FILE    *conf_file;
  char    *buffer;
  char    *lex_limit;
  char    *lex_cursor;
  char    *lex_marker;
  char    *lex_token;
  /*!stags:re2c format = 'const char *@@;\n'; */
  size_t  lex_eof;
} parserdata;

inline static int lex_feed(parserdata *pdata) {
  if (pdata->lex_eof)
    return 1;

  const size_t free = pdata->lex_token - pdata->buffer;
  if (free < 1)
    return 2;

  memmove(pdata->buffer, pdata->lex_token, pdata->lex_limit - pdata->lex_token);
  pdata->lex_limit  -= free;
  pdata->lex_cursor -= free;
  pdata->lex_marker -= free;
  pdata->lex_token  -= free;

  /*!stags:re2c format = "if (pdata->@@) pdata->@@ -= free;\n"; */
  pdata->lex_limit += fread(pdata->lex_limit, 1, free, pdata->conf_file);
  pdata->lex_limit[0] = 0;
  pdata->lex_eof |= pdata->lex_limit < pdata->buffer + BUFF_SIZE;
  return 0;
}

void batch_reset(batch_t *b) {
  b->pretty_name = NULL;
  b->in_dir      = NULL;
  b->out_dir     = NULL;
  b->log_dir     = NULL;
  b->lang        = NULL;

  b->do_subs = -1;

  b->audio_brate = 0;
  b->audio_srate = 0;
  b->video_brate = 0;
  b->max_brate   = 0;
}

void lex_init(parserdata *pdata) {
  //pdata->buffer = (char *) malloc(BUFF_SIZE + YYMAXFILL);
  pdata->buffer = (char *) malloc(BUFF_SIZE + 1);

  pdata->lex_cursor
    = pdata->lex_marker
    = pdata->lex_token
    = pdata->lex_limit
    = pdata->buffer + BUFF_SIZE
  ;
  /*!mtags:re2c format = "pdata->@@ = 0;\n"; */
  pdata->lex_eof = 0;
}

#define TAG_SSLURP(dest, begin, end)  {         \
    size_t sz = (size_t)((end - begin) + 1);    \
    dest = malloc(sz);                          \
    strncpy(dest, begin, sz - 1);               \
    dest[sz - 1] = '\0';                        \
  }

#define TAG_BOOLSLURP(var, stag)  {                                            \
    switch (stag[0]) {                                                         \
      case 't': {                                                              \
        var = true;                                                            \
        break;                                                                 \
      }                                                                        \
      case 'f': {                                                              \
        var = false;                                                           \
        break;                                                                 \
      }                                                                        \
      default:                                                                 \
        die("Bad fake boolean parameter starting with \"%c\"\n!", 1, stag[0]); \
        break;                                                                 \
    }                                                                          \
  }

#define TAG_BOOLISHSLURP(var, stag)  {                                         \
    switch (stag[0]) {                                                         \
      case 't': {                                                              \
        var = 1;                                                               \
        break;                                                                 \
      }                                                                        \
      case 'f': {                                                              \
        var = 0;                                                               \
        break;                                                                 \
      }                                                                        \
      default:                                                                 \
        die("Bad fake boolean parameter starting with \"%c\"\n!", 1, stag[0]); \
        break;                                                                 \
    }                                                                          \
  }

#define TAG_UINTSLURP(dest, begin, end)  {     \
    {                                          \
      char *temp = NULL;                       \
      TAG_SSLURP(temp, begin, end);            \
      sscanf((const char *)temp, "%u", dest);  \
      free(temp);                              \
    }                                          \
  }

#define PRINT_BATCH_INISTYLE(batch)  {                                         \
    printf("\n[%s]\n", batch->pretty_name);                                    \
    printf("in_dir = \"%s\"\n", batch->in_dir);                                \
    printf("out_dir = \"%s\"\n", batch->out_dir);                              \
    if (batch->log_dir)                                                        \
      printf("log_dir = \"%s\"\n", batch->log_dir);                            \
                                                                               \
    printf("hardsubs = ");                                                     \
    switch (batch->do_subs) {                                                  \
      case -1:                                                                 \
        printf("program/user default (see config/headers)\n");                 \
        break;                                                                 \
      case 0:                                                                  \
        puts("false");                                                         \
        break;                                                                 \
      case 1:                                                                  \
        puts("true");                                                          \
        break;                                                                 \
      default:                                                                 \
        die("UNCAUGHT PARSER ERROR!!\n", 1);                                   \
        break;                                                                 \
    }                                                                          \
    printf("extract_fonts = %s\n", (batch->extract_fonts ? "true" : "false")); \
    printf("audio_target_bitrate = ");                                         \
    switch (batch->audio_brate) {                                              \
      case 0:                                                                  \
        puts("original");                                                      \
        break;                                                                 \
      default:                                                                 \
        printf("%u\n", batch->audio_brate);                                    \
        break;                                                                 \
    }                                                                          \
    printf("audio_sample_rate = ");                                            \
    switch (batch->audio_srate) {                                              \
      case 0:                                                                  \
        puts("original");                                                      \
        break;                                                                 \
      default:                                                                 \
        printf("%u\n", batch->audio_srate);                                    \
        break;                                                                 \
    }                                                                          \
    if (batch->video_brate)                                                    \
      printf("video_target_bitrate = %u\n", batch->video_brate);               \
    if (batch->max_brate)                                                      \
      printf("max_bitrate = %u\n", batch->max_brate);                          \
  }

#define LEX_LOOP  { line_no++; goto lex_loop; }

/*!maxnmatch:re2c*/
int lex_dispatch(parserdata *pdata) {
  batch_t *batch = NULL;
  unsigned int line_no = 1;

lex_loop: {
    const char *param, *sep;  // "uints"
    param = sep = NULL;

    pdata->lex_token = pdata->lex_cursor;

    /*!re2c
      re2c:tags:expression = "pdata->@@";
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
      section_start = "[";
      section_end = "]";
      _eol = [\n];
      eol = _eol+;

      // Data types
      fakebool = ('true' | 'false');
      num = [0-9]+;
      str = ([^] \ _eol)+;
      og = "original";

      // Parameters
      idir   = wsp "in_dir"               eq quo @param str      @sep quo wsp eol;
      odir   = wsp "out_dir"              eq quo @param str      @sep quo wsp eol;
      logdir = wsp "log_dir"              eq quo @param str      @sep quo wsp eol;
      lang   = wsp "lang"                 eq quo @param str      @sep quo wsp eol;
      subs   = wsp "hardsubs"             eq     @param fakebool @sep     wsp eol;
      fonts  = wsp "extract_fonts"        eq     @param fakebool @sep     wsp eol;
      vr     = wsp "video_target_bitrate" eq     @param num      @sep     wsp eol;
      abr    = wsp "audio_target_bitrate" eq     @param num      @sep     wsp eol;
      abr_og = wsp "audio_target_bitrate" eq     @param og       @sep     wsp eol;
      asr_og = wsp "audio_sample_rate"    eq     @param og       @sep     wsp eol;
      asr    = wsp "audio_sample_rate"    eq     @param num      @sep     wsp eol;
      maxr   = wsp "max_bitrate"          eq     @param num      @sep     wsp eol;

      // Section
      section_hdr = wsp section_start @param str @sep section_end wsp eol;

      * {
        fprintf(stderr, "[ERR]: Couldn't parse line %u!  "
                        "Last section heading found was \"[%s]\".\n",
                line_no, batch->pretty_name);
        return -1;
      }

      $ {
        #ifdef DEBUG_PRINT
          PRINT_BATCH_INISTYLE(batch);
        #endif
        return 0;
      }

      section_hdr {
        if (batch) {
          #ifdef DEBUG_PRINT
            PRINT_BATCH_INISTYLE(batch);
          #endif
          {
            bool badflag = false;
            if (!batch->in_dir) {
              fprintf(stderr, "[ERR]: Section \"%s\" missing \"in_dir\" definition!\n",
                      batch->pretty_name);
              badflag = true;
            }
            if (!batch->out_dir) {
              fprintf(stderr, "[ERR]: Section \"%s\" missing \"out_dir\" definition!\n",
                      batch->pretty_name);
              badflag = true;
            }
            if (badflag)
              exit(1);
          }
          free((void *)batch->pretty_name);
          free((void *)batch->in_dir);
          free((void *)batch->out_dir);
          free((void *)batch->log_dir);
          free((void *)batch->lang);
          free((void *)batch);
        }
        batch = malloc(sizeof(batch_t));
        batch_reset(batch);

        TAG_SSLURP(batch->pretty_name, param, sep);

        LEX_LOOP;
      }

      idir    { TAG_SSLURP(batch->in_dir,  param, sep);         LEX_LOOP; }
      odir    { TAG_SSLURP(batch->out_dir, param, sep);         LEX_LOOP; }
      logdir  { TAG_SSLURP(batch->log_dir, param, sep);         LEX_LOOP; }
      lang    { TAG_SSLURP(batch->lang,    param, sep);         LEX_LOOP; }

      subs    { TAG_BOOLISHSLURP(batch->do_subs,    param);     LEX_LOOP; }
      fonts   { TAG_BOOLSLURP(batch->extract_fonts, param);     LEX_LOOP; }

      vr      { TAG_UINTSLURP(batch->video_brate, param, sep);  LEX_LOOP; }
      abr     { TAG_UINTSLURP(batch->audio_brate, param, sep);  LEX_LOOP; }
      asr     { TAG_UINTSLURP(batch->audio_srate, param, sep);  LEX_LOOP; }
      maxr    { TAG_UINTSLURP(batch->max_brate,   param, sep);  LEX_LOOP; }

      abr_og  { batch->audio_brate = 0;                         LEX_LOOP; }
      asr_og  { batch->audio_srate = 0;                         LEX_LOOP; }

     */
  }
  return -2;
}

int main(int argc, char *argv[]) {
  parserdata pdata;

  if (argc < 2) {
    die("Usage:  %s CONF_FILE\n", 1, argv[0]);
  } else if (strncmp("-", argv[1], _POSIX_ARG_MAX) == 0) {
    pdata.conf_file = stdin;
  } else {
    pdata.conf_file = fopen(argv[1], "r");
  }

  if (pdata.conf_file == NULL) {
    die("%s: Error %d, can't open \"%s\": %s!\n",
        errno, argv[0], errno, argv[1], strerror(errno));
  }
  lex_init(&pdata);


  {
    int lexstat = lex_dispatch(&pdata);
    if (lexstat != 0) {
      die("Error parsing config file named \"%s\"\n", lexstat, argv[1]);
    } else {
      return lexstat;
    }
  }
}
