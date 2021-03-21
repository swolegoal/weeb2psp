#!/usr/bin/env bash

git_root="$(git rev-parse --show-toplevel)"
. util/git
. util/bashtap

plan 1
diag Checking if you have a working TAP harness...
ok "Basic Bash TAP harness test" true
