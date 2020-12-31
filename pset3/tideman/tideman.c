#include <cs50.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
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
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        //
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

int candidateIndex(string name)
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (!strcasecmp(candidates[i], name))
        {
            return i;
        }
    }

    return -1;
}

// Update ranks given a new vote
// * If name matches the name of a valid candidate, then update the ranks array to indicate that the voter has the candidate as their rank preference (0 is the first preference, 1 is the second, etc.)
// * ranks[i] represents the user’s ith preference (i.e., ranks[0] is user's first preference, ranks[1] is user's second preference, etc.)
// * Return true if the rank was successfully recorded, and false otherwise (if, for instance, name is not the name of one of the candidates)
// * Assume that no two candidates will have the same name
bool vote(int rank, string name, int ranks[])
{
    // Check that name is valid
    int candidateNumber = candidateIndex(name);

    if (candidateNumber != -1)
    {
        // Add candidate index number to ranks array at the current position
        ranks[rank] = candidateNumber;
        return true;
    }

    return false;
}

// Update preferences given one voter's ranks
// * The function is called once for each voter, and takes as argument the ranks array, (recall that ranks[i] is the voter’s ith preference, where ranks[0] is the first preference).
// * The function should update the global preferences array to add the current voter’s preferences. Recall that preferences[i][j] should represent the number of voters who prefer candidate i over candidate j.
// * You may assume that every voter will rank each of the candidates.
void record_preferences(int ranks[])
{
    // Loop through ranks, and update preferences array using ith element value as i and each successive element value as j (e.g., if ranks = [2,1,4,3], increment value at preferences[2][1], preferences[2][4], preferences[2][3])
    for (int i = 0; i < candidate_count; i++)
    {
        int preferred = ranks[i];
        for (int j = i + 1; j < candidate_count; j++)
        {
            int preferredOver = ranks[j];
            preferences[preferred][preferredOver] += 1;
        }
    }

    return;
}

// Record pairs of candidates where one is preferred over the other
// * Add all pairs of candidates where one candidate is preferred to the pairs array. A pair of candidates who are tied (one is not preferred over the other) should not be added to the array.
// * Update the global variable pair_count to be the number of pairs of candidates. (The pairs should thus all be stored between pairs[0] and pairs[pair_count - 1], inclusive).
void add_pairs(void)
{
    // Use existing count to keep track of pairs array elements
    pair_count = 0;
    // Compare each candidate pref in preferences array to its counterpart (e.g., preferences[i][j] to preferences[j][i]), and if one is larger, add it to the pairs array
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            int first = preferences[i][j];
            int second = preferences[j][i];

            if (first > second)
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;

                pair_count++;
            }
        }
    }

    return;
}

// This pattern borrowed from https://www.geeksforgeeks.org/comparator-function-of-qsort-in-c/
int comparator(const void *p, const void *q)
{
    // Get winner and loser indeces from each pair to be compared
    int lWinner = ((pair *)p)->winner;
    int lLoser = ((pair *)p)->loser;
    int rWinner = ((pair *)q)->winner;
    int rLoser = ((pair *)q)->loser;

    // Use winner and loser indeces to find number of votes for actual comparison
    return (preferences[rWinner][rLoser] - preferences[lWinner][lLoser]);
}

// Sort pairs in decreasing order by strength of victory
// * Sort the pairs array in decreasing order of strength of victory, where strength of victory is defined to be the number of voters who prefer the preferred candidate.
// * If multiple pairs have the same strength of victory, you may assume that the order does not matter.
void sort_pairs(void)
{
    // This pattern borrowed from https://www.geeksforgeeks.org/comparator-function-of-qsort-in-c/
    qsort(&pairs[0], pair_count, sizeof pairs[0], comparator);

    return;
}

