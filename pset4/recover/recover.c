#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Define BYTE type
typedef uint8_t BYTE;

int main(int argc, char *argv[])
{
    // Check that there is one and only one argument provided to this program
    if (argc != 2)
    {
        printf("Usage: ./recover image\n");
        return 1;
    }

    FILE *file = fopen(argv[1], "r");

    if (file == NULL)
    {
        printf("Could not open file %s\n", argv[1]);
        return 1;
    }

    // Look for first three bytes of jpeg file: 0xff 0xd8 0xff 0xe...
    BYTE jpegSignature[] = {0xff, 0xd8, 0xff, 0xe0};
    BYTE buffer[512];
    int fileCounter = -1;
    bool jpgStarted = false;
    char jpgFilename[8] = "000.jpg";
    FILE *img;

    while (fread(&buffer, sizeof(BYTE), 512, file))
    {
        // (buffer[3] & 0xf0) borrowed from CS50 recover walkthrough
        if (buffer[0] == jpegSignature[0] && buffer[1] == jpegSignature[1] && buffer[2] == jpegSignature[2]
            && (buffer[3] & 0xf0) == jpegSignature[3])
        {
            fileCounter++;
            jpgStarted = true;

            // If start of first jpeg, create file, open and start writing to it
            if (fileCounter == 0)
            {
                // create file, open and start writing to it
                sprintf(jpgFilename, "%03i.jpg", fileCounter);
                img = fopen(jpgFilename, "w");
                fwrite(&buffer, sizeof(BYTE), 512, img);
            }
            else if (fileCounter > 0)
            {
                // Close previously written jpeg, increment filename, create file, open it and start writing to it
                fclose(img);
                sprintf(jpgFilename, "%03i.jpg", fileCounter);
                img = fopen(jpgFilename, "w");
                fwrite(&buffer, sizeof(BYTE), 512, img);
            }
        }
        else if (jpgStarted)
        {
            // Continue writing to open file
            fwrite(&buffer, sizeof(BYTE), 512, img);
        }
    }

    fclose(file);
}
