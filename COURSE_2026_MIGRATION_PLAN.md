# Course 2026 Patch Extraction Plan

## Summary
Create a new working branch based on `ee61ab0af118a038cf5e3a73e98eb5e42e52d23f`, copy the tracked contents of `mini_course_2025` into a new `course_2026` tree, then apply the `ee61ab0 -> fae74f0` course-content changes under `course_2026` instead of changing `mini_course_2025`.

## Key Changes
- Preserve `main` as-is and create a new branch from `ee61ab0`, e.g. `course-2026-from-ee61`.
- Copy `mini_course_2025` to `course_2026` using Git tree/index operations so tracked files and submodule gitlinks are preserved cleanly.
- Extract only the `mini_course_2025/**` portion of the diff between `ee61ab0` and `fae74f0`, rewrite paths from `mini_course_2025/` to `course_2026/`, and apply it to the new tree.
- Do not apply the root `README.md` changes from the original commit range.
- Update `.gitmodules` by adding `course_2026` submodule entries for the copied submodules; use the HTTPS Structurizr URLs from the patch for:
  - `course_2026/session_1/structurizr/structurizr-onpremises`
  - `course_2026/session_1/structurizr/structurizr-ui`
- Keep existing `mini_course_2025` files and `.gitmodules` entries intact.

## Execution Steps
1. Confirm the worktree is clean with `git status --short`.
2. Create the branch: `git switch -c course-2026-from-ee61 ee61ab0af118a038cf5e3a73e98eb5e42e52d23f`.
3. Write this plan to `COURSE_2026_MIGRATION_PLAN.md`.
4. Copy the course tree with `git read-tree --prefix=course_2026/ -u HEAD:mini_course_2025`.
5. Generate a temporary patch from `git diff --binary ee61ab0af118a038cf5e3a73e98eb5e42e52d23f fae74f0dd4818d1f6a248871954f71c3ea53dfc2 -- mini_course_2025`, rewriting `mini_course_2025/` to `course_2026/`.
6. Apply the rewritten patch with `git apply --index`.
7. Manually update `.gitmodules` to add `course_2026` submodule entries matching the copied submodule paths, with Structurizr URLs changed to HTTPS.
8. Review the resulting diff and commit only after verification passes.

## Test Plan
- Run `git diff --check` to catch whitespace or patch formatting issues.
- Run `git status --short` and verify the only intended changes are `COURSE_2026_MIGRATION_PLAN.md`, `course_2026/**`, and `.gitmodules`.
- Run `git diff --stat` and confirm the `course_2026` changes mirror the original course-content changes from `ee61ab0 -> fae74f0`.
- Run `git submodule status` and verify copied `course_2026` submodule paths are recognized.
- Optionally validate Docker Compose files with `docker compose config` in the changed lab directories under `course_2026/session_1`.

## Assumptions
- The current `main` branch should not be reset or rewritten.
- Root `README.md` changes from the original commit range are intentionally excluded.
- `course_2026` should start as a full copy of `mini_course_2025` from `ee61ab0`, then receive only the transformed course-content patch.
- The plan file will be created at repo root as `COURSE_2026_MIGRATION_PLAN.md`.