bool isCyclical(int first, int second, int tracker)
{
    if (tracker < 0)
    {
        tracker = first;
    }

    // If there is already a lock on the reverse index, stop and return true
    if (locked[second][first])
    {
        return true;
    }

    for (int i = 0; i < candidate_count; i++)
    {
        bool lockExists = locked[second][i];

        // If a lock exists and it is on the index that we are tracking, stop and return true since we have a cyclical lock
        if (lockExists && i == tracker)
        {
            return true;
        }
        // If lock exists and index does not match tracker, recurse and keep following the path of locks
        else if (lockExists && i != tracker)
        {
            // return isCyclical(second, i, tracker);
            if (isCyclical(second, i, tracker))
            {
                return true;
            }
        }
    }

    return false;
}

// Lock pairs into the candidate graph in order, without creating cycles
// * Create a locked graph, adding all edges in decreasing order of victory strength so long as the edge does not create a cycle.
void lock_pairs(void)
{
    // Since pairs is sorted in decreasing order of victory strength, start locking edges with the first element of the pairs array
    // Before each lock, check whether locking will create a cycle
    // 1.) Use index for lock and check candidate at that index to see if it has true for prospective lock index.
    // 1a.) If yes, skip lock.
    // 1b.) If no, choose next true value index and go back to step 1.
    // 2.) Repeat for all true values.
    for (int i = 0; i < pair_count; i++)
    {
        int winnerIndex = pairs[i].winner;
        int loserIndex = pairs[i].loser;

        if (!isCyclical(winnerIndex, loserIndex, -1))
        {
            locked[winnerIndex][loserIndex] = true;
        }
    }

    return;
}

// Print the winner of the election
// * print out the name of the candidate who is the source of the graph.
// * You may assume there will not be more than one source.
void print_winner(void)
{
    // Find the candidate that does not have any other candidates locked over them (i.e., the index of the candidate with no true values in the locked array)
    // 1.) Start with 0 index and track candidates with false
    // 2.) Go to next index and only check those from the previous step to see if they still have false
    // 3.) Repeat step 2 until all indexes have been checked or there is only one index left
    int notLocked[candidate_count];
    int notLockedLength = 0;

    for (int i = 0; i < candidate_count; i++)
    {
        if (i > 0 && notLockedLength)
        {
            for (int j = 0; j < notLockedLength; j++)
            {
                bool isLocked = false;

                // Skip this logic for the first element in the locked array since we want all candidate indeces with no locks from the 0th index of locked to start
                if (i > 0 && notLockedLength)
                {
                    // If notLockedLength, use j to get values in notLocked[] and only check those for locked[i][notLocked[j]]
                    isLocked = locked[i][notLocked[j]];

                    if (isLocked)
                    {
                        // If a lock is found, remove this index from notLocked and adjust length
                        // * Set index value of j from notLocked to -1
                        // * Move all elements over to fill in the gap
                        // * Decrement notLockedLength
                        notLocked[j] = -1;

                        // This pattern borrowed from https://www.geeksforgeeks.org/delete-an-element-from-array-using-two-traversals-and-one-traversal/
                        for (int k = 0; k < notLockedLength; k++)
                        {
                            if (notLocked[k] >= 0)
                            {
                                notLocked[k - 1] = notLocked[k];
                            }
                        }

                        notLockedLength -= 1;
                    }
                }
                else
                {
                    // Otherwise just get the value from locked at i, j
                    isLocked = locked[i][j];

                    if (!isLocked)
                    {
                        // Add index value of j to notLocked
                        // Increment notLockedLength
                        notLocked[notLockedLength] = j;
                        notLockedLength += 1;
                    }
                }
            }
        }
        else
        {
            for (int j = 0; j < candidate_count; j++)
            {
                bool isLocked = locked[i][j];

                if (!isLocked)
                {
                    // Add index value of j to notLocked
                    // Increment notLockedLength
                    notLocked[notLockedLength] = j;
                    notLockedLength += 1;
                }
            }
        }
    }

    printf("%s\n", candidates[notLocked[0]]);
    return;
}

