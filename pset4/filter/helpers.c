#include "helpers.h"
#include <math.h>
#include <stdio.h>

// Create a new struct to be able to handle negative values
typedef struct
{
    int  rgbtBlue;
    int  rgbtGreen;
    int  rgbtRed;
}
RGBTRIPLE_INT;

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Calculate the average value of red, blue and green, round to nearest whole number and set the value for each to the average
            // Cast RGB values to floats before averaging so that there is an accurate decimal point value afterwards, idea borrowed from https://stackoverflow.com/questions/15799024/how-to-output-the-decimal-in-average
            float hexAvg = ((float) image[i][j].rgbtRed + (float) image[i][j].rgbtGreen + (float) image[i][j].rgbtBlue) / 3;
            int hexAvgRounded = (int) round(hexAvg);

            image[i][j].rgbtRed = hexAvgRounded;
            image[i][j].rgbtGreen = hexAvgRounded;
            image[i][j].rgbtBlue = hexAvgRounded;
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    // If the image is only one pixel wide, there is nothing to reflect, so return without doing anything
    if (width <= 1)
    {
        return;
    }

    int halfWidth = 0;
    RGBTRIPLE swapTemp;
    int isOddWidth = width % 2;

    if (isOddWidth)
    {
        halfWidth = (width - 1) / 2;
    }
    else
    {
        halfWidth = width / 2;
    }

    for (int i = 0; i < height; i++)
    {
        // Stop before j is equal to halfWidth since j starts at 0
        for (int j = 0; j < halfWidth; j++)
        {
            swapTemp = image[i][j];
            // Swap with the element that is j places away from the end
            image[i][j] = image[i][width - (1 + j)];
            image[i][width - (1 + j)] = swapTemp;
        }
    }
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    // Set up an empty 2D array to hold blurred pixel values so that blurred values do not pollute calculation of subsequent blurred values
    RGBTRIPLE blurredImage[height][width];

    // Use logic to check for i > 0, j > 0, i < height, j < width for special cases where there are fewer values to average
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            RGBTRIPLE boxBlurValues[9];
            int boxBlurIndex = 0;
            int blueSum = 0;
            int greenSum = 0;
            int redSum = 0;
            float blueAvg = 0.00;
            float greenAvg = 0.00;
            float redAvg = 0.00;
            int blueAvgRounded = 0;
            int greenAvgRounded = 0;
            int redAvgRounded = 0;

            if (i > 0)
            {
                boxBlurValues[boxBlurIndex] = image[i - 1][j];
                boxBlurIndex += 1;
            }

            if (j > 0)
            {
                boxBlurValues[boxBlurIndex] = image[i][j - 1];
                boxBlurIndex += 1;
            }

            if (i > 0 && j > 0)
            {
                boxBlurValues[boxBlurIndex] = image[i - 1][j - 1];
                boxBlurIndex += 1;
            }

            if (i > 0 && j < width - 1)
            {
                boxBlurValues[boxBlurIndex] = image[i - 1][j + 1];
                boxBlurIndex += 1;
            }

            if (j > 0 && i < height - 1)
            {
                boxBlurValues[boxBlurIndex] = image[i + 1][j - 1];
                boxBlurIndex += 1;
            }

            if (i < height - 1)
            {
                boxBlurValues[boxBlurIndex] = image[i + 1][j];
                boxBlurIndex += 1;
            }

            if (j < width - 1)
            {
                boxBlurValues[boxBlurIndex] = image[i][j + 1];
                boxBlurIndex += 1;
            }

            if (i < height - 1 && j < width - 1)
            {
                boxBlurValues[boxBlurIndex] = image[i + 1][j + 1];
                boxBlurIndex += 1;
            }

            boxBlurValues[boxBlurIndex] = image[i][j];

            // Sum the values for all red, blue and green values of surrounding pixels
            for (int z = 0; z <= boxBlurIndex; z++)
            {
                blueSum += boxBlurValues[z].rgbtBlue;
                greenSum += boxBlurValues[z].rgbtGreen;
                redSum += boxBlurValues[z].rgbtRed;
            }

            // Set the current pixel values to the average of the sums, adjusting for possible float values and rounding to nearest integer
            int boxBlurLength = boxBlurIndex + 1;

            blueAvg = ((float) blueSum) / boxBlurLength;
            greenAvg = ((float) greenSum) / boxBlurLength;
            redAvg = ((float) redSum) / boxBlurLength;

            blueAvgRounded = (int) round(blueAvg);
            greenAvgRounded = (int) round(greenAvg);
            redAvgRounded = (int) round(redAvg);

            // Update reference point with new averaged RGB values
            blurredImage[i][j].rgbtBlue = blueAvgRounded;
            blurredImage[i][j].rgbtGreen = greenAvgRounded;
            blurredImage[i][j].rgbtRed = redAvgRounded;
        }
    }

    // Copy over values from blurred to original image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j] = blurredImage[i][j];
        }
    }

    return;
}

