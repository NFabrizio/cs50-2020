# CS50 2020 Final Project - Bookshelf Application

This application was built for the final project for CS50 2020. The only
requirements for this project were to 1.) build something of interest,
solves a problem, impacts the community or changes the world, and 2.)
draws upon the lessons learned throughout CS50 2020.

The application is a virtual bookshelf that allows authenticated users
to create virtual bookshelves and add books to their shelves. The
intention of the application is to create a virtual space where users can
store a record of the books that they own and categorize them using the
shelves so that they can reference their collection of books from anywhere
that they have internet access.

The application is built in Python with Flask and uses a SQL database as
a data store. The application was built on top of the starter kit provided
in CS50 2020 with the finance problem set for week 8.

## Installation and Set Up
Below are the instructions for installing this application.
*These instructions are valid as of 2020.12.31*

### Environment Set Up
1. Clone this repository to your local environment.
  1. Fork this Github repo.
    1. In a web browser, visit https://github.com/NFabrizio/cs50-2020
    2. Click the Fork button in the upper right corner of the screen
    3. In the "Where should we fork this repository?" pop up, select your username.
    Github should create a fork of the repo in your account
  2. Clone your fork of the unity-bubble-game repo.
    1. In the terminal on your local environment, navigate to the directory where
    you want to clone the cs50-2020 repo
      `cd ~/path/to/your/directory`
    2. In the terminal, run:
      `git clone [clone-url-for-your-fork]`
      The URL should be in the format git@github.com:YourUsername/cs50-2020.git
2. Install application dependencies.
  1. Install Python.
    1. Follow the instructions at https://wiki.python.org/moin/BeginnersGuide/Download.
  2. Install sqlite3.
    1. Follow the instructions at https://flaviocopes.com/sqlite-how-to-install/.
  3. Install Flask.
    1. Follow the instructions at https://flask.palletsprojects.com/en/1.1.x/installation/#install-flask.
3. Run the application by changing into the directory where you cloned the git repo and run the command below:
  `flask run`

## Application Use
To use the application, you must first create an account by navigating to the
/register route in the application. Once you have successfully registered, log in
using the credentials you provided during the registration process.

Upon logging into the application, you will be taken to the shelves route. You can
create as many shelves as you like by clicking the "Create Shelf" button and providing
a title for the shelf. From the shelves route, you can view a shelf by clicking on its
name in the list, you can edit a shelf by clicking on the edit icon to the right of the
shelf name or you can delete a shelf by clicking the "X" icon to the right of the
shelf name.

While viewing a shelf, you have the option to add books to the shelf by clicking the
"Add Book" button. After clicking this button, a modal will open with all of the fields
add for your book. Once a book has been added to a shelf, you see a card displaying
some of the basic information about the book that you added as well as a view icon to
view more details and an edit icon to make changes to the data you entered about the book.

When finished using the application, click the "Log out" link in the upper right corner
of the page in your web browser.

To stop the application from running in your local environment, use the terminal where
the application is running, and press control + C.
