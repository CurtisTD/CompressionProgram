#ifndef VIGENERE_CYPHER_HPP
#define VIGENERE_CYPHER_HPP

#include <iostream>
#include <string.h>

void vigenereCypher() {

    std::cout << std::endl << "Welcome to the Vigenere Cypher Porgram." << std::endl
        << "Please note that the message you enter for the cypher will be converted to uppercase." << std::endl
        << "Also note that the message you enter should not contain spaces, only letters." << std::endl
        << std::endl;

    //Get a message to encrypt from user
    char message[100];
    std::cout << "Enter a message to Vigenere Cipher (Limit 100 chars): ";
    std::string msgString;
    getline(std::cin, msgString);
    strcpy(message, msgString.c_str()); //Copies string to char array

    //Get a Key from user for cypher
    char key[100];
    std::cout << "Enter a key for the Vigenere Cipher  (Limit 100 chars): ";
    std::string keyString;
    getline(std::cin, keyString);
    strcpy(key, keyString.c_str()); //Copies string to char array

    int msgLength = strlen(message);
    int keyLen = strlen(key);
    char newKey[msgLength]; //New key to be generated for cypher
    char encryptedMsg[msgLength], decryptedMsg[msgLength];

    int i, j;
    //Converts strings to uppercase for cypher
    for(int i = 0; i < msgLength; i++) {
        message[i] = toupper(message[i]);
    }
    for(i = 0; i < keyLen; i++) {
        key[i] = toupper(key[i]);
    }

    //Encryption of message
    for(i = 0; i < msgLength; ++i) {
        encryptedMsg[i] = ( (message[i] + newKey[i]) % 26 ) + 'A';
    }
    //Generates a new key
    for(i = 0, j = 0; i < msgLength; ++i, ++j){
        if(j == keyLen) { j = 0; }
        newKey[i] = key[j];
    }

    newKey[i] = '\0'; //Place null terminating at end of new key
    encryptedMsg[i] = '\0'; //Place null terminating at end of encrypted message
 
    //Decryption of message
    for(i = 0; i < msgLength; ++i) {
        decryptedMsg[i] = (((encryptedMsg[i] - newKey[i]) + 26) % 26) + 'A';
    }
    decryptedMsg[i] = '\0'; //Place null terminating at end
 
    std::cout << "User Input: " << message << std::endl ;
    std::cout << "User Key: " << key << std::endl ;
    std::cout << "The Newly Generated Key: " << newKey << std::endl ;
    std::cout << "Encrypted Message: " << encryptedMsg << std::endl ;
    std::cout << "Decrypted Message: " << decryptedMsg << std::endl ;
}

#endif //VIGENERE_CIPHER_HPP