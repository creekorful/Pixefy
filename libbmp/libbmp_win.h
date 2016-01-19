#ifndef LIBBMP_H
#define LIBBMP_H
#include <iostream>
#include <windows.h>
#include <vector>

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

    struct BITMAP_FILE_FORMAT
    {
        BITMAPFILEHEADER bitmapFileHeader;
        BITMAPINFOHEADER bitmapInfoHeader;
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
    ~Bitmap();

private:
    std::string m_filePath;
    HANDLE m_bitmapFileHandle;
    BITMAPFILEHEADER m_bitmapFileHeader;
    BITMAPINFOHEADER m_bitmapInfoHeader;
    std::vector<PIXEL_24BPP> m_imageData;
};

#endif // LIBBMP_H

//****************************************************************************************************************************************************************************

Bitmap::Bitmap()
{
    //ctor
}

Bitmap::Bitmap(std::vector<Bitmap::PIXEL_24BPP> aDataArray, long width, long height, std::string outputFile)
{
    HANDLE hNewFile = CreateFile(outputFile.c_str(), GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

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
    DWORD dwBytesWritted;
    WriteFile(hNewFile, &m_bitmapFileHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritted, NULL); //Write bitmap file header
    WriteFile(hNewFile, &m_bitmapInfoHeader, sizeof(BITMAPINFOHEADER), &dwBytesWritted, NULL); //Write bitmap info header

    for(unsigned int iCurrentPixel = 0; iCurrentPixel < aDataArray.size(); iCurrentPixel++) //finally write pixel array
    {
        WriteFile(hNewFile, &aDataArray[iCurrentPixel], sizeof(Bitmap::PIXEL_24BPP), &dwBytesWritted, NULL);
    }

    CloseHandle(hNewFile);
}

bool Bitmap::openFromFile(std::string aFilePath)
{
    m_filePath = aFilePath;
    m_bitmapFileHandle = CreateFile(aFilePath.c_str(), GENERIC_ALL, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if(m_bitmapFileHandle != INVALID_HANDLE_VALUE)
    {
        DWORD dwTotalBytesRead = 0;
        DWORD dwBytesRead;
        DWORD currentOffset = 0;
        ReadFile(m_bitmapFileHandle, &m_bitmapFileHeader, sizeof(m_bitmapFileHeader), &dwBytesRead, NULL);
        currentOffset += dwBytesRead;
        ReadFile(m_bitmapFileHandle, &m_bitmapInfoHeader, sizeof(m_bitmapInfoHeader), &dwBytesRead, NULL);
        currentOffset += dwBytesRead;
        std::cout << "There is: " << (m_bitmapInfoHeader.biHeight*m_bitmapInfoHeader.biWidth) << " pixels (" << (m_bitmapInfoHeader.biHeight*m_bitmapInfoHeader.biWidth)*3 << " bytes)." << std::endl;

        SetFilePointer(m_bitmapFileHandle, m_bitmapFileHeader.bfOffBits, NULL, FILE_BEGIN); //set file pointer to pixel array start and remove 84 bytes

        for(int iCurrentPixel = 0; iCurrentPixel < (m_bitmapInfoHeader.biHeight*m_bitmapInfoHeader.biWidth); iCurrentPixel++)
        {
            PIXEL_24BPP currentPixel;
            ReadFile(m_bitmapFileHandle, &currentPixel, sizeof(PIXEL_24BPP), &dwBytesRead, NULL);
            dwTotalBytesRead += dwBytesRead;
            currentOffset += dwBytesRead;
            m_imageData.push_back(currentPixel);
        }

        std::cout << "Final offset: " << currentOffset << std::endl;
        std::cout<<std::endl;
        CloseHandle(m_bitmapFileHandle);
        return true;
    }
    else
        return false;
}

BITMAPFILEHEADER Bitmap::getBitmapFileHeader()
{
    return m_bitmapFileHeader;
}

BITMAPINFOHEADER Bitmap::getBitmapInfoHeader()
{
    return m_bitmapInfoHeader;
}

std::vector<Bitmap::PIXEL_24BPP> Bitmap::getPixelData()
{
    return m_imageData;
}

void Bitmap::setPixelData(std::vector<Bitmap::PIXEL_24BPP> aDataArray)
{
    m_bitmapFileHandle = CreateFile(m_filePath.c_str(), GENERIC_ALL, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    SetFilePointer(m_bitmapFileHandle, sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+54, 0, FILE_BEGIN);

    DWORD dwBytesWritted;
    for(unsigned int iCurrentPixel = 0; iCurrentPixel < aDataArray.size(); iCurrentPixel++)
    {
        WriteFile(m_bitmapFileHandle, &aDataArray[iCurrentPixel], sizeof(Bitmap::PIXEL_24BPP), &dwBytesWritted, NULL);
    }

    CloseHandle(m_bitmapFileHandle);
}

Bitmap::~Bitmap()
{
    //dtor
}
