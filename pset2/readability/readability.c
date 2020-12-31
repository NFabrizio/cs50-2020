#include <cs50.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

// Assumptions:
// * a letter is any lowercase character from a to z or any uppercase character from A to Z and does not include any punctuation, digits, or other symbols
// * any sequence of characters separated by spaces should count as a word
// * a sentence will not start or end with a space
// * a sentence will not have multiple spaces in a row
// * any occurrence of a period, exclamation point, or question mark indicates the end of a sentence

int main(void)
{
    float avgLetters = 0;
    float avgSentences = 0;
    float roughIndex = 0;
    int charCount = 0;
    int sentenceCount = 0;
    int wordCount = 0;

    string text = get_string("Text: ");

    for (int i = 0, textLength = strlen(text); i < textLength; i++)
    {
        int isLetter = isalpha(text[i]);
        int isSpace = isspace(text[i]);

        if (isLetter && !isSpace)
        {
            charCount += 1;
        }
        // Check that there is at least 1 character in case only spaces and punctuation are entered
        else if (isSpace && charCount >= 1)
        {
            // Handle case of word at end of sentence
            wordCount += 1;
        }
        else if ((text[i] == '.' || text[i] == '!' || text[i] == '?') && charCount >= 1)
        {
            // Handle case of word at end of sentence
            sentenceCount += 1;
        }

        // Handle counting the word at the end of the text (i.e., if we reach the end of the text and there is at least one character in the text, then the text ends with a word)
        if ((i + 1) == textLength && charCount >= 1)
        {
            wordCount += 1;
        }
    }

    // Calculate index = 0.0588 * L - 0.296 * S - 15.8, where L is letters per 100 words and S sentences per 100 words ()
    avgLetters = (float) charCount / (float) wordCount * 100;
    avgSentences = (float) sentenceCount / (float) wordCount * 100;

    roughIndex = (0.0588 * avgLetters) - (0.296 * avgSentences) - 15.8;

    // Handle logic around less than 1 and greater than 16
    if (roughIndex < 1)
    {
        printf("Before Grade 1\n");
    }
    else if (roughIndex > 16)
    {
        printf("Grade 16+\n");
    }
    else
    {
        printf("Grade %i\n", (int) round(roughIndex));
    }
}