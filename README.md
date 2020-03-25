# CompressionProgram
Independent Study - USF Spring 2020

This program implements a few compression algorithms in C++. These algorithms can take in arbitrary data and attempt to compress the file size. The user can choose an algorithm and file to compress/decompress through the program.



Currently there are the following algoriths implemented:

Lempel-Ziv: Currently compresses arbitrary data.

Huffman Code: Currently compresses text files.

RLE: Currently compresses arbitrary data, though RLE will not often shrink a file size without other modifications prior.

Quanitzation: BMP images can now be quantized.

BWT: Burrows–Wheeler Transformation of data; to be implemented with RLE.

### To implement: 
1. RLE and LZ on BWT data.
2. Multiple character Huffman encoding (maybe to be combined with BWT).
