#include <stdio.h>
#include <cs50.h>
#include <string.h>

void hasher(int hashNumber);
void rowCreator(int rowNumber, int height);
void spacer(int heightNum);

int main(void)
{
    int height = 0;

    // Re-prompt user if input is not between 1 and 8
    do
    {
        height = get_int("Height: ");
    }
    while (height < 1 || height > 8);

    // Loop to create a graphic that has the given number of rows
    for (int i = 1; i <= height; i++)
    {
        rowCreator(i, height);
    }
}

void rowCreator(int rowNumber, int inputHeight)
{
    // Add the correct number of spaces before printing any hashes
    spacer(inputHeight - rowNumber);
    // Generate the first set of hashes for the row
    hasher(rowNumber);
    // Add two spaces between hash sets
    spacer(2);
    // Generate the second set of hashes for the row
    hasher(rowNumber);
    printf("\n");
}

void hasher(int hashNumber)
{
    // Loop to print the correct number of hashes
    for (int i = 1; i <= hashNumber; i++)
    {
        printf("#");
    }
}

void spacer(int spaceNumber)
{
    // Loop to print the correct number of spaces
    for (int i = 0; i < spaceNumber; i++)
    {
        printf(" ");
    }
}