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
db = SQL("sqlite:///finance.db")

# Make sure API key is set
if not os.environ.get("API_KEY"):
    raise RuntimeError("API_KEY not set")


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""

    user_id = session["user_id"]
    cash = 0
    grand_total = 0
    ownedShares = {}
    purchaseRows = []

    transactionRows = db.execute("SELECT symbol, name, SUM(shares) as total_shares, transaction_type FROM purchase_history WHERE user_id = :user_id GROUP BY upper(symbol), transaction_type",
                          user_id=user_id)

    for row in transactionRows:
        sharesBought = 0
        sharesSold = 0
        rowSymbol = row["symbol"]

        # If rowSymbol not already in ownedShares, add it with an initial value of 0
        if not rowSymbol in ownedShares:
            ownedShares[rowSymbol] = 0

        # print(row)
        if row["transaction_type"] == "BUY":
            sharesBought = row["total_shares"]

        if row["transaction_type"] == "SELL":
            sharesSold = row["total_shares"]

        # print(f"{row['symbol']}: sold = {sharesSold}, bought = {sharesBought}, owned = {sharesBought - sharesSold}")

        # Add any shares bought and subtract any shares sold
        ownedShares[rowSymbol] += sharesBought
        ownedShares[rowSymbol] -= sharesSold

    userRows = db.execute("SELECT * FROM users WHERE id = :id",
                          id=user_id)

    cash = userRows[0]["cash"]

    for stock in transactionRows:
        if stock["transaction_type"] == "BUY":
            stockData = lookup(stock["symbol"])

            stock["total_shares"] = ownedShares[stock["symbol"]]

            total = float(stock["total_shares"]) * stockData["price"]

            stock["price"] = stockData["price"]
            stock["total"] = usd(total)

            purchaseRows.append(stock)

            grand_total += total

    grand_total += cash

    return render_template("index.html", stocksLength=len(purchaseRows), stocks=purchaseRows, cash=usd(cash), grand_total=usd(grand_total))


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":
        symbol = html.escape(request.form.get("symbol"))
        shareNumber = 0
        user_id = session["user_id"]

        try:
            shareNumber = float(int(request.form.get("shares")))
        except:
            return apology("number of shares must be a positive number", 403)

        # Ensure symbol was submitted
        if not symbol:
            return apology("must provide stock symbol", 403)

        # Ensure number of shares was submitted
        elif not shareNumber:
            return apology("must provide number of shares", 403)

        # Ensure that shares is an integer
        elif not isinstance(shareNumber, float):
             return apology("number of shares must be a positive number", 403)

        # Ensure that shares is a positive number
        elif not shareNumber > 0:
             return apology("number of shares must be greater than zero", 403)

        print(f"isinstance(shareNumber, int): {isinstance(shareNumber, int)}")
        print(f"shareNumber > 0: {shareNumber > 0}")

        stockData = lookup(symbol)
        # Get symbol, company name, latest price and pass to template
        if stockData == None:
            return apology("stock symbol not found", 403)

        userRows = db.execute("SELECT * FROM users WHERE id = :id",
                          id=user_id)

        accountBalance = 0
        companyName = stockData["name"]
        currentPrice = stockData["price"]

        cost = currentPrice * float(shareNumber)

        if len(userRows) > 0:
            accountBalance = float(userRows[0]["cash"])

        remainingBalance = accountBalance - cost
        # print(f"accountBalance: {accountBalance}, cost: {cost}, remainingBalance: {remainingBalance}")

        # If the purchase will put their balance at a negative, do not make the purchase
        if remainingBalance < 0:
            return apology("insufficient funds", 403)

        db.execute("INSERT INTO purchase_history (user_id, symbol, name, shares, price, transaction_type) VALUES(?, ?, ?, ?, ?, ?)",
                user_id, symbol.upper(), companyName, shareNumber, currentPrice, "BUY")
        db.execute("UPDATE users SET cash = :remainingBalance WHERE id = :id",
                id=user_id, remainingBalance=remainingBalance)

        if request.form.get("form-type") == "index-modal":
            return redirect("/")

    return render_template("buy.html")


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""

    user_id = session["user_id"]

    transactionRows = db.execute("SELECT * FROM purchase_history WHERE user_id = :user_id",
                          user_id=user_id)

    return render_template("history.html", stocksLength=len(transactionRows), stocks=transactionRows)


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


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":
        # Ensure symbol was submitted
        if not request.form.get("symbol"):
            return apology("must provide stock symbol", 403)

        stockData = lookup(request.form.get("symbol"))
        # Get symbol, company name, latest price and pass to template
        if stockData == None:
            return apology("stock symbol not found", 403)

        return render_template("quoted.html", symbol=stockData['symbol'].upper(), name=stockData['name'], price=usd(stockData['price']))

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("quote.html")


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""

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

        # Ensure password confirmation was submitted
        elif not request.form.get("confirmation"):
            return apology("must provide password confirmation", 403)

        # Ensure password and password confirmation match
        elif not request.form.get("password") == request.form.get("confirmation"):
            return apology("password and password confirmation must match", 403)

        # Set variables for easier access
        username = html.escape(request.form.get("username"))
        passwordHash = generate_password_hash(request.form.get("password"))

        # Ensure username does not already exist in DB
        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = :username",
                          username=username)
        if len(rows) >= 1:
            return apology("username not available", 403)

        # Query database for username
        rows = db.execute("INSERT INTO users (username, hash) VALUES(?, ?)",
                      username, passwordHash)

        # Redirect user to home page
        return redirect("/login")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("register.html")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":
        symbol = html.escape(request.form.get("symbol"))
        shareNumber = 0
        user_id = session["user_id"]
        ownedShares = 0
        sharesBought = 0
        sharesSold = 0

        try:
            print(f"request.form.get(shares): {request.form.get('shares')}")
            print(f"request.form.get(shares) int: {int(request.form.get('shares'))}")
            print(f"request.form.get(shares) float: {float(int(request.form.get('shares')))}")
            shareNumber = float(int(request.form.get("shares")))
        except:
            return apology("exception: number of shares must be a positive number", 403)

        # Ensure symbol was submitted
        if not symbol:
            return apology("must provide stock symbol", 403)

        # Ensure number of shares was submitted
        elif not shareNumber:
            return apology("must provide number of shares", 403)

        # Ensure that shares is an integer
        elif not isinstance(shareNumber, float):
             return apology("instance: number of shares must be a positive number", 403)

        # Ensure that shares is a positive number
        elif not shareNumber > 0:
             return apology("number of shares must be greater than zero", 403)

        purchaseRows = db.execute("SELECT symbol, SUM(shares) as total_shares, transaction_type FROM purchase_history WHERE user_id = :user_id AND symbol = :symbol GROUP BY upper(symbol), transaction_type",
                              user_id=user_id, symbol=symbol.upper())

        for row in purchaseRows:
            print(row)
            if row["transaction_type"] == "BUY":
                sharesBought = row["total_shares"]

            if row["transaction_type"] == "SELL":
                sharesSold = row["total_shares"]

        ownedShares = sharesBought - sharesSold
        print(f"sharesBought: {sharesBought}")
        print(f"sharesSold: {sharesSold}")
        print(f"ownedShares: {ownedShares}")
        print(f"shareNumber: {shareNumber}")

        # Ensure user owns at least number of shares input
        if shareNumber > ownedShares:
            return apology("insufficient owned shares", 403)

        stockData = lookup(request.form.get("symbol"))
        # Get symbol, company name, latest price and pass to template
        if stockData == None:
            return apology("stock symbol not found", 403)

        userRows = db.execute("SELECT * FROM users WHERE id = :id",
                          id=user_id)

        accountBalance = 0
        companyName = stockData["name"]
        currentPrice = stockData["price"]

        revenue = currentPrice * float(shareNumber)

        if len(userRows) > 0:
            accountBalance = float(userRows[0]["cash"])

        updatedBalance = accountBalance + revenue
        print(f"accountBalance: {accountBalance}, revenue: {revenue}, updatedBalance: {updatedBalance}")

        db.execute("INSERT INTO purchase_history (user_id, symbol, name, shares, price, transaction_type) VALUES(?, ?, ?, ?, ?, ?)",
                user_id, symbol.upper(), companyName, shareNumber, currentPrice, "SELL")
        db.execute("UPDATE users SET cash = :updatedBalance WHERE id = :id",
                id=user_id, updatedBalance=updatedBalance)

        if request.form.get("form-type") == "index-modal":
            return redirect("/")

    return render_template("sell.html")


def errorhandler(e):
    """Handle error"""
    if not isinstance(e, HTTPException):
        e = InternalServerError()
    return apology(e.name, e.code)


# Listen for errors
for code in default_exceptions:
    app.errorhandler(code)(errorhandler)
