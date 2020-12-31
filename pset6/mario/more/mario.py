from cs50 import get_int

height = 0
rowCount = 1


def main():
    global height
    global rowCount

    # Re-prompt user if input is not between 1 and 8
    while height < 1 or height > 8:
        height = get_int("Height: ")

    while rowCount <= height:
        print(f"{' ' * (height - rowCount)}{hasher()}  {hasher()}")
        rowCount += 1


def hasher():
    return '#' * rowCount


main()