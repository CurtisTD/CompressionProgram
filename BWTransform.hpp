//BWTransform - This file implements the BW-Transformation algorithm 
//as well as the inverse of BWT data

#ifndef BW_TRANSFORM_HPP
#define BW_TRANSFORM_HPP
#define START 0x02
#define END 0x03

#include <iostream>
#include <vector>
#include <algorithm>

//Prints the data passed
void printData(const std::string &stringParam) {
    std::cout << stringParam;
}

//Does the rotations for the BW Transformation
void rotate(std::string &toRotate) {
    char t = toRotate[toRotate.length() - 1];
    for (int i = toRotate.length() - 1; i > 0; i--) {
        toRotate[i] = toRotate[i - 1];
    }
    toRotate[0] = t;
}

//Forward BWT- BW Transformation to data
void forwardBWT(std::istream &is, std::ostream &os) {
    //Gets input data string
    char ch;
    std::string data;
    while(is.get(ch)) {
        data.push_back(ch);
    }
    //std::cout << std::endl << "Input text: " << data << std::endl;

    for (char ch : data) {
        if (ch == START || ch == END) {
            std::cout << "Input data cannot contain '^' or '|' chars." << std::endl;
        }
    }

    //Create string to modify
    std::string temp;
    temp.push_back(START);
    temp.append(data);
    temp.push_back(END);

    //Create a table for rotations
    std::vector<std::string> table;
    for (size_t i = 0; i < temp.length(); i++) {
        table.push_back(temp);
        rotate(temp); //Rotation
    }
    //Sort the data
    std::sort(table.begin(), table.end());
 
    std::string out;
    for (auto &temp : table) {
        os <<  temp[temp.length() - 1];
    }
    
    /*
    std::cout << "BWTransformed Data: ";
    printData(out);
    std::cout << std::endl << std::endl;
    */

    //os << out;
}

/****************Inverse BWT Functions*********************/

//Inverse BWT function, takes BWTransformed data and decodes it
std::string invertFunc(const std::string &bwtData) {

    //std::cout << std::endl << "BWT'd text: " << bwtData;
    int dataLength = bwtData.length();

    //'Decodes' the BWT data by traversing the table and sorting
    std::vector<std::string> table(dataLength);
    for (int i = 0; i < dataLength; i++) {
        for (int j = 0; j < dataLength; j++) {
            table[j] = bwtData[j] + table[j];
        }
        std::sort(table.begin(), table.end());
    }
    for (auto &row : table) {
        //Takes the 'tagging' chars away from the string
        if (row[row.length() - 1] == END) {
            return row.substr(1, row.length() - 2);
        }
    }
    return NULL;
}

void inverseBWT(std::istream &is, std::ostream &os) {
    //Gets input data string
    char ch;
    std::string r;
    while(is.get(ch)) {
        r.push_back(ch);
    }
    
    //Gets the inverted BWT data
    std::string inversedBWTString = invertFunc(r);
    //std::cout << std::endl << "Inverted BWT text: " << inversedBWTString << std::endl << std::endl;
    os << inversedBWTString;
}

#endif //BW_TRANSFORM_HPP