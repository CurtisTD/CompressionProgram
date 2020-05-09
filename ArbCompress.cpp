/*
ArbCompress:
This code is written using the C++11 standard as should be compiled with
"-std=c++11" minimum.
This also runs using windows libraries

This program allows for arbitray data to be passed in to it. It then tests
several implementations of different algorithms, including Lempel-Ziv, RLE, 
to compress and decompress these files.
----------------------------------------------------------
These are references I used to implement algortihms in C++.
References: 
1. Lempel-Ziv coding http://www.cplusplus.com/articles/iL18T05o/
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <bitset>
#include <limits>
#include <vector>
#include "RLE_Algorithms.hpp"
#include "LZ_Algorithms.hpp"
#include "ImageQuantize.hpp"
#include "Huff_Algo.hpp"
//Transformations
#include "BWTransform.hpp"
//Encryptions
#include "RSA_Encryption.hpp"
#include "Cypher_Encryption.hpp"

/*Global variables*/
namespace globals {

//Allowed file types for certain compression and decompression
    std::set<std::string> allowedFileTypes {"png", "bmp", "txt"};
}

/*Helper function; c++11 constant expression to aid switch string statements*/
constexpr unsigned int switchHash(const char *s, int off = 0) {                        
    return !s[off] ? 5381 : (switchHash(s, off+1)*33) ^ s[off];                           
}

/*
* Prints instructions for user, in case of errors
*/
void printCompressionInstructions() {
    std::cout << std::endl << "*******Instructions*******" << std::endl <<
        "To compress and decompress the file, type either of the following, respectively: " << std::endl <<
        "    LZCompress.exe -c AlgX inputFileName" << std::endl <<
        "    LZCompress.exe -d AlgX compressedFileName" << std::endl <<
        "    'AlgX' is the algorithm to be used, currently 'LZ' or 'RLE'" << std::endl <<
        "This program currently allows for .png and .bmp input files." << std::endl << std::endl;
}

/*
* Prints string input to a file
*/
void printStringToFile(std::string stringToPrint, std::ostream &os) {
    os << stringToPrint;
}

