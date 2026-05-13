# Session 1 Practical

See the exercise sheet for the practical details. Below are the instructions to run the labs.

### Lab 1: KalmarNotes

Each level runs the same way. From its directory, `docker compose up -d` to start and `docker compose down` to stop. Source code is under `src/`.

| Level | Path | Port |
|---|---|---|
| 0 | `kalmarnotes/kalmarnotes_level0` | 6002 |
| 1 | `kalmarnotes/kalmarnotes_level1` | 6003 |
| 2 | `kalmarnotes/kalmarnotes_level2` | 6004 |
| 3 | `kalmarnotes/kalmarnotes_level3` | 6005 |

### Lab 2: Web Image Gallery

​```bash
cd course_2026/session_1/web_ImageGallery1/challenge
docker compose up -d
# to stop
docker compose down
​```

App at **http://localhost:12080**.

### Lab 3: Structurizr

​```bash
cd course_2026/session_1/structurizr
docker compose up -d
# to stop
docker compose down
​```

App at **http://localhost:8080**.

### Lab 4: HedgeDoc

No local setup, navigate the upstream repo yourself. See the exercise sheet.