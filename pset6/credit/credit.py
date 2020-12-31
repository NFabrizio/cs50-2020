from cs50 import get_string


def main():
    cardNumber = get_string("Number: ")

    if (isValid(cardNumber) != 0):
        print("INVALID\n")
        return 0
    else:
        cardType(cardNumber)
        return 0


def isValid(cardNumber):
    doubled = 0
    summedDigits = 0

    # Get a list of card digits from right to left
    reversedNumber = reversed(cardNumber)

    for index, digit in enumerate(reversedNumber):
        if (index % 2 == 0):
            # If even index, just add the digit to the sum
            summedDigits += int(digit)
        else:
            # If odd index, double the digit and check whether it is a two digit number
            doubled = int(digit) * 2
            doubledTenth = int(doubled / 10)

            if (doubledTenth == 0):
                # If not two digit number, just add it to the running total
                summedDigits += doubled
            else:
                # If two digit number, add each digit to the running total
                summedDigits += doubled % 10
                summedDigits += doubledTenth

    return summedDigits % 10


def cardType(cardNumber):
    ccNum = int(cardNumber)
    # Anything with 16 digits that starts with 51, 52, 53, 54, or 55
    if (ccNum >= 5100000000000000 and ccNum < 5600000000000000):
        print("MASTERCARD\n")

    # Anything with 15 digits that starts with 34 or 37
    elif ((ccNum >= 340000000000000 and ccNum < 350000000000000) or (ccNum >= 370000000000000 and ccNum < 380000000000000)):
        print("AMEX\n")

    # Anything with 13 or 16 digits that starts with 4
    elif ((ccNum >= 4000000000000 and ccNum < 5000000000000) or (ccNum >= 4000000000000000 and ccNum < 5000000000000000)):
        print("VISA\n")

    # Mark anything that doesn't match the above conditions as invalid
    else:
        print("INVALID\n")


main()