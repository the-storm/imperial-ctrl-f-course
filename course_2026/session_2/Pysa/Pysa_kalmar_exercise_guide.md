# Pysa Kalmar Exercise Guide

This guide solves `kalmarnotes_level1`. Level 0 is intentionally out of scope
for this exercise.

## Files

The exercise uses three local model files:

```text
models/taint.config
models/kalmarnotes_route_sources.pysa
models/sqlite_sinks.pysa
```

`docker-compose.yml` mounts them into the container at `/workspace/pysa-models`.
The Kalmar levels are mounted at `/workspace/kalmarnotes`.

## Goal

Find the SQL injection caused by a Flask route parameter flowing into a SQLite
query.

In level 1, the relevant route is:

```python
@app.route('/note/<string:note_id>/<string:view_type>')
def view_note(note_id, view_type):
    note = db.get_note_by_id(note_id, session.get('user_id'))
```

The vulnerable database code concatenates `note_id` into SQL:

```python
cursor = db.execute('''
    SELECT id, title, content, user_id FROM notes WHERE id =
''' + note_id)
```

## Steps

1. Mark route-handler parameters as sources with a generic model query.

Start with the broad route-decorator model. It finds all functions decorated
with `@app.route(...)` and marks their parameters as user-controlled:

```python
ModelQuery(
  name = "get_kalmarnotes_app_route_sources",
  find = "functions",
  where = Decorator(name.matches(".*route")),
  model = [
    Parameters(TaintSource[UserControlled])
  ]
)
```

Use this broad model first. It avoids needing to know the vulnerable route name
before running Pysa and continues to work if new Flask routes are added.

An optional narrower model for only the known vulnerable route is:

```python
def app.view_note(note_id: TaintSource[UserControlled], view_type): ...
```

The narrower model reports fewer results, but it requires identifying
`app.view_note` before running the analysis.

2. Mark SQLite query execution as the sink.

```python
def sqlite3.Connection.execute(self, sql: TaintSink[SQL], parameters, /): ...
def sqlite3.Cursor.execute(self, sql: TaintSink[SQL], parameters, /): ...
```

3. Configure the SQL rule.

The local `taint.config` defines:

- `UserControlled` as the source.
- `SQL` as the sink.
- `StringMayBeSQL` as an implicit source for SQL-looking string literals.
- A string-combine rule for user input concatenated with SQL text.

4. Run Pysa on level 1.

```bash
docker compose run --rm pysa bash -lc '
cd /workspace/kalmarnotes/kalmarnotes_level1 &&
pyre --source-directory src analyze \
  --taint-models-path /workspace/pysa-models \
  --no-verify
'
```

## Expected Output

With the generic route-source model, level 1 should report two issues:

```text
Found 2 issues
src/app.py:49 SQL injection. [5051]
src/app.py:125 SQL injection. [5051]
```

The `src/app.py:49` report is the target finding. It points at
`view_note` calling `db.get_note_by_id`. Follow that call into `src/db.py` to
inspect the string concatenation and the `sqlite3.execute` sink.

The `src/app.py:125` report comes from the broad route-source model also
marking the API route parameter as user-controlled. That route uses Flask's
`<int:note_id>` converter, so it is less useful as the practical string SQLi
exploit path. Treat it as a review target and focus the exploit on the string
route at `src/app.py:49`.

## Exploit Shape

Level 1 randomizes the admin flag note ID, so do not guess the ID. Query by the
stable title and forge the returned `user_id` column:

```text
/note/0%20UNION%20SELECT%20id,title,content,2%20FROM%20notes%20WHERE%20title='Flag'--/long
```

In a fresh database, user ID `2` is normally the first non-admin user. The
payload returns the admin flag note but replaces the selected `user_id` column
with `2`, so the ownership check inside `get_note_by_id` accepts it.

## Remediation

Fix the SQL injection by parameterizing the query and keeping ownership in the
database predicate:

```python
cursor = db.execute(
    "SELECT id, title, content, user_id FROM notes WHERE id = ? AND user_id = ?",
    (note_id, user_id),
)
```

Parameterization removes attacker control over SQL syntax; the ownership
predicate preserves the intended access-control invariant.
