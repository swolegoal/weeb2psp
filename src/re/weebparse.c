#include <assert.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "weebparse.h"
#include "weebtypes.h"
#include "weebutil.h"

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

static const size_t BUFF_SIZE = 4096;

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
  pdata->lex_eof = pdata->lex_limit < pdata->buffer + BUFF_SIZE;
  return 0;
}

static inline batch_t *batch_alloc() {
  batch_t *b = (batch_t*)malloc(sizeof(batch_t));

  b->pretty_name = NULL;
  b->in_dir      = NULL;
  b->out_dir     = NULL;
  b->log_dir     = NULL;
  b->lang        = NULL;

  b->do_subs = -1;
  b->extract_fonts = false;

  b->audio_brate = 0;
  b->audio_srate = 0;
  b->video_brate = 0;
  b->max_brate   = 0;

  return b;
}

static inline void batch_free(batch_t *b) {
  assert(b != NULL);
  free((void *)b->pretty_name);
  free((void *)b->in_dir);
  free((void *)b->out_dir);
  free((void *)b->log_dir);
  free((void *)b->lang);
  free((void *)b);
}

static inline void lex_init(parserdata *pdata) {
  pdata->buffer = (char *) malloc(BUFF_SIZE + 1);

  pdata->lex_cursor
    = pdata->lex_marker
    = pdata->lex_token
    = pdata->lex_limit
    = pdata->buffer + BUFF_SIZE
  ;
  /*!stags:re2c format = "pdata->@@ = NULL;\n"; */
  pdata->lex_eof = 0;

  lex_feed(pdata);
}

static inline void lex_free(parserdata *pdata) {
  free(pdata->buffer);
}

static void print_batch_inistyle(const batch_t *batch) {
  if (batch->pretty_name) {
    printf("\n[%s]\n", batch->pretty_name);
    assert(batch->in_dir && batch->out_dir);
    printf("in_dir = \"%s\"\n", batch->in_dir);
    printf("out_dir = \"%s\"\n", batch->out_dir);
    if (batch->log_dir)
      printf("log_dir = \"%s\"\n", batch->log_dir);

    if (batch->lang)
      printf("lang = \"%s\"\n", batch->lang);

    printf("hardsubs = ");
    switch (batch->do_subs) {
      case -1:
        printf("program/user default (see config/headers)\n");
        break;
      case 0:
        puts("false");
        break;
      case 1:
        puts("true");
        break;
      default:
        die("UNCAUGHT PARSER ERROR!!\n", 1);
        break;
    }
    printf("extract_fonts = %s\n", (batch->extract_fonts ? "true" : "false"));
    printf("audio_target_bitrate = ");
    switch (batch->audio_brate) {
      case 0:
        puts("original");
        break;
      default:
        printf("%u\n", batch->audio_brate);
        break;
    }
    printf("audio_sample_rate = ");
    switch (batch->audio_srate) {
      case 0:
        puts("original");
        break;
      default:
        printf("%u\n", batch->audio_srate);
        break;
    }
    if (batch->video_brate)
      printf("video_target_bitrate = %u\n", batch->video_brate);
    if (batch->max_brate)
      printf("max_bitrate = %u\n", batch->max_brate);
  }
}

static bool validate_batch(const batch_t *batch) {
  if (batch->pretty_name) {
    if (!batch->in_dir) {
      fprintf(stderr, "[ERR]: Section \"%s\" missing \"in_dir\" definition!\n",
        batch->pretty_name);
      return false;
    } else if (!batch->out_dir) {
      fprintf(stderr, "[ERR]: Section \"%s\" missing \"out_dir\" definition!\n",
        batch->pretty_name);
      return false;
    } else {
      // ... other checks
    }
  } else {
    if (batch->in_dir) {
      fprintf(stderr, "[ERR]: in_dir \"%s\" without section!\n", batch->in_dir);
      return false;
    } else if (batch->out_dir) {
      fprintf(stderr, "[ERR]: out_dir \"%s\" without section!\n", batch->out_dir);
      return false;
    } else {
      // ... other checks
    }
  }
  return true;
}

