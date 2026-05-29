# Pysa Lab

This lab builds Pyre/Pysa from source and runs it against the Kalmar notes and
the upstream Pysa tutorial exercises. It does not install the `pyre-check` PyPI
package.

The upstream `facebook/pyre-check` repository is included as a pinned submodule
so the tutorial files students read are the same files mounted into the
container.

## Setup

After cloning this course repository, initialize the `pyre-check` submodule:

```bash
git submodule update --init --recursive
```

The pinned commit is:

```text
888fabee67b22bc2173f3ab08e88920948a91493
```

Build the lab image:

```bash
docker compose build
```

To test a different Pyre commit, branch, or tag:

```bash
docker compose build --build-arg PYRE_CHECK_REF=<ref>
```

## Container Shell

```bash
docker compose run --rm pysa
```

Mounted paths:

```text
/workspace/kalmarnotes
/workspace/pyre-check/documentation/pysa_tutorial
/workspace/pysa-models
```

## Upstream Pysa Tutorial

Start in the mounted tutorial directory:

```bash
cd /workspace/pyre-check/documentation/pysa_tutorial
```

Use `Pysa_tutorial_solution_manual.md` and
`Pysa_tutorial_exercise_guide.md` for the course-specific walkthrough.


## Kalmar Level 1

Run the level 1 analysis:

```bash
cd /workspace/kalmarnotes/kalmarnotes_level1
pyre --source-directory src analyze \
  --taint-models-path /workspace/pysa-models \
  --no-verify
```

Use `Pysa_kalmar_exercise_guide.md` for the student exercise flow and
`Solution_manual.md` for the level 1 solution notes.

