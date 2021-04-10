#include "weebfiles.h"
#include "weebtext.h"
#include "weebtypes.h"

#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

//Make portable later
#if defined __linux__
  #include <linux/limits.h>  // for PATH_MAX
#elif ! defined PATH_MAX
  #define PATH_MAX _POSIX_PATH_MAX
#endif

extern int errno;

int main(int argc, char *argv[]) {
  batch_t batches[256];  // TODO: find way to keep this full while threads go brrrrr
  //size_t nbatches;
  int verbosity = 0;
  int outdirs = 0;
  int indirs = 0;
  bool default_extract_fonts = false;
  bool global_disable_subs = false;
  bool sub_default = false;
  char global_lang[_POSIX_ARG_MAX] = "eng";

  {
    char **argp = argv;
    int argi = 0;
    while(argi < argc) {
      if((strncmp("-o", *argp, 3) == 0) || (strncmp("--o", *argp, 4) == 0)
         || (strncmp("--out-dir", *argp, 10) == 0)) {
        outdirs++;
        if((outdirs > 1) && (outdirs != indirs)) {
          die("Parameter error!!! Must have either one shared output directory or "
              "an output directory for every input directory!\n", 1);
        }
      } else if((strncmp("-i", *argp, 3) == 0) || (strncmp("--i", *argp, 4) == 0)
         || (strncmp("--in-dir", *argp, 9) == 0)) {
        indirs++;
        if((outdirs > 1) && (outdirs != indirs)) {
          die("Parameter error!!! Must have either one shared output directory or "
              "an output directory for every input directory!\n", 1);
        }
      }
      argi++;
      argp++;
    }
    if((outdirs > 1) && (outdirs != indirs)) {
      die("Parameter error!!! Must have either one shared output directory or "
          "an output directory for every input directory!\n", 1);
    }
  }
  {
    char batch_file[PATH_MAX] = { 0 };
    FILE *bf = NULL;

    const struct option long_opts[] = {
      {"extract-fonts", no_argument, NULL, 'e'},
      {"input-dir", required_argument, NULL, 'i'},
      {"out-dir", required_argument, NULL, 'o'},
      {"batch-file", required_argument, NULL, 'B'},
      {"burn-subtitles", no_argument, NULL, 's'},
      {"no-subtitles", no_argument, NULL, 'n'},
      {"lang", required_argument, NULL, 'l'},
      {"help", optional_argument, NULL, 'h'},
      {"dry-run", no_argument, NULL, 'd'},  // on call stack
      {"verbose", optional_argument, NULL, 'v'}, // on call stack
      {0, 0, 0, 0}
    };

    {
      int oidx = 0;
      batch_t *fillerup = batches;

      char *in_dirs[indirs];
      char *out_dirs[outdirs];
      char **in_ptr = in_dirs;
      char **out_ptr = out_dirs;

      while(1) {
        int opt = 0;
        opt = getopt_long(argc, argv, "ei:o:B:snl:h::dv::", long_opts, &oidx);
        if(opt == -1)
          break;

        switch(opt) {
          case 'e': {
            default_extract_fonts = true;
            break;
          } case 's': {
            sub_default = true;
            break;
          } case 'n': {
            global_disable_subs = true;
            sub_default = false;
            break;
          } case 'i': {
            strncpy(fillerup->in_dir, optarg, sizeof(fillerup->in_dir));
            if (!opendir(fillerup->in_dir)) {
              die("Can't open input directory \"%s\", got error code %d: %s!\n",
                  errno, fillerup->in_dir, errno, strerror(errno));
            }
            break;
          } case 'l': {
            strncpy(global_lang, optarg, sizeof(global_lang));
            break;
          } case 'h': {
            // TODO: handle help topics
            USAGE_PRINT;
            break;
          } case 'B': {
            strncpy(batch_file, optarg, sizeof(batch_file));
            if (strncmp("-", optarg, _POSIX_ARG_MAX) == 0) {
              bf = stdin;
            } else {
              bf = fopen(batch_file, "r");
            }
            if (bf == NULL)
              die("Can't open batch file \"%s\", got error code %d: %s!\n",
                  errno, batch_file, errno, strerror(errno));
            break;
          } case 'o': {
            strncpy(fillerup->out_dir, optarg, sizeof(fillerup->out_dir));
            if (!opendir(fillerup->out_dir)) {
              die("Can't open output directory \"%s\", got error code %d: %s!\n",
                  errno, fillerup->out_dir, errno, strerror(errno));
            }
            break;
          } case '?': {
            fprintf(stderr, "Invalid argument #%d!\n\n", oidx);
            USAGE_DIE(1);
          } default:
            break;
        }
      }
    }
    if(sub_default && global_disable_subs)
      die("Invalid parameters!  Do you want hardsubs or not??\n", 1);
  }

  return HAPPY_WEEB;
}
