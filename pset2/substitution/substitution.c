#include <cs50.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// * This program accepts a single command-line argument, which is the key to use for the substitution.
// * The key itself is case-insensitive, so whether any character in the key is uppercase or lowercase it does not affect the behavior of the program.
// * If the program is executed without any command-line arguments or with more than one command-line argument, the program prints an error message and returns a value of 1 from main immediately.
// * If the key is invalid (does not contain 26 characters, contains any character that is not an alphabetic character or does not contain each letter in the alphabet exactly once), the program prints an error message and returns a value of 1 from main immediately.
// * The program outputs a prompt for user input with the prompt text "plaintext:".
// * The program outputs "ciphertext:" followed by the plaintext’s corresponding ciphertext, with each alphabetical character in the plaintext substituted for the corresponding character in the ciphertext; non-alphabetical characters are output unchanged.
// * The program preserves case: capitalized letters remain capitalized; lowercase letters remain lowercase.

int findIndex(string searchString, char searchChar);
int validateKey(string key, string validator);

int main(int argCount, string argValues[])
{
    if (argCount < 2)
    {
        printf("Error: A command line argument is required to run this program, but none was provided.\n");
        return 1;
    }

    if (argCount > 2)
    {
        printf("Error: A single command line argument is required to run this program, but more than one was provided.\n");
        return 1;
    }

    string alphabetUpper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    string alphabetLower = "abcdefghijklmnopqrstuvwxyz";
    string cipherKey = argValues[1];
    int alphaUpperLength = strlen(alphabetUpper);
    int alphaLowerLength = strlen(alphabetLower);

    int keyValid = validateKey(cipherKey, alphabetUpper);
    if (!keyValid)
    {
        return 1;
    }

    // Validation complete and working properly, next step is to make happy path work
    string inputText = get_string("plaintext: ");
    int inputLength = strlen(inputText);
    char *cipherText = malloc(inputLength + 1);

    // Encode inputText
    // For each character in input text:
    // * Determine if is alphabetic, and if not, return it immediately
    // * Determine letter case
    // * Find letter index in upper/lower alphbet list
    // * Use index to get appropriate letter from key
    // * Convert substitution letter to upper/lower
    // * Add to new string

    for (int k = 0; k < inputLength; k++)
    {
        if (!isalpha(inputText[k]))
        {
            cipherText[k] = inputText[k];
        }

        if (isupper(inputText[k]))
        {
            // Get index of input from alhpabet and use it to find cipher replacement
            int baseIndex = findIndex(alphabetUpper, inputText[k]);

            cipherText[k] = toupper(cipherKey[baseIndex]);
        }
        else if (islower(inputText[k]))
        {
            // Get index of input from alhpabet and use it to find cipher replacement
            int baseIndex = findIndex(alphabetLower, inputText[k]);

            cipherText[k] = tolower(cipherKey[baseIndex]);
        }
    }

    printf("ciphertext: %s\n", cipherText);
    return 0;
}

int findIndex(string searchString, char searchChar)
{
    // This pattern comes from https://stackoverflow.com/questions/1479386/is-there-a-function-in-c-that-will-return-the-index-of-a-char-in-a-char-array/1479401
    char *pointer = strchr(searchString, searchChar);
    return pointer - searchString;
}

int validateAlphabet(string capitalizedKey, string alphabetList)
{
    int valid = 0;
    int alphaListLength = strlen(alphabetList);
    int j = 0;

    do
    {

        // Validate that all letters in alphabet exist in key by comparing to alpha list
        string validator = strchr(capitalizedKey, alphabetList[j]);

        if (!validator)
        {
            // If character from alphabetList is missing set valid to 0 to stop loop and stop validate function
            printf("Error: Key must contain every letter of the alphabet and must contain only one instance of each letter\n");
            valid = 0;
            break;
        }
        else
        {
            valid = 1;
        }

        j++;
    }
    while (j < alphaListLength);

    return valid;
}

int validateKey(string key, string validator)
{
    int i = 0;
    int valid = 0;
    int validatorLength = strlen(validator);
    int keyLength = strlen(key);
    char *allCapsKey = malloc(keyLength + 1);

    // Verify that the key has only 26 characters
    if (keyLength > 26)
    {
        printf("Error: Key must be 26 characters long. Key provided is too long.\n");
        return valid;
    }
    else if (keyLength < 26)
    {
        printf("Error: Key must be 26 characters long. Key provided is too short.\n");
        return valid;
    }

    // Make a copy of the cipher key to use for capitlization and comparison validation
    strcpy(allCapsKey, key);

    do
    {
        // Loop through cipher key and make sure all characters are alpha
        int isAlphaChar = isalpha(key[i]);

        if (!isAlphaChar)
        {
            printf("Error: Key may only contain alphabetic characters, but contains one or more non-alphabetic characters.\n");
            valid = 0;
            break;
        }
        else
        {
            valid = 1;
        }

        // Capitalize copy of cipher key to prep for comparison validation
        allCapsKey[i] = toupper(allCapsKey[i]);

        i++;
    }
    while (i < keyLength);

    // If first level of validation passes, check validation of inclusion of entire alphabet with each character used once
    if (valid)
    {
        valid = validateAlphabet(allCapsKey, validator);
    }

    return valid;
}