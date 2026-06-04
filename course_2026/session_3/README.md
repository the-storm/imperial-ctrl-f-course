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

You do not need to run any fuzzing commands before the session.
