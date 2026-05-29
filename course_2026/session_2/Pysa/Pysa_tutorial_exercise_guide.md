# Pysa Tutorial Exercise Guide

This guide covers the upstream Pysa tutorial mounted from the pinned
`pyre-check` submodule. Students should keep tutorial solution models outside
the submodule in a local `models/` directory under this `Pysa` lab directory.

## Setup

Initialize the submodule after cloning the course repository:

```bash
git submodule update --init --recursive course_2026/session_2/Pysa/pyre-check
```

If you are already in the `Pysa` lab directory, the shorter command below is
equivalent:

```bash
git submodule update --init --recursive pyre-check
```

Avoid running a repository-wide submodule update unless you need every course
submodule; that can fetch unrelated materials and makes setup slower.

Build and enter the lab container:

```bash
docker compose build
docker compose run --rm pysa
```

The first build compiles Pyre from source and can take several minutes. Long
quiet phases during the Git clone, OPAM setup, or OCaml build are normal.

Mounted paths:

```text
/workspace/pyre-check/documentation/pysa_tutorial
/workspace/pysa-models
```

The `docker-compose.yml` file may also mount Kalmar course materials using a
host-specific absolute path. That mount is not needed for this upstream Pysa
tutorial. If Docker reports that the Kalmar host path does not exist, either
create the path, remove that volume for this tutorial, or replace it with your
local Kalmar notes path.

Create a model directory for each exercise as you solve it:

```bash
mkdir -p /workspace/pysa-models/tutorial/exercise1
mkdir -p /workspace/pysa-models/tutorial/exercise2
mkdir -p /workspace/pysa-models/tutorial/exercise3
mkdir -p /workspace/pysa-models/tutorial/exercise4
mkdir -p /workspace/pysa-models/tutorial/exercise5
```

The host-side path is `Pysa/models/tutorial/exerciseN`. This directory is local
lab work; it is not part of the upstream tutorial submodule.

Each exercise has its own `.pyre_configuration`. For exercises 1 and 2, point
`taint_models_path` at only the exercise's local model directory:

```json
"taint_models_path": [
  "/workspace/pysa-models/tutorial/exerciseN"
]
```

For exercises 3 through 5, keep the upstream taint stubs and add the local model
directory:

```json
"taint_models_path": [
  "../../../stubs/taint/",
  "/workspace/pysa-models/tutorial/exerciseN"
]
```

For exercises that use the upstream taint stubs, run Pysa with `--no-verify`.
The flag lets analysis continue despite version skew between the upstream Django
taint models and the installed Django stubs. Depending on the Pyre version, you
may still see messages such as `Found 21 model verification errors!`; this is
expected for exercises 3 through 5 as long as the command exits successfully and
the final issue count matches the exercise.

## Exercise 1

Start in `exercise1`:

```bash
cd /workspace/pyre-check/documentation/pysa_tutorial/exercise1
```

Create `/workspace/pysa-models/tutorial/exercise1/sources_sinks.pysa`:

```python
django.http.request.HttpRequest.GET: TaintSource[CustomUserControlled] = ...

def eval(__source: TaintSink[CodeExecution], __globals, __locals): ...
```

Create `/workspace/pysa-models/tutorial/exercise1/taint.config` with
`CustomUserControlled`, `CodeExecution`, and the RCE rule from the exercise.
Update `.pyre_configuration` so `taint_models_path` points at
`/workspace/pysa-models/tutorial/exercise1`.

Run:

```bash
pyre analyze
```

Expected result: one issue in `views.operate_on_twos`.

## Exercise 2

Start in `exercise2`. Write the complete solved models in
`/workspace/pysa-models/tutorial/exercise2`.

`sources_sinks.pysa` should model `GET`, `POST`, `eval`, `exec`, and
`subprocess.getoutput`:

```python
django.http.request.HttpRequest.GET: TaintSource[CustomUserControlled] = ...
django.http.request.HttpRequest.POST: TaintSource[CustomUserControlled] = ...

def eval(__source: TaintSink[CodeExecution], __globals, __locals): ...
def exec(__source: TaintSink[CodeExecution], __globals, __locals): ...
def subprocess.getoutput(cmd: TaintSink[ShellExecution]): ...
```

