/*
ArbCompress:
This code is written using the C++11 standard as should be compiled with
"-std=c++11" minimum.

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
#include <string>
#include <map>
#include <set>
#include <bitset>
#include <limits>
#include <vector>

/*Type of code for compressing and decompressing*/
using CodeType = std::uint16_t; //Unsigned 16bit short

/*Overload of + operator for a vector and given char*/
std::vector<char> operator+ (std::vector<char> vecOfChars, char char_p) {
    vecOfChars.push_back(char_p);
    return vecOfChars;
}

/*Global variables*/
namespace globals {
//Allowed file types for compression and decompression
    std::set<std::string> allowedFileTypes {"png", "bmp", "txt"};

//Dictionary Maximum Size uint16_t - 16bit short
    const CodeType dms {std::numeric_limits<CodeType>::max()};
}

/*Helper function; c++11 constant expression to aid switch string statements*/
constexpr unsigned int switchHash(const char *s, int off = 0) {                        
    return !s[off] ? 5381 : (switchHash(s, off+1)*33) ^ s[off];                           
}

/*
* Lempel-Ziv Compress
* This function uses the Lempel-Ziv algorithm to compress an image
*/
void lzCompress(std::istream &is, std::ostream &os) {
    //Compression dictonary
    std::map<std::vector<char>, CodeType> compDictionary;
    
    //Resets the dictionary
    const auto resetDictionary = [&compDictionary] {
        compDictionary.clear();

        const long int minc = std::numeric_limits<char>::min();
        const long int maxc = std::numeric_limits<char>::max();

        for (long int c = minc; c <= maxc; ++c) {
            const CodeType dictionarySize = compDictionary.size();

            compDictionary[{static_cast<char> (c)}] = dictionarySize;
        }
    };

    resetDictionary();

    //While loop to get characters from input stream
    std::vector<char> str;
    char ch;
    while (is.get(ch)) {
        //If the dictionary size becomes too large
        if (compDictionary.size() == globals::dms) {
            resetDictionary();
        }
        str.push_back(ch);
        if (compDictionary.count(str) == 0) {
            const CodeType dictionarySize = compDictionary.size();

            compDictionary[str] = dictionarySize;
            str.pop_back();
            os.write(reinterpret_cast<const char*> (&compDictionary.at(str)), sizeof (CodeType));
            str = {ch};
        }
    }

    if (!str.empty()) {
        os.write(reinterpret_cast<const char *> (&compDictionary.at(str)), sizeof (CodeType));
    }
}

/*
* Lempel-Ziv Deompress
* This function uses the Lempel-Ziv algorithm to decompress an image
*/
void lzDecompress(std::istream &is, std::ostream &os) {
    std::vector<std::vector<char>> dictionary;

    //Lamda to reset dictionary and set limits
    const auto reset_dictionary = [&dictionary] {
        dictionary.clear();
        dictionary.reserve(globals::dms); //Reserve space for dictionary's max size

        const long int minc = std::numeric_limits<char>::min(); //Min chars
        const long int maxc = std::numeric_limits<char>::max(); //Max chars

        for (long int c = minc; c <= maxc; ++c) {
            dictionary.push_back({static_cast<char> (c)}); //Put 'chars' to setup dictionary
        }
    };

    reset_dictionary();
    std::vector<char> str; // String
    CodeType key;

    //Read the LZ encoded input stream, with 'keys'
    while ( is.read(reinterpret_cast<char *> (&key), sizeof (CodeType)) ) {
        //Dictionary reaches maximum size, reset
        if (dictionary.size() == globals::dms){
            reset_dictionary();
        }
        if (key > dictionary.size()) { //If key is bigger than dictionary size, error
            throw std::runtime_error("invalid compressed code");
        }
        if (key == dictionary.size()) {
            dictionary.push_back(str + str.front());
        } else if (!str.empty()) {
            dictionary.push_back(str + dictionary.at(key).front());
        }
        //Write key and the size of the key
        os.write(&dictionary.at(key).front(), dictionary.at(key).size());
        //String is current dictionary
        str = dictionary.at(key);
    }

    //Error checking
    if (!is.eof() || is.gcount() != 0) {
        throw std::runtime_error("corrupted compressed file");
    }
}

