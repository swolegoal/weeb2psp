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
if "build/parsebed" in COMMAND_LINE_TARGETS:
    parsebed = env.Program(target="build/parsebed", source=parsebed_src)

elif "mocka" in COMMAND_LINE_TARGETS:
    env.Append(LIBS="cmocka")
    mockatests = ["src/t/test_cmocka.t", "src/t/test_weebparse.t"]
    env.Alias("mocka", mockatests)

    parsetest_src = parser_src + ["src/t/test_weebparse.c"]
    test_weebparse = env.Program(
        target="src/t/test_weebparse.t",
        source=parsetest_src,
        CFLAGS=env["CCFLAGS"] + ["-Wl,--wrap=print_batch_inistyle"],
    )

    test_cmocka = env.Program(
        target="src/t/test_cmocka.t", source=["src/t/test_cmocka.c"]
    )
else:
    weeb2psp = env.Program(
        target="build/weeb2psp",
        source=["src/weeb2psp.c", "src/weebutil.c", "src/weebfiles.c"],
    )

Default("build/weeb2psp")
