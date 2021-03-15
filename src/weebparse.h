#include <stdbool.h>

int lex_file(FILE *f, const char *fname);

// Give Scons "print=1" flag to build with DEBUG_PRINT.
#ifdef DEBUG_PRINT
#define DEBUG(x) x
#else
#define DEBUG(x)
#endif

#define TAG_SSLURP(dest, begin, end)  {                           \
    if (dest != NULL) {                                           \
      DEBUG(fprintf(stderr, "[WARN]: Parameter redefinition "     \
                            "on line %u of input!\n", line_no));  \
      free(dest);                                                 \
    }                                                             \
    size_t sz = (size_t)((end - begin) + 1);                      \
    dest = malloc(sz);                                            \
    strncpy(dest, begin, sz - 1);                                 \
    dest[sz - 1] = '\0';                                          \
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
      sscanf((const char *)temp, "%u", &dest); \
      free(temp);                              \
    }                                          \
  }

#define LEX_LOOP  { line_no++; goto lex_loop; }
#define HDR_LOOP  { line_no++; goto hdr_loop; }
