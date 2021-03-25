#include <stdio.h>

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include "weebparse.h"
#include "weebtypes.h"
#include "weebutil.h"

// static batch_t __wrap_print_batch_inistyle(batch_t *b) {
//   return mock
// }

static void null_test_cmocka(void **state) {
  (void) state;
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(null_test_cmocka),
      //cmocka_unit_test(),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
