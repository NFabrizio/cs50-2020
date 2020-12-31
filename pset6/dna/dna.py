from sys import argv, exit
import csv
import re

# The program should require as its first command-line argument the name of a CSV file containing the STR counts for a list of individuals and should require as its second command-line argument the name of a text file containing the DNA sequence to identify.
# * If the program is executed with the incorrect number of command-line arguments, the program should print an error message.
# * If the correct number of arguments are provided, assume that the first argument is the filename of a valid CSV file and that the second argument is the filename of a valid text file.
# The program should open the CSV file and read its contents into memory.
# * Assume that the first row of the CSV file will be the column names. The first column will be the word name and the remaining columns will be the STR sequences themselves.
# The program should open the DNA sequence and read its contents into memory.
# For each of the STRs (from the first line of the CSV file), compute the longest run of consecutive repeats of the STR in the DNA sequence to identify.
# If the STR counts match exactly with any of the individuals in the CSV file, print out the name of the matching individual.
# * Assume that the STR counts will not match more than one individual.
# * If the STR counts do not match exactly with any of the individuals in the CSV file, print "No match".

# Check for correct usage with correct number of args
if (len(argv) != 3):
    print("Missing argument. Usage dna.py [database.csv] [sequence.txt]")
    exit(1)

answerKey = []
dnaKeys = []
dnaReadPatterns = {}
sequenceList = []

# Pattern borrowed from https://docs.python.org/3/library/csv.html
with open(argv[1], newline='') as csvFile:
    dbReader = csv.DictReader(csvFile)
    sequenceList = dbReader.fieldnames.copy()
    sequenceList.pop(0)

    # Create a list of all sequences in the dictionary and initialize each to 0
    for key in sequenceList:
        dnaReadPatterns[key] = 0

    # Add info about each person to dnaKeys list for use later on
    for index, dbRow in enumerate(dbReader):
        dnaKeys.append(dbRow)

# Once dnaKeys is complete, copy it to answerKey
answerKey = dnaKeys.copy()

# Open and read DNA sequence file
with open(argv[2], newline='') as txtFile:
    txtReader = txtFile.read()

    # Run a regex query for each sequence in the dictionary
    for sequence in sequenceList:
        # This matching pattern borrowed from https://stackoverflow.com/questions/61131768/how-to-count-consecutive-substring-in-a-string-in-python-3
        # Find all groups of the sequence that repeat consecutively
        matches = re.findall(rf"(?:{sequence})+", txtReader)

        # Make sure there are at least one match before running the logic below
        if (len(matches) > 0):
            # Find the pattern with the longest length, and add it as the value of the number of consecutive repeats
            largestMatch = max(matches, key=len)
            # Divide by sequence length since each sequence may contain multiple characters for each sequence
            dnaReadPatterns[sequence] = len(largestMatch) / len(sequence)

# Loop through keys, check sequence counts and update answerKey based on comparison
for dnaKey in dnaKeys:
    keyName = dnaKey['name']

    for sequence in sequenceList:
        # If sequence counts do not match, remove this person from the answerKey
        if (int(dnaReadPatterns[sequence]) != int(dnaKey[sequence])):
            # Loop through answerKey to find the key with the name that matches the person being checked
            for index, answerItem in enumerate(answerKey):
                if (answerItem['name'] == keyName):
                    answerKey.pop(index)
                    break
            break

# If answerKey length is 0, there were no matches
if (len(answerKey) > 0):
    print(answerKey[0]['name'])
else:
    print("No match")