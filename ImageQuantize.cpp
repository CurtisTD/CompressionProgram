//#DEFINE IMAGEQUANTIZE_HPP
#include <fstream>
#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <iterator>

std::vector<char> quantizeBMP(const std::string &file)
{
    static constexpr size_t HEADER_SIZE = 54;

    std::ifstream inputImage(file, std::ios::binary);

    //Get BMFILEHEADER
    std::array<char, HEADER_SIZE> header;
    inputImage.read(header.data(), header.size());

    /* 
    * Gets BMP File Header Byte by Byte (54 Bytes)
    * ***Bytes#, description***
    * 2-5 bfSize -> Size of file in bytes
    * 10-13 Offset from file beginning to the bitmap data in bits
    * **Then gets BMP Info Header**
    * 18-21 Width of image, in pixels
    * 22-25 Height of image, in pixels
    * 28-29 Number of bits per pixel (default: 8)
    * 30-33 Type of compression on image
    */
    auto fileSize = *reinterpret_cast<uint32_t *>(&header[2]);
    auto dataOffset = *reinterpret_cast<uint32_t *>(&header[10]);
    auto width = *reinterpret_cast<uint32_t *>(&header[18]);
    auto height = *reinterpret_cast<uint32_t *>(&header[22]);
    auto depth = *reinterpret_cast<uint16_t *>(&header[28]);

    //File info
    std::cout << "File Size: " << fileSize << std::endl;
    std::cout << "Data Offset: " << dataOffset << std::endl;
    std::cout << "Width: " << width << std::endl;
    std::cout << "Height: " << height << std::endl;
    std::cout << "Depth: " << depth << "-bit" << std::endl;

    //Sets a vector for BMP Info Header
    std::vector<char> img(dataOffset - HEADER_SIZE);
    inputImage.read(img.data(), img.size()); 

    //Reads data for the picture pixels
    auto dataSize = ((width * 3 + 3) & (~3)) * height;
    img.resize(dataSize);
    inputImage.read(img.data(), img.size());//Reads the data

    char temp = 0;

    //Reads from bottom right pixel to top left
    for (int i = 0; i <= dataSize - 4; i += 3) {
        temp = img[i];
        img[i] = img[i+2];
        img[i+2] = temp;

        //Beginning of line
        std::cout << (i/3) << ":" << " R: " << int(img[i] & 0xff) << " G: " << int(img[i+1] & 0xff) << " B: " << int(img[i+2] & 0xff) << std::endl;
    }

    return img;
}