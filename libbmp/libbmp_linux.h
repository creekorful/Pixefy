#ifndef LIBBMP_H
#define LIBBMP_H
#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
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
    BITMAP_FILE_FORMAT getFile();
    ~Bitmap();

private:
    std::string m_filePath;
    int m_bitmapFileDescriptor;
    BITMAPFILEHEADER m_bitmapFileHeader;
    BITMAPINFOHEADER m_bitmapInfoHeader;
    std::vector<PIXEL_24BPP> m_imageData;
};

//****************************************************************************************************************************************************************************

Bitmap::Bitmap()
{
    //ctor
}

Bitmap::Bitmap(std::vector<Bitmap::PIXEL_24BPP> aDataArray, long width, long height, std::string outputFile)
{
    m_bitmapFileDescriptor = open(outputFile.c_str(), O_RDWR | O_CREAT);

    //Set up bitmap file header
    m_bitmapFileHeader.bfType = 0x4D42; //magic word
    m_bitmapFileHeader.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+(aDataArray.size()*3); // *3 because 3 bytes for 1 pixel
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

    int neededPadding = (m_bitmapInfoHeader.biWidth) % 4;

    write(m_bitmapFileDescriptor, &m_bitmapFileHeader, sizeof(Bitmap::BITMAPFILEHEADER));
    write(m_bitmapFileDescriptor, &m_bitmapInfoHeader, sizeof(Bitmap::BITMAPINFOHEADER));

    for(unsigned int iCurrentPixel = 0; iCurrentPixel < aDataArray.size(); iCurrentPixel++) //finally write pixel array
    {
        write(m_bitmapFileDescriptor, &aDataArray[iCurrentPixel], sizeof(Bitmap::PIXEL_24BPP));
    }

    //CloseHandle(hNewFile);
    close(m_bitmapFileDescriptor);
}

bool Bitmap::openFromFile(std::string aFilePath)
{
    m_filePath = aFilePath;
    m_bitmapFileDescriptor = open(aFilePath.c_str(), O_RDONLY);

    if(m_bitmapFileDescriptor != -1) //file opened
    {
        DWORD dwTotalBytesRead = 0;
        DWORD dwBytesRead = 0;
        DWORD currentOffset = 0;
        read(m_bitmapFileDescriptor, &m_bitmapFileHeader, sizeof(m_bitmapFileHeader));
        currentOffset += dwBytesRead;
        read(m_bitmapFileDescriptor, &m_bitmapInfoHeader, sizeof(m_bitmapInfoHeader));
        currentOffset += dwBytesRead;
        std::cout << "There is: " << (m_bitmapInfoHeader.biHeight*m_bitmapInfoHeader.biWidth) << " pixels (" << (m_bitmapInfoHeader.biHeight*m_bitmapInfoHeader.biWidth)*3 << " bytes)." << std::endl;

        lseek(m_bitmapFileDescriptor, SEEK_SET, m_bitmapFileHeader.bfOffBits);

        for(int iCurrentPixel = 0; iCurrentPixel < (m_bitmapInfoHeader.biHeight*m_bitmapInfoHeader.biWidth); iCurrentPixel++)
        {
            PIXEL_24BPP currentPixel;
            read(m_bitmapFileDescriptor, &currentPixel, sizeof(PIXEL_24BPP));
            dwTotalBytesRead += dwBytesRead;
            currentOffset += dwBytesRead;
            m_imageData.push_back(currentPixel);
        }

        std::cout << "Final offset: " << currentOffset << std::endl;
        std::cout<<std::endl;

        close(m_bitmapFileDescriptor);

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
    m_bitmapFileDescriptor = open(m_filePath.c_str(), O_RDONLY);
    lseek(m_bitmapFileDescriptor, SEEK_SET, sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+54);

    for(unsigned int iCurrentPixel = 0; iCurrentPixel < aDataArray.size(); iCurrentPixel++)
    {
        write(m_bitmapFileDescriptor, &aDataArray[iCurrentPixel], sizeof(Bitmap::PIXEL_24BPP));
    }

    //CloseHandle(m_bitmapFileHandle);
    close(m_bitmapFileDescriptor);
}

Bitmap::~Bitmap()
{
    //dtor
}

#endif // LIBBMP_H
