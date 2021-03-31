void test_batch_inistyle(const batch_t *batch, int batch_no);

void _ini_asserts(const batch_t *b, const char *pn, const char *idir,
                  const char *odir, const char *ldir, const char *lang,
                  int sub, int fnt, int as, int ab, int vb, int mb);

#if defined TEST_WEEBPARSE_GOOD1
  #define BATCHFILE  "batch-examples/good1.batch"
  #define B_ENTRIES  4

  #define INISTYLE_ASSERTS(b)  {                                               \
      switch (batch_no) {                                                      \
        case 0:                                                                \
          assert_null(b->log_dir);                                             \
          assert_non_null(b->lang);                                            \
          _ini_asserts(b, "Initial D", "batches/initiald/in",                  \
                      "batches/initiald/out", NULL, "eng", 0, -1, 0, 0, 0, 0); \
          break;                                                               \
        case 1:                                                                \
          assert_null(b->log_dir);                                             \
          assert_non_null(b->lang);                                            \
          _ini_asserts(b, "JoJo's Bizarre Adventure Seasons 1-5",              \
                      "batches/jojo/in", "batches/jojo/out", "", "eng", 1, 1,  \
                      0, 0, 0, 0);                                             \
          break;                                                               \
        case 2:                                                                \
          assert_null(b->log_dir);                                             \
          assert_null(b->lang);                                                \
          _ini_asserts(b, "Umami", "batches/umami/in", "batches/umami/out", "",\
                      "", -1, -1, 0, 0, 0, 0);                                 \
          break;                                                               \
        case 3:                                                                \
          assert_non_null(b->log_dir);                                         \
          assert_non_null(b->lang);                                            \
          _ini_asserts(b, "Hunter x Hunter",  "batches/hxh/in",                \
                      "batches/hxh/out", "batches/hxh/log", "eng", 1, 1,       \
                      48000, 320, 3500, 6000);                                 \
          break;                                                               \
        default:                                                               \
          fail_msg("Got to batch_no idx %d, when only %d expected",            \
                   batch_no, (B_ENTRIES - 1));                                 \
          break;                                                               \
      }                                                                        \
    }

// #elif defined TEST_WEEBPARSE_GOOD2
#else
  #error "No print_batch_inistyle injection test defined!!"
#endif
