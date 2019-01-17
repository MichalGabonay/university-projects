/*
 * File: bms2B.cpp
 * 2. project to BMS (Wireless and Mobile Networks)
 * Error Correction Code - decoder
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

// MAIN
int main(int argc, char** argv) {

	string input_file;
	string output_file;

    if (argc != 2)
    {
        fprintf (stderr, "errorr: first and only parameter has to be the input file!\n");
        return EXIT_FAILURE;
    } else {
        input_file = string(argv[1]);
    }

    output_file = input_file.append(".ok"); // create output file name from input file name

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

    size_t total_chunks = total_size / (CHUNK_LEN+ECC_LEN);
    size_t total_entire_chunks = total_size / (CHUNK_LEN+ECC_LEN);
    size_t last_chunk_size = total_size % (CHUNK_LEN+ECC_LEN);

    if (last_chunk_size != 0) { // last chunk not is entire
        ++total_chunks;
    }
    else {
        last_chunk_size = (CHUNK_LEN+ECC_LEN);
    }

    // read whole content of input file into vector
    std::vector<char> fileContents;
    fileContents.reserve(total_size);
    fileContents.assign(std::istreambuf_iterator<char>(f_in),
                    std::istreambuf_iterator<char>());

    for (size_t chunk = 0; chunk < total_chunks; ++chunk){
        size_t this_chunk_size =
        chunk == total_chunks - 1 // if last chunk
        ? last_chunk_size // then fill chunk with remaining bytes
        : (CHUNK_LEN+ECC_LEN); // else fill entire chunk

        vector<char> chunk_data(this_chunk_size);
        int offset = 0;
        if (this_chunk_size == CHUNK_LEN+ECC_LEN)
        {
            // read interlaced data
            // example: indata - Aa1Bb2Cc3Dd4
            //          read chunk - ABCD (next chunks - abcd, 1234)
            for (size_t i = 0; i < this_chunk_size; ++i)
            {
                chunk_data[i] = fileContents[chunk + offset];
                offset += total_entire_chunks;
            }
        } else {
            // if not entire chunk, just take what left
            for (size_t i = 0; i < this_chunk_size; ++i)
            {
                chunk_data[i] = fileContents[(total_entire_chunks)*(CHUNK_LEN+ECC_LEN) + i];
            }
        }

        ezpwd::RS<CHUNK_LEN + ECC_LEN,CHUNK_LEN> rs; // initializing of Reed-Solomov

        rs.decode( chunk_data ); // decode data with RS

        chunk_data.resize( chunk_data.size() - rs.nroots() ); // discard parity simpols

        for (const auto c : chunk_data)
        {
          f_out << c; // write repaired data into output file
        }
    }

    f_in.close();
    f_out.close();

    return EXIT_SUCCESS;
}
