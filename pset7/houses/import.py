from sys import argv, exit
import cs50
import csv

# The program should accept the name of a CSV file as a command-line argument.
# * If the incorrect number of command-line arguments are provided, the program should print an error and exit.
# * Assume that the CSV file will exist, and will have columns name, house, and birth.
# For each student in the CSV file, insert the student into the students table in the students.db database.
# * While the CSV file provided has just a name column, the database has separate columns for first, middle, and last names.
# * Parse each name and separate it into first, middle, and last names.
# * Assume that each person’s name field will contain either two space-separated names (a first and last name) or three space-separated names (a first, middle, and last name).
# * For students without a middle name, leave their middle name field as NULL in the table.

# Check for correct usage with correct number of args
if (len(argv) != 2):
    print("Missing argument. Usage python import.py [data.csv]")
    exit(1)

# Open connection to DB file
db = cs50.SQL("sqlite:///students.db")

# Open CSV and read data
with open(argv[1], newline='') as csvFile:
    dbReader = csv.DictReader(csvFile)

    for dbRow in dbReader:
        firstName = ''
        middleName = ''
        lastName = ''

        # Split name field at spaces to get first, last and/or middle
        namesList = dbRow['name'].split(' ')
        hasMiddleName = len(namesList) > 2

        firstName = namesList[0]

        # If there is a middle name, insert it with the other fields into the DB
        if hasMiddleName:
            middleName = namesList[1]
            lastName = namesList[2]

            db.execute("INSERT INTO students (first, middle, last, house, birth) VALUES(?, ?, ?, ?, ?)",
                       firstName, middleName, lastName, dbRow['house'], dbRow['birth'])
        # Else just insert the fields that exist
        else:
            lastName = namesList[1]

            db.execute("INSERT INTO students (first, last, house, birth) VALUES(?, ?, ?, ?)",
                       firstName, lastName, dbRow['house'], dbRow['birth'])
