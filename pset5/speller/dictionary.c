// Implements a dictionary's functionality
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "dictionary.h"

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

int findIndex(char *searchString, char searchChar);
unsigned int hash(const char *word);
void insertNode(int hashValue, node *newNode);
// int power(int base, unsigned int exp);

// Number of chars to use for hash table
// Tested up to 5 chars, but 4 was actually slightly faster
const unsigned int charLimit = 4;

// Number of buckets in hash table
// Set this to 26^charLimit
const unsigned int N = 456976;

// Hash table
node *table[N] = { 0 };

// Number of words loaded in dictionary
int dictWordCount = 0;

// Returns true if word is in dictionary else false
bool check(const char *word)
{
    int hashValue = hash(word);

    if (table[hashValue] == NULL)
    {
        return false;
    }

    node *hashNode = table[hashValue];

    // Loop through hash table linked list until a word is found that comes after tmpWord
    while (strcasecmp(word, hashNode->word) > 0)
    {
        // Ensure we have not reached the end of the linked list before moving on to the next node
        if (hashNode->next != NULL)
        {
            // Set hashNode to the next node
            hashNode = hashNode->next;
        }
        else
        {
            return false;
        }
    }

    if (strcasecmp(word, hashNode->word) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// ******** Hash by first N char ********
// Hashes word to a number
unsigned int hash(const char *word)
{
    // Set up array of all letters in alphabet
    char *alphabetUpper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char *alphabetLower = "abcdefghijklmnopqrstuvwxyz";

    int baseIndex = 0;
    int M = 31;
    int indexArray[charLimit];
    int indexArrayLength = 0;
    int wordLength = strlen(word);

    for (int i = 0; i < charLimit && i < wordLength; i++)
    {
        if (isalpha(word[i]) && isupper(word[i]))
        {
            // Get index of input from alhpabet and use it to find cipher replacement
            indexArray[indexArrayLength] = findIndex(alphabetUpper, word[i]);
            indexArrayLength += 1;
        }
        else if (isalpha(word[i]) && islower(word[i]))
        {
            // Get index of input from alhpabet and use it to find cipher replacement
            indexArray[indexArrayLength] = findIndex(alphabetLower, word[i]);
            indexArrayLength += 1;
        }
    }

    // This pattern borrowed from https://www.strchr.com/hash_functions multiplicative hash function
    for(int j = 0; j < indexArrayLength; ++j)
    {
        baseIndex = M * baseIndex + indexArray[j];
    }

    return baseIndex % N;
}

// Loads dictionary into memory, returning true if successful else false
bool load(const char *dictionary)
{
    // Use table as array (array of linked lists) for hash table
    // Open dictionary file
    // Read strings from file, one at a time
    // Create a new node for each word
    // Hash word to obtain a hash value
    // Insert node as part of linked list into hash table at that location
    FILE *dictFile = fopen(dictionary, "r");

    if (dictFile == NULL)
    {
        printf("Could not open file %s\n", dictionary);
        return false;
    }

    char tmpWord[LENGTH + 1];
    node *newNode = NULL;

    // Use fscanf(dictFile, "%s", place_to_store_word) to read the strings from the dictFile, returns EOF when it reaches the end of the file
    while(fscanf(dictFile, "%s", tmpWord) != EOF)
    {
        // Use malloc() to store word in new node, be sure to check if malloc returns NULL, copy word into node using strcopy
        newNode = malloc(sizeof(node));
        if (newNode == NULL)
        {
            return false;
        }

        strcpy(newNode->word, tmpWord);
        newNode->next = NULL;

        int hashValue = hash(tmpWord);

        insertNode(hashValue, newNode);
    }

    fclose(dictFile);

    return true;
}

// Returns number of words in dictionary if loaded else 0 if not yet loaded
unsigned int size(void)
{
    // Set up a counter and increment it in load() to keep track of how many words are in dictionary
    return dictWordCount;
}

// Unloads dictionary from memory, returning true if successful else false
bool unload(void)
{
    // Loop through hash table and free each element in each linked list
    // Use two pointers, one to move to next node and one to free current node
    node *liberator = NULL;
    node *cursor = NULL;

    for (int i = 0; i < N; i++)
    {
        // Only free memory if the hash table index has some allocated
        if (table[i] != NULL)
        {
            cursor = table[i];
            liberator = table[i];

            while(cursor->next != NULL)
            {
                // Point cursor to next node in linked list
                cursor = cursor->next;
                // Free current node in linked list
                free(liberator);
                // Point liberator to next node in linked list
                liberator = cursor;
            }

            // Free final node in linked list
            free(liberator);
        }
    }
    return true;
}

int findIndex(char *searchString, char searchChar)
{
    // This pattern comes from https://stackoverflow.com/questions/1479386/is-there-a-function-in-c-that-will-return-the-index-of-a-char-in-a-char-array/1479401
    char *pointer = strchr(searchString, searchChar);
    return pointer - searchString;
}

void insertNode(int hashValue, node *newNode)
{
    if (table[hashValue] == NULL)
    {
        table[hashValue] = newNode;
        dictWordCount += 1;
    }

    node *hashNode = table[hashValue];
    char *tmpWord = newNode->word;

    // Loop through hash table linked list until a word is found that comes after tmpWord
    while (strcasecmp(tmpWord, hashNode->word) > 0)
    {
        // Ensure we have not reached the end of the linked list before moving on to the next node
        if (hashNode->next != NULL)
        {
            // Set hashNode to the next node
            hashNode = hashNode->next;
        }
        else
        {
            hashNode->next = newNode;
            dictWordCount += 1;
        }
    }

    // Ensure that tmpWord is not the same as hashNode->word
    if (strcasecmp(tmpWord, hashNode->word) < 0)
    {
        // tmpWord comes before hashNode->word, so add pointer from newNode and move pointer from previous node to point to newNode
        newNode->next = hashNode->next;
        hashNode->next = newNode;
        dictWordCount += 1;
    }
}
