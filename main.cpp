#include <iostream>
#include <stdlib.h>
#include "libbmp/global.h"

using namespace std;

bool pixelate(const char* inputFile, int ratio, const char* outputFile)
{
    unsigned int currentDword = 0;
    unsigned int pixelisationRatio = 0;
    Bitmap aBitmap;
    std::vector<Bitmap::PIXEL_24BPP> newImageData;

    if(!aBitmap.openFromFile(inputFile))
    {
        cout << "Unable to open " << inputFile << "... Exiting.";
        return false;
    }

    //display some information about the file
    cout << "*** INPUT FILE INFORMATIONS ***" << endl;
    cout << inputFile << " opened!" << endl;
    cout << "magic word: " << (char)aBitmap.getBitmapFileHeader().bfType << endl;
    cout << "file size: " << aBitmap.getBitmapFileHeader().bfSize << " bytes." << endl;
    cout << "Pixel array start offset: " << aBitmap.getBitmapFileHeader().bfOffBits << endl;
    cout << "bfReserved1: " << aBitmap.getBitmapFileHeader().bfReserved1 << endl;
    cout << "bfReserved2: " << aBitmap.getBitmapFileHeader().bfReserved2 << endl;

    cout << "height: " << aBitmap.getBitmapInfoHeader().biHeight << " width: " << aBitmap.getBitmapInfoHeader().biWidth << endl;
    cout << "Bits per pixel: " << aBitmap.getBitmapInfoHeader().biBitCount << endl;
    cout << "biYPelsPerMeter: " << aBitmap.getBitmapInfoHeader().biYPelsPerMeter << endl;
    cout << "biXPelsPerMeter: " << aBitmap.getBitmapInfoHeader().biXPelsPerMeter << endl;
    cout << "biPlanes: " << aBitmap.getBitmapInfoHeader().biPlanes << endl;
    cout << "biCompression: " << aBitmap.getBitmapInfoHeader().biCompression << endl;
    cout << "biSizeImage: " << aBitmap.getBitmapInfoHeader().biSizeImage << endl;
    cout << "biClrUsed: " << aBitmap.getBitmapInfoHeader().biClrUsed << endl;

    //here modifiy the image and put and put in aNewBitmap
    newImageData = aBitmap.getPixelData();

    pixelisationRatio = ratio;

    for(unsigned int iCurrentPixel = 0; iCurrentPixel < newImageData.size(); iCurrentPixel+=pixelisationRatio)
    {
        Bitmap::PIXEL_24BPP sourcePixel = newImageData[iCurrentPixel];

        for(unsigned int i = 0; i < pixelisationRatio; i++)
        {
            if(iCurrentPixel+i < newImageData.size())
                newImageData[iCurrentPixel+i] = sourcePixel;
        }

        for(unsigned int i = 0; i < pixelisationRatio; i++)
        {
            if(iCurrentPixel+aBitmap.getBitmapInfoHeader().biWidth+i < newImageData.size())
                newImageData[iCurrentPixel+aBitmap.getBitmapInfoHeader().biWidth+i] = sourcePixel;
        }

        currentDword ++;

        if(currentDword == aBitmap.getBitmapInfoHeader().biWidth)
        {
            iCurrentPixel += aBitmap.getBitmapInfoHeader().biWidth;
            currentDword = 0;
        }
    }

    //Then create new bitmap from array data
    /*
    cout << endl << "*** OUTPUT FILE INFORMATIONS ***" << endl;
    Bitmap outputBitmap(newImageData, aBitmap.getBitmapInfoHeader().biWidth, aBitmap.getBitmapInfoHeader().biHeight, outputFile);

    cout << outputFile << " opened!" << endl;
    cout << "magic word: " << (char)outputBitmap.getBitmapFileHeader().bfType << endl;
    cout << "file size: " << outputBitmap.getBitmapFileHeader().bfSize << " bytes." << endl;
    cout << "Pixel array start offset: " << outputBitmap.getBitmapFileHeader().bfOffBits << endl;
    cout << "bfReserved1: " << outputBitmap.getBitmapFileHeader().bfReserved1 << endl;
    cout << "bfReserved2: " << outputBitmap.getBitmapFileHeader().bfReserved2 << endl;

    cout << "height: " << outputBitmap.getBitmapInfoHeader().biHeight << " width: " << outputBitmap.getBitmapInfoHeader().biWidth << endl;
    cout << "Bits per pixel: " << outputBitmap.getBitmapInfoHeader().biBitCount << endl;
    cout << "biYPelsPerMeter: " << outputBitmap.getBitmapInfoHeader().biYPelsPerMeter << endl;
    cout << "biXPelsPerMeter: " << outputBitmap.getBitmapInfoHeader().biXPelsPerMeter << endl;
    cout << "biPlanes: " << outputBitmap.getBitmapInfoHeader().biPlanes << endl;
    cout << "biCompression: " << outputBitmap.getBitmapInfoHeader().biCompression << endl;
    cout << "biSizeImage: " << outputBitmap.getBitmapInfoHeader().biSizeImage << endl;
    cout << "biClrUsed: " << outputBitmap.getBitmapInfoHeader().biClrUsed << endl;*/

    return true;
}

int main(int argc, char* argv[])
{

    if(argc > 3)
    {
        pixelate(argv[1], atoi(argv[2]), argv[3]);
    }
    else
        cout << "Error, correct usage Pixefy.exe <inputFile> <pixelisationRatio> <outputFile>" << endl;

    return 0;
}
