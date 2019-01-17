/*
 * File: bms2A.cpp
 * 2. project to BMS (Wireless and Mobile Networks)
 * Error Correction Code - coder
 * Michal Gabonay (xgabon00@stud.fit.vutbr.cz)
 * Faculty of Information Technology
 * University of Technology Brno
 * date: 6.12.2018
 *
 *****************************************************************
 * Reed-Solomov implemetation itself was taken
 * from repositare https://github.com/pjkundert/ezpwd-reed-solomon.
 *****************************************************************
 *
 */

#include <fstream>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>

#include "ezpwd/rs"

// ration 4:3 - outputfile cant get over 175% of inputfile
#define CHUNK_LEN 146 // length of data to encode
#define ECC_LEN 109 // length of parity symbols

using namespace std;

// interlase data byte by byte
// example: indata - ABCD | abcd | 1234
//          interlace - Aa1Bb2Cc3Dd4
int interlace_data(vector<char> &chunk_encoded, uint8_t* interlaced_data, size_t chunk, size_t total_chunks){

    int offset = 0;
    for (const auto c : chunk_encoded)
    {
        if (c != '\0')
        {
            interlaced_data[offset + chunk] = c;
        }
        offset += total_chunks;
    }

    return 0;
}

// MAIN
int main(int argc, char** argv) {

    string input_file;
	string output_file;

    // programme argment handle
    if (argc != 2)
    {
        fprintf (stderr, "errorr: first and only parameter has to be the input file!\n");
        return EXIT_FAILURE;
    } else {
        input_file = string(argv[1]);
    }

    output_file = input_file.append(".out"); // create output file name from input file name

    // input file
    ifstream f_in (argv[1], ios::binary);
    if(!(f_in.is_open())) {
        fprintf (stderr, "errorr: input file can not be opened!\n");
        return EXIT_FAILURE;
    }

    // output file
    ofstream f_out (output_file, ios::binary);
    if(!(f_out.is_open())) {
        f_in.close();
        fprintf (stderr, "errorr: output file can not be opened!\n");
        return EXIT_FAILURE;
    }


    struct stat filestatus;
    stat(argv[1], &filestatus);
    size_t total_size = filestatus.st_size; // get file size

    size_t total_chunks = total_size / CHUNK_LEN;
    size_t total_entire_chunks = total_size / CHUNK_LEN;
    size_t last_chunk_size = total_size % CHUNK_LEN;
    size_t interlaced_data_size = (total_chunks) * (CHUNK_LEN + ECC_LEN);

    if (last_chunk_size != 0) { // last chunk not is entire
        ++total_chunks;
    }
    else {
        last_chunk_size = CHUNK_LEN;
    }

    uint8_t* interlaced_data = (uint8_t*) malloc(interlaced_data_size);
    memset(interlaced_data, '#', interlaced_data_size);

    for (size_t chunk = 0; chunk < total_chunks; ++chunk)
    {
      size_t this_chunk_size =
        chunk == total_chunks - 1 // if last chunk
        ? last_chunk_size // then fill chunk with remaining bytes
        : CHUNK_LEN; // else fill entire chunk

        vector<char> chunk_data(this_chunk_size);
        vector<char> chunk_repaired(this_chunk_size);
        f_in.read(&chunk_data[0], // address of buffer start
                this_chunk_size); // this many bytes is to be read


        ezpwd::RS<CHUNK_LEN + ECC_LEN,CHUNK_LEN> rs; // initializing of Reed-Solomov

        rs.encode( chunk_data ); // encode data with RS

        if (chunk == total_chunks - 1) // if last, not entire, chunk
        {
            if (this_chunk_size != CHUNK_LEN)
            {
                f_out << interlaced_data; // write to output file all interlaced data
                for (const auto c : chunk_data)
                {
                    f_out << c; // last, not entire, chunk just write to output, not interlaced
                }
            } else {
                interlace_data(chunk_data, interlaced_data, chunk, total_entire_chunks);
                f_out << interlaced_data;
            }
        } else {
            // if entire chunk, interlace its data
            interlace_data(chunk_data, interlaced_data, chunk, total_entire_chunks);
        }
    }

    free(interlaced_data);
    f_in.close();
    f_out.close();

    return EXIT_SUCCESS;
}
