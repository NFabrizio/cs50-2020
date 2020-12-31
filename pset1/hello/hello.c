#include <stdio.h>
#include <cs50.h>

int main(void)
{
    // Prompt user to enter their name
    string name = get_string("What is your name?\n");
    // Print hello, <user's name> to screen
    printf("hello, %s\n", name);
}