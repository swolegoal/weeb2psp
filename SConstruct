import sconutils

env = Environment()
env.Append(CCFLAGS="-O2")

env.Program(
    target="build/weeb2psp",
    source=["src/weeb2psp.c", "src/weebutil.c", "src/weebfiles.c"],
)
