#ifndef LIBBMP_H
#define LIBBMP_H
#include <iostream>
#include <stdio.h>
#include <vector>

#define BI_RGB 0


typedef unsigned short WORD;
typedef unsigned long DWORD;

class Bitmap
{
public:
    //structures
    struct PIXEL_24BPP //24BPP = 3 bytes per pixel (3*8 = 24 bit)
    {
        unsigned char blue;
        unsigned char green;
        unsigned char red;
    };

    typedef struct tagBITMAPFILEHEADER
    {
        WORD bfType;
        DWORD bfSize;
        WORD bfReserved1;
        WORD bfReserved2;
        DWORD bfOffBits;
    } BITMAPFILEHEADER, *PBITMAPFILEHEADER;

    typedef struct tagBITMAPINFOHEADER
    {
        DWORD biSize;
        long biWidth;
        long biHeight;
        WORD biPlanes;
        WORD biBitCount;
        DWORD biCompression;
        DWORD biSizeImage;
        long biXPelsPerMeter;
        long biYPelsPerMeter;
        DWORD biClrUsed;
        DWORD biClrImportant;
    } BITMAPINFOHEADER, *PBITMAPINFOHEADER;

    struct BITMAP_HEADERS
    {
        BITMAPFILEHEADER bitmapFileHeader;
        BITMAPINFOHEADER bitmapInfoHeader;
    };

    struct BITMAP_FILE_FORMAT
    {
        BITMAP_HEADERS bitmapHeaders;
        std::vector<PIXEL_24BPP> imageData;
    };

    //class
    Bitmap();
    Bitmap(std::vector<PIXEL_24BPP> aDataArray, long width, long height, std::string outputFile); //create bitmap from pixel array
    bool openFromFile(std::string aFilePath);
    BITMAPFILEHEADER getBitmapFileHeader();
    BITMAPINFOHEADER getBitmapInfoHeader();
    std::vector<PIXEL_24BPP> getPixelData();
    void setPixelData(std::vector<PIXEL_24BPP> aDataArray);
    BITMAP_FILE_FORMAT getFile(); //todo
    ~Bitmap();

private:
    std::string m_filePath;
    FILE* m_pBitmapFile;
    BITMAPFILEHEADER m_bitmapFileHeader;
    BITMAPINFOHEADER m_bitmapInfoHeader;
    std::vector<PIXEL_24BPP> m_imageData;
};

//****************************************************************************************************************************************************************************

Bitmap::Bitmap()
{
    m_pBitmapFile = NULL;
}

Bitmap::Bitmap(std::vector<Bitmap::PIXEL_24BPP> aDataArray, long width, long height, std::string outputFile)
{
    m_pBitmapFile = fopen(outputFile.c_str(), "w+");

    //Set up bitmap file header
    m_bitmapFileHeader.bfType = 0x4D42; //magic word
    m_bitmapFileHeader.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+(aDataArray.size()*3); // *3 because 3 bytes per pixel
    m_bitmapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER); //value is 54
    m_bitmapFileHeader.bfReserved1 = 0;
    m_bitmapFileHeader.bfReserved2 = 0;

    //Set up bitmap info header
    m_bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    m_bitmapInfoHeader.biHeight = height;
    m_bitmapInfoHeader.biWidth = width;
    m_bitmapInfoHeader.biPlanes = 1;
    m_bitmapInfoHeader.biBitCount = 24;
    m_bitmapInfoHeader.biCompression = BI_RGB;
    m_bitmapInfoHeader.biSizeImage = (aDataArray.size()*3);
    m_bitmapInfoHeader.biYPelsPerMeter = width*7; //todo real value
    m_bitmapInfoHeader.biXPelsPerMeter = m_bitmapInfoHeader.biYPelsPerMeter-10; //todo real value
    m_bitmapInfoHeader.biClrUsed = 0;
    m_bitmapInfoHeader.biClrImportant = 0;

    int neededPadding = (m_bitmapInfoHeader.biWidth) % 4; //todo implement padding

    fwrite(&m_bitmapFileHeader, sizeof(m_bitmapFileHeader), 1, m_pBitmapFile);
    fwrite(&m_bitmapInfoHeader, sizeof(m_bitmapInfoHeader), 1, m_pBitmapFile);

    for(unsigned int iCurrentPixel = 0; iCurrentPixel < aDataArray.size(); iCurrentPixel++) //finally write pixel array
    {
        fwrite(&aDataArray[iCurrentPixel], sizeof(Bitmap::PIXEL_24BPP), 1, m_pBitmapFile);
    }

    fclose(m_pBitmapFile);
}

bool Bitmap::openFromFile(std::string aFilePath)
{
    m_filePath = aFilePath;
    m_pBitmapFile = fopen(aFilePath.c_str(), "r");

    if(m_pBitmapFile != NULL) //file opened
    {
        fread(&m_bitmapFileHeader, sizeof(m_bitmapFileHeader), 1, m_pBitmapFile);
        fread(&m_bitmapInfoHeader, sizeof(m_bitmapInfoHeader), 1, m_pBitmapFile);
        std::cout << "There is: " << (m_bitmapInfoHeader.biHeight*m_bitmapInfoHeader.biWidth) << " pixels (" << (m_bitmapInfoHeader.biHeight*m_bitmapInfoHeader.biWidth)*3 << " bytes)." << std::endl;
        fseek(m_pBitmapFile, m_bitmapFileHeader.bfOffBits, SEEK_SET);

        for(int iCurrentPixel = 0; iCurrentPixel < (m_bitmapInfoHeader.biHeight*m_bitmapInfoHeader.biWidth); iCurrentPixel++)
        {
            PIXEL_24BPP currentPixel;
            fread(&currentPixel, sizeof(Bitmap::PIXEL_24BPP), 1, m_pBitmapFile);
            m_imageData.push_back(currentPixel);
        }

        fclose(m_pBitmapFile);

        return true;
    }
    else
        return false;
}

Bitmap::BITMAPFILEHEADER Bitmap::getBitmapFileHeader()
{
    return m_bitmapFileHeader;
}

Bitmap::BITMAPINFOHEADER Bitmap::getBitmapInfoHeader()
{
    return m_bitmapInfoHeader;
}

std::vector<Bitmap::PIXEL_24BPP> Bitmap::getPixelData()
{
    return m_imageData;
}

void Bitmap::setPixelData(std::vector<Bitmap::PIXEL_24BPP> aDataArray)
{
    m_pBitmapFile = fopen(m_filePath.c_str(), "rw+");
    fseek(m_pBitmapFile, sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+54, SEEK_SET);

    for(unsigned int iCurrentPixel = 0; iCurrentPixel < aDataArray.size(); iCurrentPixel++)
    {
        fwrite(&aDataArray[iCurrentPixel], sizeof(Bitmap::PIXEL_24BPP), 1, m_pBitmapFile);
    }

    fclose(m_pBitmapFile);
}

Bitmap::~Bitmap()
{
    //dtor
}

#endif // LIBBMP_H
