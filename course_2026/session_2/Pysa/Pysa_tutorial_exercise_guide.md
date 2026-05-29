# Pysa Tutorial Exercise Guide

This guide covers the upstream Pysa tutorial mounted from the pinned
`pyre-check` submodule. Students should keep tutorial solution models outside
the submodule in a local `models/` directory under this `Pysa` lab directory.

## Setup

Initialize the submodule after cloning the course repository:

```bash
git submodule update --init --recursive
```

Build and enter the lab container:

```bash
docker compose build
docker compose run --rm pysa
```

Mounted paths:

```text
/workspace/pyre-check/documentation/pysa_tutorial
/workspace/pysa-models
```

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
The flag suppresses model-verification noise from version skew between the
upstream Django taint models and the installed Django stubs.

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

Expected result: zero issues.

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

Filter the two false-positive features in SAPP:

```text
issues(exclude_features=["always-type:bool", "always-via:assert_numeric"])
```

Expected result: zero displayed issues.

## Exercise 5

Start in `exercise5`. Keep the upstream taint stubs in `taint_models_path` and
add `/workspace/pysa-models/tutorial/exercise5`.

Generate the Django path-parameter model into the local model directory:

```bash
python3 generate_models.py --output-directory /workspace/pysa-models/tutorial/exercise5
cat /workspace/pysa-models/tutorial/exercise5/generated_django_path_params.pysa
pyre analyze --no-verify
```

Expected result: one issue in `views.operate_on_twos`.

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
