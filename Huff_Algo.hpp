#ifndef HUFF_ALGO_HPP
#define HUFF_ALGO_HPP

/* 
HuffStringCompress:
This program takes in a given string and uses Huffman encoding to compress it.
Huffman encoding does this by finding the frequency of characters in the string and
assigns 'codes' to represent the characters while satisying the prefix rule, which
ensures we can only decode the encoded string to its orginal form.
----------------------------------------------------
Author: Curtis Davis
*/

#include <string>
#include <iostream>
#include <queue>
#include <unordered_map>

//Node of the Huffman Tree
struct HuffNode {
    char ch;
    int num;
    HuffNode *leftChild;
    HuffNode *rightChild;

    //Constructor for a new node
    HuffNode(char ch_p, int num_p, HuffNode* left = NULL, HuffNode* right = NULL){
        this->ch = ch_p;
        this->num = num_p;
        leftChild = left;
        rightChild = right;
    }
};

//Comparison gives 'priority' to least occurring character
struct compareNodeWeights {
    bool operator ()(HuffNode* leftNode, HuffNode* rightNode){
        return leftNode->num > rightNode->num;
    }
};

//Stores the 'Huffman Code' into a map
void encodeHuff(HuffNode* rootNode, std::string string_p, 
        std::unordered_map<char, std::string> &huffmanCode){
    
    if(rootNode == nullptr){
        return;
    }

    //Stores the code when it finds a leaf
    else if(!rootNode->leftChild && !rootNode->rightChild){       
        huffmanCode[rootNode->ch] = string_p;     
    }

    encodeHuff(rootNode->leftChild, string_p + "0", huffmanCode);
    encodeHuff(rootNode->rightChild, string_p + "1", huffmanCode);
}

void decodeHuff(HuffNode* rootNode, int &index, std::string string_p){
    if (rootNode == nullptr){
        return;
    }

    //Prints a found leaf node
    else if(rootNode->leftChild == NULL && rootNode->rightChild == NULL){
        std::cout << rootNode->ch;
        return;
    }

    index++;
    if(string_p[index] != '0'){
        decodeHuff(rootNode->rightChild, index, string_p);
    } else {
        decodeHuff(rootNode->leftChild, index, string_p);
    }
}

void buildHuffTree(std::string textInput){
    //Gets frequency of characters of the input string
    std::unordered_map<char, int> numOfChars;
    for(char ch : textInput){
        numOfChars[ch]++;
    }

    // Creates a priority queue for the nodes of the Huffman tree, lower freq = higher priority
    std::priority_queue<HuffNode*, std::vector<HuffNode*>, compareNodeWeights> minHeap;
    for(auto pair : numOfChars){
        minHeap.push(new HuffNode(pair.first, pair.second));
    }

    // Checks for a single character input
    if(minHeap.size() == 1){
        HuffNode *onlyNode = minHeap.top();
        minHeap.pop();
        minHeap.push(new HuffNode('\0', onlyNode->num, onlyNode));
    }

    while(minHeap.size() != 1){       
        HuffNode *left = minHeap.top();
        minHeap.pop();
        HuffNode *right = minHeap.top();
        minHeap.pop();

        // Pushes a new null node which is the sum of the two child  
        // nodes frequencies to the minHeap priority-queue
        int sumOfNodes = left->num + right->num;
        minHeap.push(new HuffNode('\0', sumOfNodes, left, right));
    }

    // Tree traversal to get huffman code in the map
    HuffNode* root = minHeap.top();   
    std::unordered_map<char, std::string> huffmanCode;
    encodeHuff(root, "", huffmanCode);

    std::cout << "The Huffman Codes are: " << std::endl;
    for(auto pair : huffmanCode){
        std::cout << pair.first << ": " << pair.second << std::endl;
    }

    std::cout << "Original string: " << textInput << std::endl;

    std::string encodedString = "";
    for(char ch : textInput){
        encodedString += huffmanCode[ch];
    }
    std::cout << "Compressed code/string is: " << encodedString << std::endl;

    int index = -1;
    std::cout << "Decompressed string is: ";
    while(index < (int)encodedString.size() - 1){
        decodeHuff(root, index, encodedString);
    }
}

#endif //HUFF_ALGO_HPP