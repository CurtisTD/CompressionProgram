#ifndef LZ_ALGORITHMS_HPP
#define LZ_ALGORITHMS_HPP
#include <vector>
#include <string>
#include <iostream>

/*Type of code for compressing and decompressing*/
using CodeType = std::uint16_t; //Unsigned 16bit short

/*Overload of + operator for a vector and given char*/
std::vector<char> operator+ (std::vector<char> vecOfChars, char char_p) {
    vecOfChars.push_back(char_p);
    return vecOfChars;
}

/* LZ Globals */
namespace globals {

//Dictionary Maximum Size uint16_t - 16bit short
    const CodeType dms {std::numeric_limits<CodeType>::max()};
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
* Lempel-Ziv Decompress
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

#endif //LZ_ALGORITHMS_HPP