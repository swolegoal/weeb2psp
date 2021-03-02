#include "weebtypes.h"
#include "weebutil.h"

#define USAGE \
  "  %s [OPTION]\n" \
  "\n" \
  "%s is a command that utilizes FFmpeg to encode AVC that plays on the PSP's\n" \
  "built-in hardware-accelerated XMB media player in a batch.  If running one\n" \
  "instance of this script doesn't fully-saturate all of your CPU threads, you can\n" \
  "safely spawn as many instances of %s are required to fully-saturate all of\n" \
  "your CPU's threads.  Spawning the instances in separate terminal sessions is\n" \
  "reccommended for logging and sanity purposes.\n" \
  "\n" \
  "This script expects to find valid video in a given input directory.  It then\n" \
  "encodes the video in an ouput directory.  By default, the output directory name\n" \
  "is INPUT_DIRECTORY with the suffix \"_out\" appended to the end.\n" \
  "\n" \
  "If a VARS_FILE is defined with the -B (or --batch-file) flag, INPUT_DIRECTORY\n" \
  "parameter is ignored.\n" \
  "\n" \
  "OPTIONS\n" \
  "  -B BATCH_FILE, --ingest-def=BATCH_FILE\n" \
  "      Source input and output directory names from variables defined in\n" \
  "      BATCH_FILE.  See BATCH_FILE section below for more information.\n" \
  "\n" \
  "  -i INPUT_DIRECTORY, --input-dir=INPUT_DIRECTORY\n" \
  "      Override the derived input directory name with a custom input path.\n" \
  "      (Ignored if -B is used.)\n" \
  "\n" \
  "  -o OUTPUT_DIRECTORY, --out-dir=OUTPUT_DIRECTORY\n" \
  "      Override the derived output directory name with a custom output path.\n" \
  "      (Ignored if -B is used.)\n" \
  "\n" \
  "  -e, --extract-fonts\n" \
  "      Extract all MKV attachments (all platforms), then install any TTF fonts\n" \
  "      found (Linux only) and exit.\n" \
  "\n" \
  "  -s, --subtitles\n" \
  "      Bake in subtitles from the source material as hard subtitles in the output\n" \
  "      video track.\n" \
  "\n" \
  "  -n, --no-subtitles\n" \
  "      Force subtitles to be disabled, no matter what.\n" \
  "\n" \
  "  -l LANG, --lang=LANG\n" \
  "      Specify a language to look for when picking audio and subtitle tracks.\n" \
  "      \"%s\" is the default.\n" \
  "\n" \
  "  -h, --help\n" \
  "      Display this help.\n" \
  "\n" \
  "  -d, --dry-run\n" \
  "      Do a dry run.  Gets -vv info on each file and skips actual processing.\n" \
  "\n" \
  "  -v\n" \
  "      Increase the verbosity level.\n" \
  "\n" \
  "BATCH_FILE\n" \
  "  The BATCH_FILE is a simple INI-like config file that has variables defined for\n" \
  "  batches to be converted.\n" \
  "\n" \
  "  SUPPORTED PARAMETERS\n" \
  "    - in_dir: A string path\n" \
  "    - out_dir (optional)\n" \
  "\n" \
  "  SYNTAX\n" \
  "      [BATCH NAME]
  "\n" \
  "  EXAMPLE\n" \
  "\n" \
  "      [Initial D]\n" \
  "      in_dir = \"batches/initiald/in\"\n" \
  "      out_dir = \"batches/initiald/out\"\n" \
  "      lang = \"eng\"\n" \
  "      hardsubs = false\n" \
  "\n" \
  "      [JoJo's Bizarre Adventure Seasons 1-5]\n" \
  "      in_dir = \"batches/jojo/in\"\n" \
  "      out_dir = \"batches/jojo/out\"\n" \
  "      lang = \"eng\"\n" \
  "      hardsubs = true\n" \
  "      extract_fonts = true\n"

#define USAGE_DIE(stat) \
  { \
            char *cmd = strrchr(argv[0], '/'); \
            cmd = cmd ? cmd + 1 : argv[0]; \
            die(USAGE, stat, cmd, cmd, cmd, global_lang); \
  }

#define USAGE_PRINT \
  { \
            char *cmd = strrchr(argv[0], '/'); \
            cmd = cmd ? cmd + 1 : argv[0]; \
            printf(USAGE, cmd, cmd, cmd, global_lang); \
            exit(HAPPY_WEEB); \
  }
