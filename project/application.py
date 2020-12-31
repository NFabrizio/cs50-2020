import html
import os

from cs50 import SQL
from flask import Flask, flash, jsonify, redirect, render_template, request, session
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.exceptions import default_exceptions, HTTPException, InternalServerError
from werkzeug.security import check_password_hash, generate_password_hash

from helpers import apology, login_required, lookup, usd

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True

# Ensure responses aren't cached
@app.after_request
def after_request(response):
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response

# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = mkdtemp()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///bookshelf.db")

# Make sure API key is set
# if not os.environ.get("API_KEY"):
#     raise RuntimeError("API_KEY not set")


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""

    user_id = session["user_id"]

    shelfRows = db.execute("SELECT id, shelf_name FROM shelves WHERE user_id = :user_id",
                            user_id=user_id)

    return render_template("index.html", rowLength=len(shelfRows), shelfRows=shelfRows)


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 403)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 403)

        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = :username",
                          username=html.escape(request.form.get("username")))

        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(rows[0]["hash"], request.form.get("password")):
            return apology("invalid username and/or password", 403)

        # Remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to login form
    return redirect("/")


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # Ensure first name was submitted
        if not request.form.get("fname"):
            return apology("must provide first name", 403)

        # Ensure last name was submitted
        if not request.form.get("lname"):
            return apology("must provide last name", 403)

        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 403)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 403)

        # Ensure password confirmation was submitted
        elif not request.form.get("confirmation"):
            return apology("must provide password confirmation", 403)

        # Ensure password and password confirmation match
        elif not request.form.get("password") == request.form.get("confirmation"):
            return apology("password and password confirmation must match", 403)

        # Set variables for easier access
        fname = html.escape(request.form.get("fname"))
        lname = html.escape(request.form.get("lname"))
        username = html.escape(request.form.get("username"))
        passwordHash = generate_password_hash(request.form.get("password"))

        # Ensure username does not already exist in DB
        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = :username",
                          username=username)
        if len(rows) >= 1:
            return apology("username not available", 403)

        # Query database for username
        rows = db.execute("INSERT INTO users (fname, lname, username, hash) VALUES(?, ?, ?, ?)",
                      fname, lname, username, passwordHash)

        # Redirect user to home page
        return redirect("/login")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("register.html")


@app.route("/add-book", methods=["POST"])
@login_required
def addBook():
    """Add book to DB"""

    user_id = session["user_id"]

    # if request.method == "POST":
    book_title = request.form.get("book-title")
    book_subtitle = request.form.get("book-subtitle")
    description = request.form.get("description")
    authors = request.form.get("authors")
    categories = request.form.get("categories")
    published_date = request.form.get("published-date")
    page_count = request.form.get("page-count")
    isbn_10 = request.form.get("isbn-10")
    isbn_13 = request.form.get("isbn-13")
    image_url = request.form.get("image-url")
    shelf_id = request.form.get("shelf-id")

    # Ensure first name was submitted
    if not book_title:
        return apology("must provide book name", 403)

    db.execute("INSERT INTO books (user_id, book_title, book_subtitle, description, authors, categories, published_date, page_count, isbn_10, isbn_13, image_url, shelf_id) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
                user_id, book_title, book_subtitle, description, authors, categories, published_date, page_count, isbn_10, isbn_13, image_url, shelf_id)

    # Send user back to where they started
    return redirect(request.referrer)


@app.route("/books/<book_id>", methods=["GET"])
@login_required
def viewBook(book_id):
    """View book"""

    user_id = session["user_id"]

    # Ensure first name was submitted
    if not book_id:
        return apology("could not find book", 403)

    bookRow = db.execute("SELECT * FROM books WHERE user_id = :user_id AND id = :book_id",
                user_id=user_id, book_id=book_id)

    # Send user back to where they started
    return render_template("book-view.html", bookData=bookRow[0])


@app.route("/books", methods=["GET"])
@login_required
def viewAllBook():
    """View book"""

    user_id = session["user_id"]
    bookRows = db.execute("SELECT * FROM books WHERE user_id = :user_id",
                user_id=user_id)

    # Send user back to where they started
    return render_template("books-all.html", bookRows=bookRows, bookRowsLength=len(bookRows))


