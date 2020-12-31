from sys import argv, exit
import cs50
import csv

# The program should accept the name of a house as a command-line argument.
# * If the incorrect number of command-line arguments are provided, the program should print an error and exit.
# The program should query the students table in the students.db database for all of the students in the specified house.
# The program should then print out each student’s full name and birth year (formatted as, e.g., Harry James Potter, born 1980 or Luna Lovegood, born 1981).
# * Each student should be printed on their own line.
# * Students should be ordered by last name. For students with the same last name, they should be ordered by first name.

# Check for correct usage with correct number of args
if (len(argv) != 2):
    print("Missing argument. Usage python roster.py [house_name]")
    exit(1)

# Open connection to DB file
db = cs50.SQL("sqlite:///students.db")

results = db.execute("SELECT first, middle, last, birth FROM students WHERE house = ? ORDER BY last ASC, first ASC",
                     argv[1])

for result in results:
    if result['middle']:
        print(f"{result['first']} {result['middle']} {result['last']}, born {result['birth']}")
    else:
        print(f"{result['first']} {result['last']}, born {result['birth']}")