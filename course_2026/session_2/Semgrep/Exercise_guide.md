# Kalmarnotes Semgrep Exercise Guide

This guide is for students. It gives direction for solving the lab with Semgrep without fully spelling out the solution.

## Goal

Use Semgrep to identify the main vulnerability in each level, inspect the affected code, make a fix, and run Semgrep again to confirm the finding is gone.

You will work on:

```text
Level 1: SQL injection
Level 2: stored XSS
Level 3: stored XSS plus unsafe caching
```

Level 0 is skipped because it does not add a meaningfully different Semgrep workflow from Level 1.

## Setup

From this directory:

```sh
cd course_2026/session_2/Semgrep
```

Build or refresh the Semgrep container:

```sh
docker compose build
```

The lab source code is mounted in the Semgrep container at:

```text
/levels
```

In this checkout, `docker-compose.yml` mounts the complete Kalmarnotes source tree from:

```text
../../session_1/in-class/kalmarnotes
```

If `/levels/kalmarnotes_level1/src/app.py` is missing inside the container, check the mount path before debugging Semgrep rules.

The custom rules are mounted at:

```text
/workspace/rules
```

Validate the rules before starting:

```sh
docker compose run --rm semgrep scan --validate --config /workspace/rules/level1.yml /levels/kalmarnotes_level1
docker compose run --rm semgrep scan --validate --config /workspace/rules/level2.yml /levels/kalmarnotes_level2
docker compose run --rm semgrep scan --validate --config /workspace/rules/level3.yml /levels/kalmarnotes_level3
```

Run these validation commands one at a time. Running several `docker compose run` commands in parallel can race while Compose creates its default network.

## How To Work

For each level:

1. Run the Semgrep rule for that level.
2. Read the reported file and line.
3. Decide whether the finding is a source, a sink, or supporting context.
4. Trace how attacker-controlled data reaches the sink.
5. Patch the vulnerable code.
6. Run the same Semgrep command again.
7. Confirm the important finding disappeared.

Some rules are intentionally noisy. A noisy rule is not wrong if it points you toward code that needs human review.

## Level 1: SQL Injection

Run:

```sh
docker compose run --rm semgrep scan \
  --config /workspace/rules/level1.yml \
  --dataflow-traces \
  /levels/kalmarnotes_level1
```

You should see two kinds of findings:

```text
kalmarnotes.flask-route-param-to-db-helper
kalmarnotes.function-param-to-sql-execute
```

The first rule is a taint rule from route parameters to database helper calls. It shows where attacker-controlled route data leaves the Flask handler.

The second rule is a taint rule from a function parameter to an unsafe SQL execution expression. It shows where a parameter reaches string-built SQL.

Open-source Semgrep gives these as two local traces. A single route-parameter-to-SQL trace across `app.py` and `db.py` requires interprocedural/interfile analysis.

Questions to answer:

- Which route accepts a note identifier from the URL?
- Where does that route send the note identifier?
- Which database helper eventually builds SQL?
- Is the SQL query using placeholders, or is it assembled as a string?
- Does the query verify that the note belongs to the current user?

Fix direction:

- Use a parameterized SQL query.
- Bind user-controlled values as query parameters.
- Keep authorization close to the data lookup.

After patching, rerun:

```sh
docker compose run --rm semgrep scan \
  --config /workspace/rules/level1.yml \
  --dataflow-traces \
  /levels/kalmarnotes_level1
```

Checkpoint: the `function-param-to-sql-execute` finding should be gone. The route-parameter-to-helper findings may remain as review context.

### Optional: Single-File Taint Demo

The original Level 1 splits the Flask route and database helper across modules. Open-source Semgrep does not produce one full route-to-SQL interprocedural trace for that layout. To make the taint concept easier to see, this repo includes a teaching-only single-file variant mounted at:

```text
/custom_level1
```

Run:

```sh
docker compose run --rm semgrep scan \
  --config /workspace/rules/level1_custom_taint.yml \
  --dataflow-traces \
  /custom_level1
```

Look for:

```text
kalmarnotes.custom-level1-route-param-to-sql
```

This demonstrates a Flask route parameter reaching a SQL execution sink in one file. The helper is kept local to the route so the open-source Semgrep engine can show the taint flow clearly. The rule is in:

