# Session 1 Practical

## Objectives

Get hands-on experience with threat modeling and security review. You are not expected to exploit the applications (you may do this for fun). Focus on security review of the whole application, identifying attack surface and documeting your findings.

---

## Environment Setup

You need: **Git**, **Docker**, and optionally an IDE (VS Code is highly recommended).

**Recommended VS Code extension for security review:**
Install [weaudit](https://marketplace.visualstudio.com/items?itemName=trailofbits.weaudit): it lets you annotate code with findings, mark suspicious lines, and export a report.

### Lab 1 — KalmarNotes

```bash
cd tm_kalmarnotes/kalmarnotes_level1
docker compose up -d

# to stop
docker compose down
```

App is available at **http://localhost:11080**, change the port at `docker-compose.yml` if needed. Code available at `src/`.

### Lab 2 — Web Image Gallery

```bash
cd mini_course_2025/session_1/web_ImageGallery1/challenge
docker compose up -d
# to stop
docker compose down
```

App is available at **http://localhost:12080**

### Lab 3 — Structurizr 

```
cd structurizr
docker compose up -d
# to stop
docker compose down
```

App is available at **http://localhost:8080**

### Hedgedoc
ToDo