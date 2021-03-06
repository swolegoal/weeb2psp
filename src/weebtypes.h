#include <stdbool.h>

enum _do_subs { NEVER_SUBS, MAYBE_SUBS, YES_SUBS };

typedef struct {
  char *pretty_name;
  char *in_dir;
  char *out_dir;
  char *log_dir;
  char *lang;
  int do_subs;  // -1 is undef (will set to user config/program defaults)
  int extract_fonts;
  unsigned int audio_srate;
  unsigned int audio_brate;
  unsigned int video_brate;
  unsigned int max_brate;
} batch_t;

#define HAPPY_WEEB 0
