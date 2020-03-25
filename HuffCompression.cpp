/* 
CompressionProg: 
Main file for all compression/encoding files and algorithms.
----------------------------------------
Author: Curtis Davis
*/

#include "Huff_Algo.hpp"
//#include "ImageCompress.cpp"

int main(){

    std::string stringTest = "";
     std::cout << "Huffman Test:" << std::endl;
    while(stringTest.empty()){
        std::cout << "Enter a string of text(whitespace included): ";
        getline(std::cin, stringTest);
    }
    //Sends user entered line into huffman compression
    buildHuffTree(stringTest);

/*LempelZiv is in ImageCompress.cpp*/
/*Need to merge functions*/
//    std::cout << std::endl << std::endl << "Lempel Ziv Text Test:" << std::endl;
//    std::string lzCompressed = lzCompress("Hello, World!");
//    std::cout << lzCompressed << std::endl;
//    std::cout << lzDecompress(lzCompressed);

    return EXIT_SUCCESS;
}