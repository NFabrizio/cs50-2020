#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

// Max number of candidates
#define MAX 9

// Candidates have name and vote count
typedef struct
{
    string name;
    int votes;
}
candidate;

// Array of candidates
candidate candidates[MAX];

// Number of candidates
int candidate_count;

// Function prototypes
bool vote(string name);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: plurality [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i].name = argv[i + 1];
        candidates[i].votes = 0;
    }

    int voter_count = get_int("Number of voters: ");

    // Loop over all voters
    for (int i = 0; i < voter_count; i++)
    {
        string name = get_string("Vote: ");

        // Check for invalid vote
        if (!vote(name))
        {
            printf("Invalid vote.\n");
        }
    }

    // Display winner of election
    print_winner();
}

void candidateUpdateVoteCount(string name)
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (!strcasecmp(candidates[i].name, name))
        {
            candidates[i].votes++;

            break;
        }
    }
}

bool candidateValid(string name)
{
    string foundName = "";

    for (int i = 0; i < candidate_count; i++)
    {
        if (!strcasecmp(candidates[i].name, name))
        {
            foundName = name;
            break;
        }
    }

    return strcasecmp(foundName, "");
}

int findMaxVotes()
{
    int highestVotes = 0;
    for (int i = 0; i < candidate_count; i++)
    {
        // If number of candidate votes is the same or more than highestVotes so far, change value of highestVotes
        if (candidates[i].votes >= highestVotes)
        {
            highestVotes = candidates[i].votes;
        }
    }

    return highestVotes;
}

// Update vote totals given a new vote
// * vote takes a single argument, a string called name, representing the name of the candidate who was voted for.
// * If name matches one of the names of the candidates in the election, then update that candidate’s vote total to account for the new vote. The vote function in this case should return true to indicate a successful ballot.
// * If name does not match the name of any of the candidates in the election, no vote totals should change, and the function should return false to indicate an invalid ballot.
// * Assume that no two candidates will have the same name.

bool vote(string name)
{
    if (candidateValid(name))
    {
        candidateUpdateVoteCount(name);

        return true;
    }

    return false;
}

// Print the winner (or winners) of the election
// * Print out the name of the candidate who received the most votes in the election, and then print a newline.
// * If the election ends in a tie, output the names of each of the winning candidates, each on a separate line.
void print_winner(void)
{
    // Keep track of highest number of votes
    int maxVotes = findMaxVotes();

    for (int i = 0; i < candidate_count; i++)
    {
        // If number of candidate votes is the same or more than maxVotes so far, add their name to the list
        if (candidates[i].votes >= maxVotes)
        {
            printf("%s\n", candidates[i].name);
        }
    }
}
