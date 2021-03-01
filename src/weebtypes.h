#include <stdbool.h>

//Make portable later
#include <linux/limits.h>  // for PATH_MAX

#define BATCH_QUEUE_SZ 256

enum _do_subs { NEVER_SUBS, MAYBE_SUBS, YES_SUBS };

typedef struct _batch {
  char in_dir[PATH_MAX];
  char out_dir[PATH_MAX];
  char log_dir[PATH_MAX];
  char *lang;
  int do_subs;
  bool extract_fonts;
} batch;

#define HAPPY_WEEB 0
