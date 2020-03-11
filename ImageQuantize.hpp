#ifndef IMAGEQUANTIZE_HPP
#define IMAGEQUANTIZE_HPP

#include <fstream>
#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <iterator>
#include <iomanip>
#include <cmath> //round()
#include "BitMapFunctions.hpp"

#define HUE_AMOUNT 30 //Amount per segment of 360 hue colors (e.g 20 gives 18 segments)

//Object to represent RGB of a pixel
struct RgbPix {
    double r;
    double g;
    double b;
};

//Object to represent HSV of pixel
struct HsvPix {
    double h; //Hue
    double s; //Saturation
    double v; //Value

    //To keep x,y location of image
    int x = 0;
    int y = 0;
};
struct hsvCmp {
    //Comparator for key value of x and y location
    bool operator()(const HsvPix& lhs, const HsvPix &rhs) const {
        return lhs.x < rhs.x  || lhs.y < rhs.y;
    }
};

//Object to keep x,y,z location inside a color space
struct LocMarker {
    int x;
    int y;
    int z;
};

/*
* HSV to RGB
* Converts HSV value to RGB
*/
RgbPix hsv2rgb(HsvPix in){
    RgbPix rgbRet;
    rgbRet.r = 0;
    rgbRet.g = 0;
    rgbRet.b = 0;

    double h_prime = in.h / 60;
    double chroma = in.v * in.s;
    
    double x = chroma * (1 - abs( std::fmod(h_prime, 2.0) - 1));
    double m = in.v - chroma;

    if(h_prime <= 1) {
        rgbRet.r = chroma;
        rgbRet.g = x;
        rgbRet.b = 0;
    } else if (h_prime <= 2) {
        rgbRet.r = x;
        rgbRet.g = chroma;
        rgbRet.b = 0;
    } else if(h_prime <= 3) {
        rgbRet.r = 0;
        rgbRet.g = chroma;
        rgbRet.b = x;
    } else if (h_prime <= 4) {
        rgbRet.r = 0;
        rgbRet.g = x;
        rgbRet.b = chroma;
    } else if(h_prime <= 5) {
        rgbRet.r = x;
        rgbRet.g = 0;
        rgbRet.b = chroma;
    } else if (h_prime <= 6) {
        rgbRet.r = chroma;
        rgbRet.g = 0;
        rgbRet.b = x;
    }

    //Set to valid RGB ranges
    rgbRet.r = (rgbRet.r + m) * 255;
    rgbRet.g = (rgbRet.g + m) * 255;
    rgbRet.b = (rgbRet.b + m) * 255;

    return rgbRet;
}


/*
* RGB to HSV
* Converts RGB value to HSV
*/
HsvPix rgb2hsv(RgbPix in){
    double r = in.r / 255; //Range of color, fraction
    double g = in.g / 255;
    double b = in.b / 255;

    HsvPix hsvResult; //HSV of pixel to return
    double min, max, delta;

    //Min and max values based on rgb inputs
    min = std::min(std::min(r, g), b);
    max = std::max(std::max(r, g), b);
    hsvResult.v = max;
    delta = max - min;

    if( max != 0 ) {
        hsvResult.s = delta / max;
    } else {
        // r = g = b = 0
        hsvResult.s = 0;
        hsvResult.h = -1;
        return hsvResult;
    }

    if(r == max) { 
        hsvResult.h = (g - b) / delta;
    } else if (g == max){
        hsvResult.h = 2 + (b - r) / delta;
    } else {
        hsvResult.h = 4 + (r - g) / delta;
    }
    hsvResult.h *= 60; //Degrees
    if(hsvResult.h < 0) {
        hsvResult.h += 360; //Corrects negative
    }
    
    return hsvResult;
}


