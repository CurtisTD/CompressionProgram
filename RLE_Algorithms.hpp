#ifndef RLE_ALGOS_HPP
#define RLE_ALGOS_HPP

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

#endif //RLE_ALGOS_HPP