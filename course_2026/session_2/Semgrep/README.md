# Kalmarnotes Semgrep Lab

This directory contains a Dockerized Semgrep setup, custom Semgrep rules, and lab guides for the Kalmarnotes exercises.

The container mounts:

```text
/levels         original Kalmarnotes levels
/custom_level1  single-file taint-flow teaching demo
/workspace      this Semgrep lab directory
```

## Files

```text
Exercise_guide.md     student-facing lab guide
Solution_manual.md    instructor solution manual
rules/                custom Semgrep rules
custom_level1/        small single-file taint demo for Level 1
```

## Setup

Build the scanner image:

```sh
docker compose build
```

Validate the lab rules:

```sh
docker compose run --rm semgrep scan --validate --config /workspace/rules/level1.yml /levels/kalmarnotes_level1
docker compose run --rm semgrep scan --validate --config /workspace/rules/level2.yml /levels/kalmarnotes_level2
docker compose run --rm semgrep scan --validate --config /workspace/rules/level3.yml /levels/kalmarnotes_level3
```

## Common Commands

Run the student lab rules level by level:

```sh
docker compose run --rm semgrep scan --config /workspace/rules/level1.yml --dataflow-traces /levels/kalmarnotes_level1
docker compose run --rm semgrep scan --config /workspace/rules/level2.yml /levels/kalmarnotes_level2
docker compose run --rm semgrep scan --config /workspace/rules/level3.yml /levels/kalmarnotes_level3
```

Run the single-file taint-flow demo:

```sh
docker compose run --rm semgrep scan --config /workspace/rules/level1_custom_taint.yml --dataflow-traces /custom_level1
```

Run Semgrep's broader community rules against all levels:

```sh
docker compose run --rm semgrep
```