/*
* Run Length Encoding
* This function uses the RLE algorithm to encode/compress data
*/
void runLengthEncode(std::istream &is, std::ostream &os){    
    char ch; //Buffering character
    char prev_ch; //Holds previous characters
    int count = 1; //Counts number of chars in a run
    is.get(prev_ch); //Get first char of input stream
    while (is.get(ch)) {
        if(ch != prev_ch) { //Character has changed from a run
            os << count << prev_ch;
            count = 0; //Reset          
        }
        count++;
        prev_ch = ch; //Set prev_ch to char just read
    } os << count << prev_ch; //Reads last character to output
}

/*
* Run Length Decoding
* This function uses the RLE algorithm to decode/decompress data
*/
void runLengthDecode(std::istream &is, std::ostream &os){
    char ch; //Buffering character
    while (is.get(ch)) {
        int numToPrint = ch - '0'; //Gets number of char
        is.get(ch); //Gets character to print
        while(numToPrint--){
            os << ch;
        }
    }
}

/*
* Quantize
* This function quantizes data to be better compressed
*/
void quantize(){
    //TODO
}


/*
* Prints instructions for user, in case of errors
*/
void printInstructions() {
    std::cout << std::endl << "*******Instructions*******" << std::endl <<
        "To compress and decompress the file, type either of the following, respectively: " << std::endl <<
        "    LZCompress.exe -c AlgX inputFileName" << std::endl <<
        "    LZCompress.exe -d AlgX compressedFileName" << std::endl <<
        "    'AlgX' is the algorithm to be used, currently 'LZ' or 'RLE'" << std::endl <<
        "This program currently allows for .png and .bmp input files." << std::endl << std::endl;
}

/* 
*  Main function of the program.
*  Char* arg value takes in an arbitrary input.
*/
int main (int argc, char* argv[]) {
    //argv[0]: executable, argv[1]: -c/-d option, argv[2]: algorithm choice, argv[3]: file input

    if (argc != 4) {
        printInstructions();
        return EXIT_FAILURE;
    }

    /*Gets the input file*/
    std::ifstream inputFile(argv[3], std::ios_base::binary);
    if(!inputFile.is_open()){
        std::cout << "Could not open the specified file.";
        printInstructions();
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
        /*Compression Algorithm Choices*/  
        if(std::string("-c") == argv[1]){ 
            //Switch statement to chosen algorithm
            switch ( switchHash(algorithmChoice) ){
                case switchHash("RLE"): {
                    std::ofstream outputFile(exactFileName + "_RLEcompressed." + savedExtension, std::ios_base::binary);        
                    runLengthEncode(inputFile, outputFile);
                    break;
                }
                case switchHash("LZ"): {
                    //Open new file for LZCompressed output
                    std::ofstream outputFile(exactFileName + "_LZcompressed." + savedExtension, std::ios_base::binary);        
                    lzCompress(inputFile, outputFile); 
                    break;
                }
                default: {
                    printInstructions();
                    return EXIT_FAILURE;
                }
            }            
        } 
        /*Decompression Algorithms*/
        else if(std::string("-d") == argv[1] ) {
            switch ( switchHash(algorithmChoice) ){
                case switchHash("RLE"): {
                    std::ofstream outputFile(exactFileName + "_RLEdecompressed." + savedExtension, std::ios_base::binary);        
                    runLengthDecode(inputFile, outputFile);
                    break;
                }
                case switchHash("LZ"):{
                    //Open new file for LZDecompressed output
                    std::ofstream outputFile(exactFileName + "_LZdecompressed." + savedExtension, std::ios_base::binary);        
                    lzDecompress(inputFile, outputFile); 
                    break;
                }
                default: {
                    printInstructions();
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
        printInstructions();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}