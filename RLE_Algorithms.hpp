#ifndef RLE_ALGOS_HPP
#define RLE_ALGOS_HPP

using std::cout;
using std::endl;
using std::ios;

/* Add an escape to tell program that a number isn't part of the count */
char escCharEndNum = 0x02;

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
            
            os << count << escCharEndNum << prev_ch;
            count = 0; //Reset          
        }
        count++;
        prev_ch = ch; //Set prev_ch to char just read
    } os << count << escCharEndNum << prev_ch; //Reads last character to output
}

/*
* Run Length Decoding
* This function uses the RLE algorithm to decode/decompress data
*/
std::string runLengthDecode(std::istream &is){

    std::string returnString;

    //Holds number to print
    std::string numToPrint;

    char ch; //Buffering character
    while (is.get(ch)) {
        numToPrint.push_back(ch);

        //Gets number of chars to print
        if(ch == escCharEndNum){
            numToPrint.pop_back(); //Removes last char

            //Then prints character after escape char
            try {
                int iterationsToPrint = std::stoi(numToPrint); //Converts to int to print
                is.get(ch); //Gets char to print
                while(iterationsToPrint--){
                    returnString.push_back( ch );
                }                
            }
            catch(std::invalid_argument const &exceptArg) {
                std::cout << "Invalid argument when converting string to integer, RLE Decode." << std::endl;
            }
            catch(std::out_of_range const &exceptRange) {
                std::cout << "Out of range exception when converting string to integer, RLE Decode." << std::endl;
            }
            
            numToPrint = ""; //Clears num and starts again
        }
    }
    return returnString;
}
/* End of Arbitrary RLE Functions */

/**********************************/
/*********BMP Fucntions************/
/**********************************/

typedef struct SPixel {
    uint8_t red;    
    uint8_t green;
    uint8_t blue;
} PIXEL;

bool compare(SPixel pixel1, SPixel pixel2) {
    if (pixel1.red == pixel2.red && pixel1.green == pixel2.green && pixel1.blue == pixel2.blue)
        return true;
    else return false;
}

void bmpEncode(std::string &input, std::string &output){
    unsigned short repetition = 1;
    PIXEL current;
    PIXEL next;

std::cout << input << endl; 

    std::fstream file;
    std::fstream compressed;
    file.open(input, ios::in | ios::binary);
    if (!file.is_open()) {
        cout << "cannot open file to encode." << endl;
        getchar();
        exit(1);
    }

    compressed.open(output, ios::out | ios::trunc | ios::binary);
    if (!compressed.is_open()) {
        cout << "cannot open file to save encoded file." << endl;
        getchar();
        exit(1);
    }

    file.read((char*)&current, sizeof(PIXEL));
    file.read((char*)&next, sizeof(PIXEL));

    while (!file.eof()) {
        if (!compare(current, next)) {
            compressed.write((char*)&repetition, sizeof(repetition));
            compressed.write((char*)&current, sizeof(PIXEL));
            repetition = 0;
        }
        repetition++;
        current = next;
        file.read((char*)&next, sizeof(PIXEL));
    }

    file.close();
    compressed.close();
}

void bmpDecode(std::string &input, std::string &output) {
    std::fstream file;
    std::fstream ready;
    PIXEL current;
    unsigned short repetition = 0;

    file.open(input, ios::in | ios::binary);
    if (!file.is_open()) {
        cout << "cannot open file to decode." << endl;
        getchar();
        exit(1);
    }
    ready.open(output, ios::trunc | ios::out | ios::binary);
    if (!ready.is_open()) {
        cout << "cannot open file to save decoded file." << endl;
        getchar();
        exit(1);
    }

    while (!file.eof()){
        file.read((char*)&repetition, sizeof(repetition));
        file.read((char*)&current, sizeof(PIXEL));
        for (int j = 0; j < repetition; j++)
            ready.write((char*)&current, sizeof(PIXEL));
    }

    file.close();
    ready.close();
}

#endif //RLE_ALGOS_HPP