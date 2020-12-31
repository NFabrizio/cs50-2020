#include <cs50.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

// Max number of candidates
// #define MAX 9

// preferences[i][j] is number of voters who prefer i over j
// int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[6][6];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
// string candidates[MAX];
pair pairs[] = {
    [0].winner = 0,
    [0].loser = 1,
    [1].winner = 1,
    [1].loser = 4,
    [2].winner = 4,
    [2].loser = 2,
    [3].winner = 4,
    [3].loser = 3,
    [4].winner = 3,
    [4].loser = 5,
    [5].winner = 5,
    [5].loser = 1,
    [6].winner = 2,
    [6].loser = 1
};

int pair_count = 7;
int candidate_count = 6;

void lock_pairs(void);

int main(int argc, string argv[])
{
    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 7;

    lock_pairs();

    return 0;
}



bool isCyclical(int first, int second, int tracker)
{
    // Need a way to keep track of first for comparison as we recurse
    // If we follow the path and ever get a true for first, stop and return true

printf("tracker initial: %i\n", tracker);
    if(tracker < 0)
    {
        tracker = first;
    }
printf("first: %i\n", first);
printf("second: %i\n", second);
printf("tracker secondary: %i\n", tracker);
    // If there is already a lock on the reverse index, stop and return true
    if(locked[second][first])
    {
printf("lock is cyclical\n");
        return true;
    }

    for(int i=0; i < candidate_count; i++)
    {
        bool lockExists = locked[second][i];

        // If a lock exists and it is on the index that we are tracking, stop and return true since we have a cyclical lock
        if(lockExists && i == tracker)
        {
printf("lockExists & lock is cyclical\n");
            return true;
        }
        // If lock exists and index does not match tracker, recurse and keep following the path of locks
        else if(lockExists && i != tracker)
        {
            // return isCyclical(second, i, tracker);
            if(isCyclical(second, i, tracker))
            {
                return true;
            }
//             bool isTrue = isCyclical(second, i, tracker);
// printf("isTrue: %d\n", isTrue);
//             return isTrue;
        }
    }
printf("Returning false!\n");
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
    for(int i=0; i < pair_count; i++)
    {
        int winnerIndex = pairs[i].winner;
        int loserIndex = pairs[i].loser;

        if(!isCyclical(winnerIndex, loserIndex, -1))
        {
            locked[winnerIndex][loserIndex] = true;
        }
    }

//Displaying array elements
printf("locked array elements:\n");
for(int i=0; i<candidate_count; i++) {
    printf("Candidate %i: ", i);
    for(int j=0;j<candidate_count;j++) {
        printf("%d ", locked[i][j]);
        if(j==candidate_count-1){
            printf("\n");
        }
    }
}

    return;
}