RGBTRIPLE_INT weightedValues(int weight, RGBTRIPLE rgbObjectOriginal)
{
    RGBTRIPLE_INT rgbObject;
    if (!rgbObjectOriginal.rgbtBlue)
    {
        rgbObject.rgbtBlue = 0;
        rgbObject.rgbtGreen = 0;
        rgbObject.rgbtRed = 0;

        return rgbObject;
    }

    rgbObject.rgbtBlue = rgbObjectOriginal.rgbtBlue * weight;
    rgbObject.rgbtGreen = rgbObjectOriginal.rgbtGreen * weight;
    rgbObject.rgbtRed = rgbObjectOriginal.rgbtRed * weight;

    return rgbObject;
}

// Detect edges
// * Compute Gx and Gy for each channel of red, green and blue
// * For pixels at the border of the image, treat any pixels outside of the image as having all 0 values, as if they were completely black
// * Compute each new channel value as the square root of Gx squared plus Gy squared
// * For any newly computed value that is greater than 255, cap it at 255
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    // Set up an empty 2D array to hold edged pixel values so that edged values do not pollute calculation of subsequent edged values
    RGBTRIPLE edgedImage[height][width];
    // Setup 2D array for Gx and Gy that matches the following
    int gx[3][3] =
    {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    int gy[3][3] =
    {
        {-1, -2, -1},
        {0, 0, 0},
        {1, 2, 1}
    };


    // Use logic to check for i > 0, j > 0, i < height, j < width for special cases where there are fewer values to average
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            RGBTRIPLE_INT gxBoxEdgeValues[9];
            RGBTRIPLE_INT gyBoxEdgeValues[9];
            RGBTRIPLE nullValues = {};
            int boxEdgeIndex = 0;
            int gxBlueSum = 0;
            int gxGreenSum = 0;
            int gxRedSum = 0;
            int gyBlueSum = 0;
            int gyGreenSum = 0;
            int gyRedSum = 0;
            float blueAvg = 0.00;
            float greenAvg = 0.00;
            float redAvg = 0.00;
            int blueAvgRounded = 0;
            int greenAvgRounded = 0;
            int redAvgRounded = 0;

            if (i > 0)
            {
                // [refX - 1][refY]
                gxBoxEdgeValues[boxEdgeIndex] = weightedValues(gx[0][1], image[i - 1][j]);
                gyBoxEdgeValues[boxEdgeIndex] = weightedValues(gy[0][1], image[i - 1][j]);

                boxEdgeIndex += 1;
            }
            else
            {
                gxBoxEdgeValues[boxEdgeIndex] = weightedValues(gx[0][1], nullValues);
                gyBoxEdgeValues[boxEdgeIndex] = weightedValues(gy[0][1], nullValues);

                boxEdgeIndex += 1;
            }

            if (j > 0)
            {
                // [refX][refY - 1]
                gxBoxEdgeValues[boxEdgeIndex] = weightedValues(gx[1][0], image[i][j - 1]);
                gyBoxEdgeValues[boxEdgeIndex] = weightedValues(gy[1][0], image[i][j - 1]);

                boxEdgeIndex += 1;
            }
            else
            {
                gxBoxEdgeValues[boxEdgeIndex] = weightedValues(gx[1][0], nullValues);
                gyBoxEdgeValues[boxEdgeIndex] = weightedValues(gy[1][0], nullValues);

                boxEdgeIndex += 1;
            }

            if (i > 0 && j > 0)
            {
                // [refX - 1][refY - 1]
                gxBoxEdgeValues[boxEdgeIndex] = weightedValues(gx[0][0], image[i - 1][j - 1]);
                gyBoxEdgeValues[boxEdgeIndex] = weightedValues(gy[0][0], image[i - 1][j - 1]);

                boxEdgeIndex += 1;
            }
            else
            {
                gxBoxEdgeValues[boxEdgeIndex] = weightedValues(gx[0][0], nullValues);
                gyBoxEdgeValues[boxEdgeIndex] = weightedValues(gy[0][0], nullValues);

                boxEdgeIndex += 1;
            }

            if (i > 0 && j < width - 1)
            {
                // [refX - 1][refY + 1]
                gxBoxEdgeValues[boxEdgeIndex] = weightedValues(gx[0][2], image[i - 1][j + 1]);
                gyBoxEdgeValues[boxEdgeIndex] = weightedValues(gy[0][2], image[i - 1][j + 1]);

                boxEdgeIndex += 1;
            }
            else
            {
                gxBoxEdgeValues[boxEdgeIndex] = weightedValues(gx[0][2], nullValues);
                gyBoxEdgeValues[boxEdgeIndex] = weightedValues(gy[0][2], nullValues);

                boxEdgeIndex += 1;
            }

            if (j > 0 && i < height - 1)
            {
                // [refX + 1][refY - 1]
                gxBoxEdgeValues[boxEdgeIndex] = weightedValues(gx[2][0], image[i + 1][j - 1]);
                gyBoxEdgeValues[boxEdgeIndex] = weightedValues(gy[2][0], image[i + 1][j - 1]);

                boxEdgeIndex += 1;
            }
            else
            {
                gxBoxEdgeValues[boxEdgeIndex] = weightedValues(gx[2][0], nullValues);
                gyBoxEdgeValues[boxEdgeIndex] = weightedValues(gy[2][0], nullValues);

                boxEdgeIndex += 1;
            }

            if (i < height - 1)
            {
                // [refX + 1][refY]
                gxBoxEdgeValues[boxEdgeIndex] = weightedValues(gx[2][1], image[i + 1][j]);
                gyBoxEdgeValues[boxEdgeIndex] = weightedValues(gy[2][1], image[i + 1][j]);

                boxEdgeIndex += 1;
            }
            else
            {
                gxBoxEdgeValues[boxEdgeIndex] = weightedValues(gx[2][1], nullValues);
                gyBoxEdgeValues[boxEdgeIndex] = weightedValues(gy[2][1], nullValues);

                boxEdgeIndex += 1;
            }

            if (j < width - 1)
            {
                // [refX][refY + 1]
                gxBoxEdgeValues[boxEdgeIndex] = weightedValues(gx[1][2], image[i][j + 1]);
                gyBoxEdgeValues[boxEdgeIndex] = weightedValues(gy[1][2], image[i][j + 1]);

                boxEdgeIndex += 1;
            }
            else
            {
                gxBoxEdgeValues[boxEdgeIndex] = weightedValues(gx[1][2], nullValues);
                gyBoxEdgeValues[boxEdgeIndex] = weightedValues(gy[1][2], nullValues);

                boxEdgeIndex += 1;
            }

            if (i < height - 1 && j < width - 1)
            {
                // [refX + 1][refY + 1]
                gxBoxEdgeValues[boxEdgeIndex] = weightedValues(gx[2][2], image[i + 1][j + 1]);
                gyBoxEdgeValues[boxEdgeIndex] = weightedValues(gy[2][2], image[i + 1][j + 1]);

                boxEdgeIndex += 1;
            }
            else
            {
                gxBoxEdgeValues[boxEdgeIndex] = weightedValues(gx[2][2], nullValues);
                gyBoxEdgeValues[boxEdgeIndex] = weightedValues(gy[2][2], nullValues);

                boxEdgeIndex += 1;
            }

            // [refX][refY]
            gxBoxEdgeValues[boxEdgeIndex] = weightedValues(gx[1][1], image[i][j]);
            gyBoxEdgeValues[boxEdgeIndex] = weightedValues(gy[1][1], image[i][j]);

            // Sum the values for all red, blue and green values of surrounding pixels
            for (int z = 0; z <= boxEdgeIndex; z++)
            {
                gxBlueSum += gxBoxEdgeValues[z].rgbtBlue;
                gxGreenSum += gxBoxEdgeValues[z].rgbtGreen;
                gxRedSum += gxBoxEdgeValues[z].rgbtRed;

                gyBlueSum += gyBoxEdgeValues[z].rgbtBlue;
                gyGreenSum += gyBoxEdgeValues[z].rgbtGreen;
                gyRedSum += gyBoxEdgeValues[z].rgbtRed;
            }

            // Combine Gx and Gy values by squaring them, adding the squares and calculating the square root of the combined value
            blueAvg = (float) sqrt((gxBlueSum * gxBlueSum) + (gyBlueSum * gyBlueSum));
            greenAvg = (float) sqrt((gxGreenSum * gxGreenSum) + (gyGreenSum * gyGreenSum));
            redAvg = (float) sqrt((gxRedSum * gxRedSum) + (gyRedSum * gyRedSum));

            // Round averages
            blueAvgRounded = (int) round(blueAvg);
            greenAvgRounded = (int) round(greenAvg);
            redAvgRounded = (int) round(redAvg);

            // Cap values at 255 since RGB values have a maximum of 255
            if (blueAvgRounded > 255)
            {
                blueAvgRounded = 255;
            }

            if (greenAvgRounded > 255)
            {
                greenAvgRounded = 255;
            }

            if (redAvgRounded > 255)
            {
                redAvgRounded = 255;
            }

            // Update reference point with new averaged RGB values
            edgedImage[i][j].rgbtBlue = blueAvgRounded;
            edgedImage[i][j].rgbtGreen = greenAvgRounded;
            edgedImage[i][j].rgbtRed = redAvgRounded;
        }
    }

    // Copy over values from blurred to original image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j] = edgedImage[i][j];
        }
    }

    return;
}
