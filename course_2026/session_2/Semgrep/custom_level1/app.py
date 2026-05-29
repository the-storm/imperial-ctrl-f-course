from flask import Flask, redirect, render_template, session
import sqlite3
from contextlib import closing


app = Flask(__name__)
app.secret_key = "dev-only"


def authenticated_only(f):
    def decorated_function(*args, **kwargs):
        if "user_id" not in session:
            return redirect("/")
        return f(*args, **kwargs)
    return decorated_function


def connect_db():
    return sqlite3.connect("notes.db")


def get_note_by_id(note_id, user_id):
    with closing(connect_db()) as db:
        cursor = db.execute(
            """
            SELECT id, title, content, user_id FROM notes WHERE id =
            """ + note_id,
        )
        row = cursor.fetchone()
        if row and row[3] == user_id:
            return {"id": row[0], "title": row[1], "content": row[2], "user_id": row[3]}
        return None


@app.route("/note/<string:note_id>/<string:view_type>")
@authenticated_only
def view_note(note_id, view_type):
    note = get_note_by_id(note_id, session.get("user_id"))
    if not note:
        return redirect("/")
    return render_template("view_note_long.html", note=note)


@app.route("/ce-demo/<string:note_id>")
@authenticated_only
def ce_demo(note_id):
    def local_get_note_by_id(user_id):
        with closing(connect_db()) as db:
            cursor = db.execute(
                """
                SELECT id, title, content, user_id FROM notes WHERE id =
                """ + note_id,
            )
            row = cursor.fetchone()
            if row and row[3] == user_id:
                return {"id": row[0], "title": row[1], "content": row[2], "user_id": row[3]}
            return None

    note = local_get_note_by_id(session.get("user_id"))
    if not note:
        return redirect("/")
    return render_template("view_note_long.html", note=note)