`taint.config` should contain the original RCE rule plus a second rule for
`CustomUserControlled` data reaching `ShellExecution`.

Update `.pyre_configuration` so `taint_models_path` points at
`/workspace/pysa-models/tutorial/exercise2`, then run:

```bash
pyre analyze
```

Expected result: three issues.

## Exercise 3

Start in `exercise3`. Keep the upstream taint stubs in `taint_models_path` and
add `/workspace/pysa-models/tutorial/exercise3`.

Create `/workspace/pysa-models/tutorial/exercise3/sanitizers.pysa`:

```python
@Sanitize
def views.get_operator_safe(request): ...

@Sanitize
def views.sanitized_operator(operator): ...
```

Add an identity helper in `views.py`:

```python
def sanitized_operator(operator: str) -> str:
    return operator
```

Call it in `operate_on_threes` after the assertion:

```python
operator = sanitized_operator(operator)
```

Run:

```bash
pyre analyze --no-verify
```

Expected result: zero issues. It is OK if Pyre prints upstream Django model
verification messages before the final empty JSON list.

## Exercise 4

Start in `exercise4`. Keep the upstream taint stubs in `taint_models_path` and
add `/workspace/pysa-models/tutorial/exercise4`.

Create `/workspace/pysa-models/tutorial/exercise4/taint.config` with the custom
feature:

```json
{
  "sources": [],
  "sinks": [],
  "features": [
    {
      "name": "assert_numeric",
      "comment": "operand was checked with assert_numeric"
    }
  ],
  "rules": [],
  "maximum_overrides_to_analyze": 30
}
```

Create `/workspace/pysa-models/tutorial/exercise4/features.pysa`:

```python
def views.assert_numeric(operand: AddFeatureToArgument[Via[assert_numeric]]): ...
```

Run:

```bash
pyre analyze --no-verify --save-results-to .
sapp analyze taint-output.json
sapp explore
```

`--save-results-to .` writes analysis artifacts into the exercise directory,
including files such as `taint-output.json`, `modules.json`, call graph JSON
files, and `sapp.db`. These files are generated output and can be deleted when
you are done exploring the results.

Filter the two false-positive features in SAPP:

```text
issues(exclude_features=["always-type:bool", "always-via:assert_numeric"])
```

Expected result: zero displayed issues.

SAPP may print SQLAlchemy cache warnings in this container. They are harmless
for the tutorial if the `issues(...)` query returns the expected number of
displayed issues.

## Exercise 5

Start in `exercise5`. Keep the upstream taint stubs in `taint_models_path` and
add `/workspace/pysa-models/tutorial/exercise5`.

Generate the Django path-parameter model into the local model directory:

```bash
python3 generate_models.py --output-directory /workspace/pysa-models/tutorial/exercise5
cat /workspace/pysa-models/tutorial/exercise5/generated_django_path_params.pysa
pyre analyze --no-verify
```

Expected result: one issue in `views.operate_on_twos`. As in the previous
exercises, upstream Django model verification messages are expected noise if the
analysis exits successfully.

Then complete the decorator generator in `generate_models.py`:

```python
"decorator_extracted_params": generate_taint_models.AnnotatedFreeFunctionWithDecoratorGenerator(
    root=".",
    annotation_specifications=[
        generate_taint_models.DecoratorAnnotationSpecification(
            decorator="@api_wrapper",
            annotations=generator_specifications.default_entrypoint_taint,
        )
    ],
),
```

Also add the mode:

```python
"decorator_extracted_params",
```

Regenerate into the local model directory:

```bash
python3 generate_models.py --output-directory /workspace/pysa-models/tutorial/exercise5
cat /workspace/pysa-models/tutorial/exercise5/generated_decorator_extracted_params.pysa
pyre analyze --no-verify
```

Expected result: two issues, one for `operate_on_twos` and one for
`operate_on_threes`.
