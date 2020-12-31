#include <stdio.h>
#include <cs50.h>
#include <math.h>

void cardType(long ccNum);
int ccNumLength(long ccNum);
int validateCardNumber(long ccNum);

int main(void)
{
    // Get the card number
    long cardNumber = get_long("Number: ");

    // Do the checksum on the number to make sure it is a valid CC number
    int validated = validateCardNumber(cardNumber);

    // If the CC number does not pass checksum, zero will be returned, so print INVALID and stop the function from continuing
    if (validated != 0)
    {
        printf("INVALID\n");
        return 0;
    }

    // Checksum passed, so figure out which type of card it is
    cardType(cardNumber);
}

int ccNumLength(long ccNum)
{
    // This function built off of logic suggested at https://stackoverflow.com/questions/3068397/finding-the-length-of-an-integer-in-c

    // Since we only care about numbers with 16 digits at most, lump all larger numbers into one group called 17
    if (ccNum >= 10000000000000000)
    {
        return 17;
    }

    if (ccNum >= 1000000000000000)
    {
        return 16;
    }
    if (ccNum >= 100000000000000)
    {
        return 15;
    }

    if (ccNum >= 10000000000000)
    {
        return 14;
    }

    if (ccNum >= 1000000000000)
    {
        return 13;
    }

    // Since we only care about numbers with 13 digits at the least, lump all smaller numbers into one group called 1
    return 1;
}

void cardType(long ccNum)
{
    // Anything with 16 digits that starts with 51, 52, 53, 54, or 55
    if (ccNum >= 5100000000000000 && ccNum < 5600000000000000)
    {
        printf("MASTERCARD\n");
    }
    // Anything with 15 digits that starts with 34 or 37
    else if ((ccNum >= 340000000000000 && ccNum < 350000000000000) || (ccNum >= 370000000000000 && ccNum < 380000000000000))
    {
        printf("AMEX\n");
    }
    // Anything with 13 or 16 digits that starts with 4
    else if ((ccNum >= 4000000000000 && ccNum < 5000000000000) || (ccNum >= 4000000000000000 && ccNum < 5000000000000000))
    {
        printf("VISA\n");
    }
    // Mark anything that doesn't match the above conditions as invalid
    else
    {
        printf("INVALID\n");
    }
}

int validateCardNumber(long ccNum)
{
    // Get the length of the card number
    int ccLength = ccNumLength(ccNum);
    // Convert card number length to zero based for use with arrays
    int zeroLength = ccLength - 1;
    // Initiate an array with the correct length
    int numArray[zeroLength];
    // Initiate variable to hold the sum of the numbers for doing checksum
    int numSum = 0;

    // This loop breaks off each digit in the card number and adds it to the array starting
    // from the right side of the card number
    for (int z = 0; z <= zeroLength; z++)
    {
        long divisor = pow(10, z);
        long modulo = pow(10, z + 1);
        // As we loop, gradually increase the modulo by powers of ten to get the card numbers
        // starting from the right side and then divide by increasing powers of ten by a factor
        // of one less so that only the first number in the set will be left (e.g., to get the
        // digit third from the right for the card number 24680, we take modulo 1000 which
        // returns 680 and divide by 100 which leaves 6)
        long digit = (ccNum % modulo) / divisor;

        numArray[z] = digit;
    }

    // This loop performs the first part of the checksum using every other digit starting
    // from the right side of the card number. Since the card number was inversely stored
    // in the array in the previous loop, it can start from the beginning of the array.
    // This loop starts at 1 to get the second element in the array and increments by 2
    // on each iteration to take every other number.
    for (int y = 1; y <= zeroLength; y += 2)
    {
        // Double the array element digit
        int doubled = numArray[y] * 2;
        // Set modulo default to zero
        int doubledModulo = 0;
        // Divide the doubled digit by ten to handle the case of a two digit number after
        // doubling and set to an integer type so that any decimal values will be removed,
        // leaving only the number in the tens place
        int doubledTenth = doubled / 10;

        // If dividing the doubled number by ten returns zero, the doubled number is a
        // single digit and should be added to the sum
        if (!doubledTenth)
        {
            numSum += doubled;
        }
        // If there is a number other than zero after dividing the doubled digit by ten,
        // it is a two digit number and we should calculate the modulo 10 of it to get
        // the digit in the ones place and assign it to the doubledModulo variable for later
        else
        {
            doubledModulo = doubled % 10;
        }

        // doubledTenth will be zero if the doubled digit was a single digit number or
        // greater than zero if the doubled digit was a two digit number
        numSum += doubledTenth;
        // doubledModulo will be zero if the doubled digit was a single digit number or
        // greater than zero if the doubled digit was a two digit number
        numSum += doubledModulo;
    }

    // After doing all the complicated math, add every other digit in the card number
    // array starting from the first element to the running sum total
    for (int x = 0; x <= zeroLength; x += 2)
    {
        numSum += numArray[x];
    }

    // Finally, do modulo ten on the sum and return the value (zero it ends with zero
    // or a number greater than zero if it has a remainder)
    return numSum % 10;
}