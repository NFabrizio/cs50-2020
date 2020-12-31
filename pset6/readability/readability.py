from cs50 import get_string
import re

# Assumptions:
# * a letter is any lowercase character from a to z or any uppercase character from A to Z and does not include any punctuation, digits, or other symbols
# * any sequence of characters separated by spaces should count as a word
# * a sentence will not start or end with a space
# * a sentence will not have multiple spaces in a row
# * any occurrence of a period, exclamation point, or question mark indicates the end of a sentence

inputText = get_string("Text: ")

# Use regex to find all letters, spaces and accepted punctuation
allLetters = re.findall("[A-Za-z]", inputText)
allSpaces = re.findall("\s", inputText)
allPunctuation = re.findall("[.!?]", inputText)

charCount = len(allLetters)
# Add one to space count to account for words at the end of the text
wordCount = len(allSpaces) + 1
sentenceCount = len(allPunctuation)

# Calculate index = 0.0588 * L - 0.296 * S - 15.8, where L is letters per 100 words and S sentences per 100 words ()
avgLetters = charCount / wordCount * 100
avgSentences = sentenceCount / wordCount * 100

roughIndex = (0.0588 * avgLetters) - (0.296 * avgSentences) - 15.8

# Handle logic around less than 1 and greater than 16
if (roughIndex < 1):
    print("Before Grade 1\n")
elif (roughIndex > 16):
    print("Grade 16+\n")
else:
    print(f"Grade {round(roughIndex)}")