# Session 3 Pre-Session Setup

Complete this before the fuzzing session.

## Requirement

Make sure Docker Desktop or Docker Engine is installed and running.

## Build the AFL++ Image

From the repository root:

```bash
cd course_2026/session_3
docker compose -f docker-compose.pre-session.yml build
```

This prepares the AFL++ Docker image used in the session:

```text
imperial-fuzzing-aflplusplus-2026:pre-session
```

## Check the Image

Run this quick check:

```bash
docker compose -f docker-compose.pre-session.yml run --rm aflplusplus bash -lc 'afl-fuzz -V 1 2>&1 | head -n 5 && afl-clang-fast++ --version | head -n 1'
```

The command should print AFL++ startup/version output and a Clang version line. It is okay if AFL++ exits after printing usage or startup text; this check only confirms the container starts and the AFL++ tools are available.

You do not need to run any fuzzing commands before the session.