int lex_dispatch(parserdata *pdata) {
  batch_t *batch = batch_alloc();
  unsigned int line_no = 1;

hdr_loop: {
    const char *param, *sep;
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
      section_start = "[";
      section_end = "]";
      sp = " ";
      tab = "\t";
      wsp = (sp | tab)*;
      eol = [\n];
      eq = wsp "=" wsp;
      quo = ["];

      // Data types
      str = ([^] \ eol)+;

      // Section
      section_hdr = wsp section_start @param str @sep section_end wsp eol;

      * {
        fprintf(stderr, "[ERR]: Couldn't parse line %u!  Batch file must start "
                        "with section heading!\n", line_no);
        return -1;
      }

      $ { batch_free(batch); return 0; }

      section_hdr {
        batch = batch_alloc();

        TAG_SSLURP(batch->pretty_name, param, sep);

        LEX_LOOP;
      }

      eol  { HDR_LOOP; }

     */
  }

lex_loop: {
    const char *param, *sep;
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

      // Data types
      fakebool = ('true' | 'false');
      num = [0-9]+;
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

      * {
        fprintf(stderr, "[ERR]: Couldn't parse line %u!\n", line_no);
        if (batch->pretty_name) {
          fprintf(stderr, "Last section heading found was \"[%s]\".\n",
            batch->pretty_name);
        }
        batch_free(batch);
        return -1;
      }

      $ {
        DEBUG(print_batch_inistyle(batch));
        if (!validate_batch(batch)) {
          batch_free(batch);
          return -1;
        } else {
          batch_free(batch);
          return 0;
        }
      }

      section_hdr {
        DEBUG(print_batch_inistyle(batch));

        if (!validate_batch(batch)) {
          batch_free(batch);
          return -1;
        }

        batch_free(batch);
        batch = batch_alloc();

        TAG_SSLURP(batch->pretty_name, param, sep);

        LEX_LOOP;
      }

      eol    { LEX_LOOP; }

      idir   { TAG_SSLURP(batch->in_dir,  param, sep);        LEX_LOOP; }
      odir   { TAG_SSLURP(batch->out_dir, param, sep);        LEX_LOOP; }
      logdir { TAG_SSLURP(batch->log_dir, param, sep);        LEX_LOOP; }
      lang   { TAG_SSLURP(batch->lang,    param, sep);        LEX_LOOP; }

      subs   { TAG_BOOLISHSLURP(batch->do_subs,    param);    LEX_LOOP; }
      fonts  { TAG_BOOLSLURP(batch->extract_fonts, param);    LEX_LOOP; }

      vr     { TAG_UINTSLURP(batch->video_brate, param, sep); LEX_LOOP; }
      abr    { TAG_UINTSLURP(batch->audio_brate, param, sep); LEX_LOOP; }
      asr    { TAG_UINTSLURP(batch->audio_srate, param, sep); LEX_LOOP; }
      maxr   { TAG_UINTSLURP(batch->max_brate,   param, sep); LEX_LOOP; }

      abr_og { batch->audio_brate = 0;                        LEX_LOOP; }
      asr_og { batch->audio_srate = 0;                        LEX_LOOP; }

     */
  }
  return -2;
}

int lex_file(FILE *f, const char *fname) {
  parserdata pdata;
  pdata.conf_file = f;

  lex_init(&pdata);

  {
    int lexstat = lex_dispatch(&pdata);
    lex_free(&pdata);
    if (pdata.conf_file != stdin) {
      fclose(pdata.conf_file);
    }
    if (lexstat != 0) {
      die("Error parsing config file named \"%s\"\n", lexstat, fname);
    } else {
      return lexstat;
    }
  }
}