/* 
*  Main function of the program.
*  Char* arg value takes in an arbitrary input.
*/
int main (int argc, char* argv[]) {
    //argv[0]: executable, argv[1]: -c/-d option, argv[2]: algorithm choice, argv[3]: file input

    if (argc != 4) {
        printCompressionInstructions();
        return EXIT_FAILURE;
    }

    /*Gets the input file*/
    std::ifstream inputFile(argv[3], std::ios_base::binary);
    if(!inputFile.is_open()){
        std::cout << "Could not open the specified file.";
        printCompressionInstructions();
        return EXIT_FAILURE;
    }

    /**Creates new parameters for an output file with an encoded extension on file name**/
    std::string newFileName = argv[3]; //Old file name  
    std::string savedExtension = newFileName.substr(newFileName.find_last_of(".") + 1); 
    //Create new file name with extension
    size_t lastIndex = newFileName.find_last_of(".");
    std::string exactFileName = newFileName.substr(0, lastIndex);
    //Sets a char pointer variable for algorithm choice
    char* algorithmChoice = argv[2];

    /*Determines the user's intention for the input file*/
    if (globals::allowedFileTypes.find(savedExtension) != globals::allowedFileTypes.end()) { //If file type is allowed      
        
        /* Compression Algorithm Choices */  
        if(std::string("-c") == argv[1]){ 
            //Switch statement to chosen algorithm
            switch ( switchHash(algorithmChoice) ){
                /* RLE */
                case switchHash("RLE"): {                           
                    /* BMP File Type */
                    if(savedExtension == "bmp") { 
                        std::cout << "For a BMP file, this will result in a lossy compression; continue? (y/n) ";
                        char choice = 'y';
                        std::cin >> choice;
                        if(choice == 'n') break; 
                        //Quanitzes a bmp image before running RLE
                        quantizeBMP(argv[3]); //Data quanitzed, then passed to RLE
                        std::string inpp = "./Test Files/QuantizedImage.bmp";
                        std::string outpp = (exactFileName + "_RLEcompr." + savedExtension);
                        bmpEncode(inpp, outpp);
                        break;
                    } else if(savedExtension == "txt") {
                        //Ask user to check file size before doing BWT, to cut down time/memory constraints
                        char bwtAnswer;                        
                        while(bwtAnswer != 'y' && bwtAnswer != 'n'){
                            std::cout << "\nDo you want to use BWT on this file?" <<
                            " (Only recommended for smaller < 5Kb text files) [y/n]: ";
                            std::cin >> bwtAnswer;
                        }

                        if(bwtAnswer == 'y') {
                            /* BWTransform RLE */
                            std::cout << "This will now use BW-Transformation before RLE." << std::endl;

                            //Creates string to hold BWT data
                            std::string BWTString = forwardBWT(inputFile);
                            
                            //Creates a string stream for RLE encoding of BWT data
                            std::stringstream BWTStringStream;
                            BWTStringStream << BWTString;                            
                            
                            //Creates final output file
                            std::ofstream outputFileBWTRLE(exactFileName + "_BWT_RLEcompr." + savedExtension, std::ios_base::binary);

                            //Output BWT->RLE file
                            runLengthEncode(BWTStringStream, outputFileBWTRLE);
                            
                            outputFileBWTRLE.close();                   
                        }

                        //DEBUG: Outputs RLE only on text file for testing - set to 1 to create RLE only file
                        bool flag = 0;
                        if (flag || bwtAnswer == 'n') {
                            std::ifstream inputFileRLE(argv[3], std::ios_base::binary);
                            std::ofstream outputFileRLEOnly(exactFileName + "_RLEOnly." + savedExtension, std::ios_base::binary);
                            runLengthEncode(inputFileRLE, outputFileRLEOnly);
                            inputFileRLE.close();
                            outputFileRLEOnly.close();
                        }

                        break;
                    } else { 
                        std::ofstream outputFile(exactFileName + "_RLEcompr." + savedExtension, std::ios_base::binary); 
                        //Let user know about RLE drawbacks
                        std::cout << "RLE may result in a larger file size for this type." << std::endl;

                        runLengthEncode(inputFile, outputFile);
                        break;
                    }
                }
                /* Lempel-Ziv */
                case switchHash("LZ"): {
                    //Open new file for LZCompressed output
                    std::ofstream outputFile(exactFileName + "_LZcompressed." + savedExtension, std::ios_base::binary);        
                    lzCompress(inputFile, outputFile); 
                    break;
                }
                default: {
                    printCompressionInstructions();
                    return EXIT_FAILURE;
                }
            }            
        } 
        /* Decompression Algorithms */
        else if(std::string("-d") == argv[1] ) {
            switch ( switchHash(algorithmChoice) ){
                /* RLE */
                case switchHash("RLE"): {
                    if(savedExtension == "bmp") { 
                        //RLE BMP compression
                        std::string inpp = argv[3];
                        std::string outpp = (inpp + "_RLEdecompressed." + savedExtension);
                        bmpDecode(inpp, outpp);
                        break;
                   } else if(savedExtension == "txt"){

                        //Undo RLE first
                        std::string decodedRLE = runLengthDecode(inputFile); //Undoes RLE

                        //Creates string stream in lieu of input file stream
                        std::stringstream inputBWTinvertedRLE;
                        inputBWTinvertedRLE << decodedRLE;

                        //Undo BWT next
                        std::ofstream outinvertedBWTinvertedRLE(exactFileName + "_RLEdecomp_BWTinvert." + savedExtension, std::ios_base::binary);
                        inverseBWT(inputBWTinvertedRLE, outinvertedBWTinvertedRLE);
                        outinvertedBWTinvertedRLE.close();

                        break;                    
                    } else {
                        std::ofstream outputFile(exactFileName + "_RLEdecompressed." + savedExtension, std::ios_base::binary);        
                        std::string decodedData = runLengthDecode(inputFile);

                        printStringToFile(decodedData, outputFile);
                        break;
                    }
                }
                /* Lempel-Ziv */
                case switchHash("LZ"):{
                    //Open new file for LZDecompressed output
                    std::ofstream outputFile(exactFileName + "_LZdecompressed." + savedExtension, std::ios_base::binary);        
                    lzDecompress(inputFile, outputFile); 
                    break;
                }
                default: {
                    printCompressionInstructions();
                    return EXIT_FAILURE;
                }
            }               
        } 
        /*Transformation Algorithms*/
        else if(std::string("-trans") == argv[1] ) {
            switch ( switchHash(algorithmChoice) ){
                /* BWT Transformation */
                case switchHash("BWT"): {
                    std::ofstream outputFile(exactFileName + "_BWTransformed." + savedExtension, std::ios_base::binary);        

                    std::string BWTString = forwardBWT(inputFile);

                    printStringToFile(BWTString, outputFile);
                    break;
                }
                /* Inverse BW-Transformation */
                case switchHash("inBWT"): {
                    std::ofstream outputFile(exactFileName + "_InverseBWTransform." + savedExtension, std::ios_base::binary);        
                    inverseBWT(inputFile, outputFile);
                    break;
                }
                default: {
                    printCompressionInstructions();
                    return EXIT_FAILURE;
                }
            }
        } else if(std::string("-encrypt") == argv[1]) {
            switch ( switchHash(algorithmChoice) ){
                /* RSA Encryption */
                case switchHash("RSA"): {
                    //Runs the RSA program       
                    RSA_Encryption(); 
                    break;
                }
                /* Viginere Cypher */
                case switchHash("Vig"): {
                    //Runs the Vig Cypher program
                    vigenereCypher();
                    break;
                }
                default: {
                    printCompressionInstructions();
                    return EXIT_FAILURE;
                }     
            }   
        } else {
            //Print an error
            std::cout <<
            "Cannot decompress this file type. Please choose a following file type:\n" <<
            ".png, .bmp" << std::endl;
            return EXIT_FAILURE;
        }
    } else {
        printCompressionInstructions();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}