/*
* QuantizeBMP
* Image processing function that quantizes a BMP file
*/
void quantizeBMP(const std::string &file) {

    //Input stream of input image
    std::ifstream inputImage(file, std::ios::binary);

    static constexpr size_t HEADER_SIZE = 54; //Size of header offset in bits

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
    auto dataOffset = *reinterpret_cast<uint32_t *>(&header[10]);
    auto width = *reinterpret_cast<uint32_t *>(&header[18]);
    auto height = *reinterpret_cast<uint32_t *>(&header[22]);

    //Sets a vector for BMP Info Header
    std::vector<char> img(dataOffset - HEADER_SIZE);
    inputImage.read(img.data(), img.size()); 

    //Reads data for the picture pixels
    auto dataSize = ((width * 3 + 3) & (~3)) * height;
    img.resize(dataSize);
    inputImage.read(img.data(), img.size()); //Reads the pixel data into img
 
    //Dictionary of objects to convert after reading the image
    std::map<HsvPix, LocMarker, hsvCmp> imageHsvLoc;
    //Dictionary to save each pixel's x,y and HSV values 

    //Reads from bottom left pixel to top right, 3 values at a time.
    //Currently reads bottom line first, left to right, then goes up a row.
    char temp = 0;
    int col = 0;
    int row = height;
    for (int i = 0; i <= (int)(dataSize - 1); i += 3) {
        //Corrects BGR to RGB
        temp = img[i];
        img[i] = img[i+2];
        img[i+2] = temp;

        col = (i/3) % width;
        //Finishes a row
        if(col == 0) {
            row--;
        }

        //Set a temp rgb value for hsv conversion
        RgbPix tempRgbPix;
        tempRgbPix.r = int(img[i] & 0xff);
        tempRgbPix.g = int(img[i+1] & 0xff);
        tempRgbPix.b = int(img[i+2] & 0xff);

        //Pixel RGB value        
        HsvPix tempHsvPix = rgb2hsv(tempRgbPix);

        tempHsvPix.x = col; //Saves pixel location info in image
        tempHsvPix.y = row;

        //Put pixels into similar 'Buckets'
        //3D Color space buckets consolidate into one prevalent color        
        //Marker for pixel location in the color space
        LocMarker tempLoc;

        /* 
        * Hues segmented into 18 ranges, 360/18 = 20 values per segment
        * Saturation into 3 ranges 100%/3 = ~33 values per segment
        * Value into 3 ranges 100%/3 = ~33 values per segment 
        */
        //Segmenting and putting values into buckets/bins
        tempLoc.x = tempHsvPix.h / HUE_AMOUNT; //18 segments (values of 0->19)
        tempLoc.y = (tempHsvPix.s * 100.0) / 33.0;
        tempLoc.z = (tempHsvPix.v * 100.0) / 33.0;

        //Save each pixels location in the colorspace and actual location
        imageHsvLoc.insert( std::make_pair(tempHsvPix, tempLoc) );        
    }
    
    /********Write new BMP image********/
    unsigned char image[height][width][bytesPerPixel]; //Structure to hold new values
    /*****Quantize the image from their locations in the color space*****/
    std::map<HsvPix, LocMarker>::iterator it = imageHsvLoc.begin();

    //Temp RGB for new image pixels, to be modified
    HsvPix tempHSV;
    RgbPix tempRGB;
    for (it = imageHsvLoc.begin(); it != imageHsvLoc.end(); ++it) {
        double quantH, quantS, quantV;
        quantH = it->second.x * HUE_AMOUNT; //Gives new Hue to write
        quantS = (it->second.y * 0.33); //Gives new Saturaton to write
        quantV = (it->second.z * 0.33); //Gives new Value to write

        tempHSV.h = quantH; //Holds temp HSV hue value
        tempHSV.s = quantS; //Holds temp HSV saturation value
        tempHSV.v = quantV; //Holds temp HSV intensity value
        tempRGB = hsv2rgb(tempHSV);

        image[it->first.y][it->first.x][2] = (unsigned char)((double) tempRGB.r); ///red
        image[it->first.y][it->first.x][1] = (unsigned char)((double) tempRGB.g); ///green
        image[it->first.y][it->first.x][0] = (unsigned char)((double) tempRGB.b); ///blue
    }    

    //Make the BMP image with the new values
    generateBitmapImage((unsigned char *)image, height, width, 0, "./Test Files/QuantizedImage.bmp");    

}

#endif