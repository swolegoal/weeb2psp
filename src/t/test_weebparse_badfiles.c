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

#define BATCHFILE_NOHEADER1  "batch-examples/bad-noheader1.batch"

static void _lexret_assert(const char *fname) {
  int rv;
  FILE *file = fopen(fname, "r");
  if (file == NULL) {
    fail_msg("%s: Error %d, can't open \"%s\": %s!\n",
             fname, errno, fname, strerror(errno));
  }

  rv = lex_file(file, fname);
  fprintf(stderr, "\"%s\" returned %d\n", fname, rv);
  assert_int_equal(rv, -1);
}

static void test_batch_noheader1(void **state) {
  (void) state;
  _lexret_assert(BATCHFILE_NOHEADER1);
}

static void test_batch_noparam_abrate(void **state) {
  (void) state;
  _lexret_assert("batch-examples/bad-emptyparam-ab.batch");
}

static void test_batch_noparam_asrate(void **state) {
  (void) state;
  _lexret_assert("batch-examples/bad-emptyparam-as.batch");
}

static void test_batch_noparam_fnt(void **state) {
  (void) state;
  _lexret_assert("batch-examples/bad-emptyparam-fonts.batch");
}

static void test_batch_noparam_indir(void **state) {
  (void) state;
  _lexret_assert("batch-examples/bad-emptyparam-id.batch");
}

static void test_batch_noparam_lang(void **state) {
  (void) state;
  _lexret_assert("batch-examples/bad-emptyparam-lang.batch");
}

static void test_batch_noparam_logdir(void **state) {
  (void) state;
  _lexret_assert("batch-examples/bad-emptyparam-ld.batch");
}

static void test_batch_noparam_maxbrate(void **state) {
  (void) state;
  _lexret_assert("batch-examples/bad-emptyparam-mb.batch");
}

static void test_batch_noparam_outdir(void **state) {
  (void) state;
  _lexret_assert("batch-examples/bad-emptyparam-od.batch");
}

static void test_batch_noparam_subs(void **state) {
  (void) state;
  _lexret_assert("batch-examples/bad-emptyparam-subs.batch");
}

static void test_batch_noparam_vbrate(void **state) {
  (void) state;
  _lexret_assert("batch-examples/bad-emptyparam-vb.batch");
}

static void test_batch_noparam_all(void **state) {
  (void) state;
  _lexret_assert("batch-examples/bad-emptyparam1.batch");
}

static void test_batch_nohdr_fonts(void **state) {
  (void) state;
  _lexret_assert("batch-examples/bad-nohdr-fonts.batch");
}

static void test_batch_nohdr_subs(void **state) {
  (void) state;
  _lexret_assert("batch-examples/bad-nohdr-subs.batch");
}

static void test_batch_nohdr_ogs(void **state) {
  (void) state;
  _lexret_assert("batch-examples/bad-nohdr-ogs.batch");
}

static void test_batch_noheader2(void **state) {
  (void) state;
  _lexret_assert("batch-examples/bad-nohdr.batch");
}

static void test_batch_badparam(void **state) {
  (void) state;
  _lexret_assert("batch-examples/bad-param.batch");
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_batch_noheader1),
      cmocka_unit_test(test_batch_noheader2),
      cmocka_unit_test(test_batch_nohdr_ogs),
      cmocka_unit_test(test_batch_nohdr_subs),
      cmocka_unit_test(test_batch_nohdr_fonts),
      cmocka_unit_test(test_batch_noparam_indir),
      cmocka_unit_test(test_batch_noparam_outdir),
      cmocka_unit_test(test_batch_noparam_logdir),
      cmocka_unit_test(test_batch_noparam_lang),
      cmocka_unit_test(test_batch_noparam_subs),
      cmocka_unit_test(test_batch_noparam_fnt),
      cmocka_unit_test(test_batch_noparam_asrate),
      cmocka_unit_test(test_batch_noparam_abrate),
      cmocka_unit_test(test_batch_noparam_vbrate),
      cmocka_unit_test(test_batch_noparam_maxbrate),
      cmocka_unit_test(test_batch_noparam_all),
      cmocka_unit_test(test_batch_badparam),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
