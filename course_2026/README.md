# CTRL+F Course 2026

Hands-on course materials for practical security review, threat modeling, code auditing, static analysis, and fuzzing.

## Sessions

| Session | Topic | Materials |
| --- | --- | --- |
| 1 | Threat modeling and attack surface review | [session_1](session_1/) |
| 2 | KalmarNotes review labs | [session_2](session_2/) |
| 3 | Static analysis with Pysa and Semgrep | [session_3](session_3/) |
| 4 | Fuzzing module | [session_4](session_4/) |

## Setup

From the repository root, initialize submodules before running labs:

```bash
git submodule update --init --recursive
```

Most labs run with Docker Compose from their challenge directory:

```bash
docker compose up -d
docker compose down
```

See the session README files for exact paths, ports, and lab-specific commands.
