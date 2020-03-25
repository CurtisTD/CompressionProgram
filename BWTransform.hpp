//BWT - 'Burrows-Wheeler Transform' of data
//To be combined with compression --> RLE or such

#ifndef BWT_TRANSFORM_HPP
#define BWT_TRANSFORM_HPP

#include <iostream> 
#include <stdlib.h> 
#include <string.h> 

//Struct stores rotation data
struct dataRotation { 
    int index; 
    char* suffix; 
}; 

//Compares two rotation objects and to sort alphabetically
int rotationCompare(const void* rot_1, const void* rot_2) {
    struct dataRotation* rot_1_A = (struct dataRotation*) rot_1;
    struct dataRotation* rot_2_B = (struct dataRotation*) rot_2;
    int ret = strcmp(rot_1_A->suffix, rot_2_B->suffix);
    return ret;
}

int* transformSuffArray(char* inputText, int lengthOfText) {
    //Holds an array of rotations and their index/place
    struct dataRotation rotations[lengthOfText];
    int i; //Definition of variable for loops

    //Stores old indices
    for(i = 0; i < lengthOfText; i++) {
        rotations[i].index = i;
        rotations[i].suffix = (inputText + i);
    }

    //Quick sort algorithm to sort rotations using our comparator function 'rotationCompare'
    qsort(rotations, lengthOfText, sizeof(struct dataRotation), rotationCompare);

    int* indexArray = (int*) malloc(lengthOfText * sizeof(int));
    for(i = 0; i < lengthOfText; i++) {
        indexArray[i] = rotations[i].index;
    }

    return indexArray;
}

//Takes the rotation array and returns the BWT of the text
char* findLastChar(char* inputText, int* suffArray, int sizeParam) {
    char* BWT_Array = (char*) malloc(sizeParam * sizeof(char));
    int i;

    for(i = 0; i < sizeParam; i++) {
        int j = suffArray[i] - 1; //Gets last char index
        if(j < 0) {
            j = j + sizeParam;
        }    
        BWT_Array[i] = inputText[j];
    }
    BWT_Array[i] = '\0'; //Null terminator
    return BWT_Array;
}

//Main function - takes in input file and produces output file
void BWTransform(std::istream &is, std::ostream &os){
    //Gets input data string
    char ch;
    std::string inputStringParam;
    while(is.get(ch)) {
        inputStringParam.push_back(ch);
    }

    //Definition of variables
    int lengthOfInput = inputStringParam.length();
    char inputText[lengthOfInput];
    strcpy(inputText, inputStringParam.c_str());

    //Gets the suffix array of the input text
    int* suffArray = transformSuffArray(inputText, lengthOfInput);

    //Get last char of each rotation and add them to the output
    char* BWT_Array = findLastChar(inputText, suffArray, lengthOfInput);

    std::cout << "Input text: " << inputText << std::endl;
    std::cout << "BWT of input text: " << BWT_Array << std::endl;
}

#endif //BWT_TRANSFORM_HPP