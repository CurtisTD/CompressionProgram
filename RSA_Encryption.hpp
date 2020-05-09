#ifndef RSA_ENCRYPTION_HPP
#define RSA_ENCRYPTION_HPP

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <string.h>

//Globals for RSA encryption
int n, t, x, y;
int flag; //Used for checking conditions
int encrypted[50], decrypted[50], temp[50]; //Holds keys of encryption and decryption

//Globals to hold message, encrypted, and decrypted texts
char msg[100];
char en[50], decryptedMessage[50]; 

//Checks if a number is prime
int prime(int pr) {
   int i;
   for(i = 2; i <= pr / 2; i++) {
      if(pr % i == 0) {
         return 0;
      }
   }
   return 1; //Return true if never non-prime
}

//Used to aid in decrypting the encrypted message
int cd(int cd_param) {
   int num = 1;
   while(true) {
      num = num + t;
      if(num % cd_param == 0) {
         return(num / cd_param);
      }
   }
}

//Generates an encryption key
void encryption_key() {
   int k = 0;
   for(int i = 2; i < t; i++) {
      if(t % i == 0) {
         continue;
      }

      flag = prime(i); //Check if i is prime or not
      if(flag == 1 && i != x && i != y) {
         encrypted[k] = i;
         flag = cd( encrypted[k] );
      }
        
      if(flag > 0) {
         decrypted[k] = flag;
         k++;
      }
        
      if(k == 99) {
         break;
      }
   }
}

//RSA Encryption of message
void rsa_encrypt() {
   //Cypher text = message ^ n modulus n

   int pt, ct;
   int key = encrypted[0];
   int k, len = strlen(msg), i = 0;

   //Encrypts the original message
   while(i != len) {
      pt = decryptedMessage[i];
      pt = pt - 96;
      k = 1;

      for(int j = 0; j < key; j++) {
         k = k * pt;
         k = k % n;
      }

      temp[i] = k;
      ct= k + 96;
      en[i] = ct;
      i++;
   }

   en[i] = -1;
   std::cout << std::endl << std::endl << "The encrypted message is: ";
   for(i = 0; en[i] != -1; i++) {
      std::cout << en[i];
   }
}

//RSA decryption of a message
void rsa_decrypt() {
   //Decrypted text = encryptedMessage ^ d modulus n
   
   int key = decrypted[0], ky;
   int pt, ct;   

   //Decrypts the encrypted message
   int i = 0;
   while(en[i] != -1) {
      ct = temp[i];
      ky = 1;
      
      for(int j = 0; j < key; j++) {
         ky = ky * ct;
         ky = ky % n;
      }

      pt = ky + 96;
      decryptedMessage[i] = pt;
      i++;
   }

   decryptedMessage[i] = -1;
   //Reads out decrypted message
   std::cout << std::endl << std::endl << "The decrypted message: ";
   for(i = 0; decryptedMessage[i] != -1; i++) {
      std::cout << decryptedMessage[i];
   }

  std::cout << std::endl << std::endl;
}

int RSA_Encryption() {
   std::cout << std::endl << "Enter a prime number: ";
   std::cin >> x;

   //Check if prime number was entered
   if(prime(x) == 0) {
      std::cout << std::endl << "That is not a prime number" << std::endl;
      return EXIT_FAILURE;
   }

   std::cout << std::endl << "Enter the second prime number: ";
   std::cin >> y;

   //Check if second number is prime or if it matches the first number
   if(prime(y) == 0 || x == y) {
      std::cout << std::endl << "That is not a prime number" << std::endl;;
      return EXIT_FAILURE;
   }

   std::cout << std::endl << "Enter a message for the program to encrypt (100 char limit): ";
   
   //Flush buffer
   std::cin.ignore (std::numeric_limits<std::streamsize>::max(), '\n'); 

   std::string msgString;
   getline(std::cin, msgString);
   strcpy(msg, msgString.c_str());

   for(int i = 0; msg[i] != '\0'; i++) {
      decryptedMessage[i] = msg[i];
   }

   //Creating the keys for RSA
   n = x * y;
   t = (x - 1) * (y - 1); //Totient function

   //Generate an encryption key
   encryption_key();

   //Runs the encryption algorithm on global message
   rsa_encrypt();

   //Runs the decryption algorithm on global encrypted message
   rsa_decrypt();

   return EXIT_SUCCESS;
} //end of the main program

#endif //RSA_ENCRYPTION_HPP