```text
rules/level1_custom_taint.yml
```

For full interprocedural analysis from `app.py` into `db.py` in the original app, use Semgrep Pro/interfile analysis.

## Level 2: Stored XSS

Run:

```sh
docker compose run --rm semgrep scan \
  --config /workspace/rules/level2.yml \
  /levels/kalmarnotes_level2
```

The main rule is:

```text
kalmarnotes.jinja-safe-on-user-controlled-note-data
```

This rule looks for Jinja values marked with `| safe`. In templates, `| safe` disables escaping. That is dangerous when the value can contain user-controlled data.

The rule may report both the long and short note templates. Treat all `| safe` uses in note-rendering templates as suspicious, even when one finding is less directly exploitable than another.

Questions to answer:

- Which template renders the long note view?
- Does the short note template repeat the same rendering pattern?
- Which fields are rendered with `| safe`?
- Which of those fields can an attacker control?
- Is the username rendered into the page?
- What happens if a username contains HTML or JavaScript?

Fix direction:

- Remove `| safe` from user-controlled values.
- Remove unnecessary `| safe` from IDs rendered into JavaScript calls too; numeric IDs do not need HTML-safe rendering.
- Let Jinja escape values by default.
- Only render HTML as safe if it has first gone through a strict allowlist sanitizer.

After patching, rerun:

```sh
docker compose run --rm semgrep scan \
  --config /workspace/rules/level2.yml \
  /levels/kalmarnotes_level2
```

Checkpoint: the template `| safe` findings should be gone.

## Level 3: XSS Plus Caching

Run:

```sh
docker compose run --rm semgrep scan \
  --config /workspace/rules/level3.yml \
  /levels/kalmarnotes_level3
```

You should see findings in three areas:

```text
kalmarnotes.jinja-safe-on-user-controlled-note-data
kalmarnotes.level3-dynamic-note-route-allows-static-extension
kalmarnotes.level3-varnish-caches-by-static-extension
```

The XSS finding is the same class of issue as Level 2.

The route finding points to a dynamic route with a string path component. This is review context: ask whether unexpected suffixes such as `.css` or `.js` can still reach a dynamic HTML view.

The Varnish findings point to caching based on static-looking file extensions.

Questions to answer:

- Can a note URL ending in `.css` still reach the Flask note view?
- Does Flask reject unknown view types?
- Does the cache key vary by cookie or user?
- Does Varnish cache any URL ending in `.css`, `.js`, `.png`, or `.gif`?
- Could one user's rendered page be cached and served to another user?

Fix direction:

- Fix the XSS as in Level 2.
- Reject unknown note view types.
- Do not cache authenticated dynamic pages.
- Restrict caching to a real static path such as `/static/...`.
- Pass requests with cookies through to the backend instead of caching them.

After patching, rerun:

```sh
docker compose run --rm semgrep scan \
  --config /workspace/rules/level3.yml \
  /levels/kalmarnotes_level3
```

Checkpoint:

- The template `| safe` findings should be gone.
- The Varnish static-extension caching findings should be gone.
- The dynamic-route finding should be gone if unknown view types no longer render an HTML note page.
- The generic string-route finding may remain if the route still uses a string converter. Treat it as review noise if the route now explicitly rejects unsafe view types.

## Final Check

Run the combined rules:

```sh
docker compose run --rm semgrep scan \
  --config /workspace/rules/kalmarnotes.yml \
  /levels/kalmarnotes_level1 \
  /levels/kalmarnotes_level2 \
  /levels/kalmarnotes_level3
```

Expected result:

- No SQL string-concatenation sink remains.
- No unsafe `| safe` template rendering remains.
- No Varnish rule caches arbitrary static-looking URLs.
- Generic route/source findings may remain and should be manually reviewed.

## Optional Manual Review

Semgrep is a guide, not a substitute for reading the code. After Semgrep is clean for the important sink findings, manually inspect:

- route handlers in `src/app.py`
- database helpers in `src/db.py`
- templates in `src/templates/`
- Varnish config in `default.vcl`

Make sure each fix addresses the underlying data flow, not only the exact Semgrep pattern.
