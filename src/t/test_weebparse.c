#include <stdio.h>

#include <errno.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <cmocka.h>

#include "weebparse.h"
#include "weebtypes.h"
#include "weebutil.h"
#include "test_weebparse.h"

extern int errno;

void _ini_asserts(const batch_t *b, const char *pn, const char *idir,
                  const char *odir, const char *ldir, const char *lang,
                  int sub, int fnt, int as, int ab, int vb, int mb) {
  assert_non_null(b->pretty_name);
  assert_non_null(b->in_dir);
  assert_non_null(b->out_dir);

  assert_string_equal((const char*)b->pretty_name, pn);
  assert_string_equal((const char*)b->in_dir, idir);
  assert_string_equal((const char*)b->out_dir, odir);
  if (b->log_dir) {
    assert_string_equal((const char*)b->log_dir, ldir);
  } else {
    assert_null(b->log_dir);
  }
  if (b->lang) {
    assert_string_equal((const char*)b->lang, lang);
  } else {
    assert_null(b->lang);
  }
  assert_int_equal(b->do_subs, sub);
  assert_int_equal(b->extract_fonts, fnt);
  assert_int_equal((int) b->audio_srate, as);
  assert_int_equal((int) b->audio_brate, ab);
  assert_int_equal((int) b->video_brate, vb);
  assert_int_equal((int) b->max_brate, mb);
}

void test_batch_inistyle(const batch_t *batch, int batch_no) {
  if (batch->pretty_name) {
    INISTYLE_ASSERTS(batch);
  }
}

static void test_batch_goodfile(void **state) {
  (void) state;
  FILE *good_file = fopen(BATCHFILE, "r");
  if (good_file == NULL) {
    fail_msg("%s: Error %d, can't open \"%s\": %s!\n",
             BATCHFILE, errno, BATCHFILE, strerror(errno));
  }

  int rv = lex_file(good_file, BATCHFILE);
  assert_int_equal(rv, 0);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_batch_goodfile),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
