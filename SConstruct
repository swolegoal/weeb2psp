from sconutils import runpretty

env = Environment()
env.Append(CCFLAGS="-O2")

dbg = ARGUMENTS.get("debug", 0)
afl = ARGUMENTS.get("afl", 0)
iniprint = ARGUMENTS.get("iniprint", 0)

if int(afl):
    dbg = "1"
    env.Replace(CC="afl-gcc")

if int(dbg):
    env.Append(CCFLAGS="-g")

if int(iniprint):
    env.Append(CPPDEFINES=["DEBUG_PRINT"])

# Build C lexer state machines
runpretty("re2c -i -W --verbose src/re/weebparse.c -o src/weebparse.c")

# Program Targets
weeb2psp = env.Program(
    target="build/weeb2psp",
    source=["src/weeb2psp.c", "src/weebutil.c", "src/weebfiles.c"],
)

parsebed_src = ["src/parsebed.c", "src/weebparse.c", "src/weebutil.c"]
parsebed = env.Program(target="build/parsebed", source=parsebed_src)

Default("build/weeb2psp")