@app.route("/update-book", methods=["POST"])
@login_required
def updateBook():
    """Update book in DB"""

    user_id = session["user_id"]

    book_id = int(request.form.get("book-id"))
    book_title = request.form.get("book-title")
    book_subtitle = request.form.get("book-subtitle")
    authors = request.form.get("authors")
    categories = request.form.get("categories")
    published_date = request.form.get("published-date")
    page_count = request.form.get("page-count")
    isbn_10 = request.form.get("isbn-10")
    isbn_13 = request.form.get("isbn-13")
    description = request.form.get("description")
    image_url = request.form.get("image-url")

    # Ensure first name was submitted
    if not book_id:
        return apology("could find book", 403)

    db.execute("UPDATE books SET book_title = :book_title, book_subtitle = :book_subtitle, authors = :authors, categories = :categories, published_date = :published_date, page_count = :page_count, isbn_10 = :isbn_10, isbn_13 = :isbn_13, description = :description, image_url = :image_url WHERE user_id = :user_id AND id = :book_id",
                user_id=user_id, book_id=book_id, book_title=book_title, book_subtitle=book_subtitle, authors=authors, categories=categories, published_date=published_date, page_count=page_count, isbn_10=isbn_10, isbn_13=isbn_13, description=description, image_url=image_url)

    # Send user back to where they started
    return redirect(request.referrer)


@app.route("/add-shelf", methods=["POST"])
@login_required
def addShelf():
    """Add shelf to DB"""

    user_id = session["user_id"]

    # if request.method == "POST":
    shelf_name = request.form.get("shelf-name")

    # Ensure first name was submitted
    if not shelf_name:
        return apology("must provide shelf name", 403)

    db.execute("INSERT INTO shelves (user_id, shelf_name) VALUES(?, ?)",
                user_id, shelf_name)

    # Send user back to where they started
    return redirect(request.referrer)


@app.route("/remove-shelf", methods=["POST"])
@login_required
def removeShelf():
    """Add shelf to DB"""

    user_id = session["user_id"]

    # if request.method == "DELETE":
    shelf_id = request.form.get("shelf-id")

    # Ensure first name was submitted
    if not shelf_id:
        return apology("could not remove shelf", 403)

    db.execute("DELETE FROM shelves WHERE user_id = :user_id AND id = :shelf_id",
                user_id=user_id, shelf_id=shelf_id)

    # Send user back to where they started
    return redirect(request.referrer)


@app.route("/shelves/<shelf_id>", methods=["GET"])
@login_required
def viewShelf(shelf_id):
    """View shelf"""

    user_id = session["user_id"]

    # Ensure first name was submitted
    if not shelf_id:
        return apology("could not find shelf", 403)

    shelfRow = db.execute("SELECT id, shelf_name FROM shelves WHERE user_id = :user_id AND id = :shelf_id",
                user_id=user_id, shelf_id=shelf_id)
    bookRows = db.execute("SELECT * FROM books WHERE user_id = :user_id AND shelf_id = :shelf_id",
                user_id=user_id, shelf_id=shelf_id)

    # Send user back to where they started
    return render_template("shelf-view.html", shelfData=shelfRow[0], bookRows=bookRows, bookRowsLength=len(bookRows))


@app.route("/update-shelf", methods=["POST"])
@login_required
def updateShelf():
    """Update shelf in DB"""

    user_id = session["user_id"]

    shelf_id = int(request.form.get("shelf-id"))
    shelf_name = request.form.get("shelf-name")

    # Ensure first name was submitted
    if not shelf_id:
        return apology("could find shelf", 403)

    db.execute("UPDATE shelves SET shelf_name = :shelf_name WHERE user_id = :user_id AND id = :shelf_id",
                user_id=user_id, shelf_id=shelf_id, shelf_name=shelf_name)

    # Send user back to where they started
    return redirect(request.referrer)


def errorhandler(e):
    """Handle error"""
    if not isinstance(e, HTTPException):
        e = InternalServerError()
    return apology(e.name, e.code)


# Listen for errors
for code in default_exceptions:
    app.errorhandler(code)(errorhandler)
