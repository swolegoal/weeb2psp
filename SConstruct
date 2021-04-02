from sconutils import runpretty, bldlexrs

env = Environment()
env.Append(CCFLAGS="-O2 -Wall -Wextra")
env.Append(LIBPATH=["/usr/lib", "/usr/local/lib", "build"])

dbg = ARGUMENTS.get("debug", 0)
afl = ARGUMENTS.get("afl", 0)
iniprint = ARGUMENTS.get("iniprint", 0)
Repository("/usr/include", "src")

if int(afl):
    dbg = "1"
    env.Replace(CC="afl-gcc")

if int(dbg):
    env.Append(CCFLAGS="-g")

if int(iniprint):
    env.Append(CPPDEFINES=["INI_PRINT"])

# Build C lexer state machines
bldlexrs()

# Components
parser_src = ["src/weebparse.c", "src/weebutil.c"]
parsebed_src = ["src/parsebed.c"] + parser_src

# Program Targets
if (
    "build/parsebed" in COMMAND_LINE_TARGETS
    or "parsebed" in COMMAND_LINE_TARGETS
):
    env.Alias("parsebed", ["build/parsebed"])
    parsebed = env.Program(target="build/parsebed", source=parsebed_src)

elif "mocka" in COMMAND_LINE_TARGETS:
    env.Append(LIBS="cmocka")
    env.Append(CCFLAGS="-g")
    mockatests = [
        "src/t/test_cmocka.t",
        "src/t/test_weebparse_good1.t",
        "src/t/test_weebparse_good2.t",
        "src/t/test_weebparse_badfiles.t",
    ]
    env.Alias("mocka", mockatests)

    lextest_defines = ["MOCKA_TESTVARS"]
    if "CPP_DEFINES" in env:
        lextest_defines += env["CPP_DEFINES"]
    printwrap_def = lextest_defines + ["INI_PRINT"]

    twg1_env = env.Clone(
        CFLAGS=env["CCFLAGS"],
        CPPDEFINES=printwrap_def + ["TEST_WEEBPARSE_GOOD1"],
        OBJPREFIX="twg1_",
    )
    test_weebparse_good1 = twg1_env.Program(
        target="src/t/test_weebparse_good1.t",
        source=parser_src + ["src/t/test_weebparse.c"],
    )

    twg2_env = env.Clone(
        CFLAGS=env["CCFLAGS"],
        CPPDEFINES=printwrap_def + ["TEST_WEEBPARSE_GOOD2"],
        OBJPREFIX="twg2_",
    )
    test_weebparse_good2 = twg2_env.Program(
        target="src/t/test_weebparse_good2.t",
        source=parser_src + ["src/t/test_weebparse.c"],
    )

    test_weebparse_badfiles = env.Program(
        target="src/t/test_weebparse_badfiles.t",
        source=parser_src + ["src/t/test_weebparse_badfiles.c"],
        CFLAGS=env["CCFLAGS"],
    )

    test_cmocka = env.Program(
        target="src/t/test_cmocka.t", source=["src/t/test_cmocka.c"]
    )
else:
    env.Alias("weeb2psp", ["build/weeb2psp"])
    weeb2psp = env.Program(
        target="build/weeb2psp",
        source=["src/weeb2psp.c", "src/weebutil.c", "src/weebfiles.c"],
    )

Default("build/weeb2psp")
