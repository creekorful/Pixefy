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
        BITMAPINFOHEADER bitmapV5Header;
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
