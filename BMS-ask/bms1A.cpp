/*
 * File: bms1A.cpp
 * 1. project to BMS (Wireless and Mobile Networks)
 * ASK modulation
 * Michal Gabonay (xgabon00@stud.fit.vutbr.cz)
 * Faculty of Information Technology 
 * University of Technology Brno  
 * date: 12.10.2018
 */

#include <cstdlib>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "sndfile.hh"

using namespace std;

#define SAMPLE_RATE 32000
#define CHANELS 1
#define FORMAT (SF_FORMAT_WAV | SF_FORMAT_PCM_24)
#define AMPLITUDE (1.0 * 0x7F000000)
#define FREQ (1000.0 / SAMPLE_RATE)
#define SYNC_SEQ "00110011" // synchronization sequence
#define BAUD_LEN 32 // BAUD_LEN = SAMPLE_RATE / 1000Hz

// ASK modulation
// istream* data - input file stream or synchronization sequence
// int &clock - global counter, representing time of signal 
// SndfileHandle handle - handler for wav file
void modulate(istream* data, int &clock, SndfileHandle handle)
{
    char bit1, bit2;
    int *buffer = new int[BAUD_LEN];

    //reading input file by 2 symbols
    while (data->get(bit1) && data->get(bit2))
    {  
        if (bit1 == '0' && bit2 == '0') // 00
        {
	        for (int i = 0; i < BAUD_LEN; i++)
	        {
	            buffer [i] = 0; // zero amplitude
	            clock++;
	        }    
        }
        else if (bit1 == '0' && bit2 == '1') // 01
        {
	        for (int i = 0; i < BAUD_LEN; i++)
	        {
	            buffer [i] = (AMPLITUDE * sin(FREQ * 2 * i * M_PI)) / 3; // 1/3 of amplitude
	            clock++;
	        }    
        }
        else if (bit1 == '1' && bit2 == '0') // 10
        {
	        for (int i = 0; i < BAUD_LEN; i++)
	        {
	            buffer [i] = (AMPLITUDE * sin(FREQ * 2 * i * M_PI) * 2) / 3; // 2/3 of amplitude
	            clock++;
	        }    
        }
        else if (bit1 == '1' && bit2 == '1') // 11
        { 
	        for (int i = 0; i < BAUD_LEN; i++)
	        {
	            buffer [i] = AMPLITUDE * sin(FREQ * 2 * i * M_PI); // full amplitude
	            clock++;
	        }    
        }
        // if there is illegal symbol in input file or there is unpair nuber of symbols
        else {
            fprintf(stderr, "errorr: illegal symbol in input file, use only 0 and 1 in pairs!\n");
            exit(EXIT_FAILURE);
        }

        handle.write(buffer, BAUD_LEN);
    }

    delete [] buffer;
}

// MAIN
int main(int argc, char** argv) {

	string input_file;
	string output_file;
    SndfileHandle outputFile;
    int clock = 0;

    // programme argment handle
    if (argc != 2)
    {
        fprintf (stderr, "errorr: first and only parameter has to be the input file!\n");
        return EXIT_FAILURE;
    } else {
        input_file = string(argv[1]);
    }

    output_file = input_file.substr(0, input_file.find_last_of(".")).append(".wav"); // create wav file name from input file name

    ifstream in (input_file);

    if(!(in.is_open())) {
        fprintf (stderr, "errorr: input file can not be opened!\n");
        return EXIT_FAILURE;
    }

    outputFile = SndfileHandle(output_file, SFM_WRITE, FORMAT, CHANELS, SAMPLE_RATE);

    stringstream sequence (SYNC_SEQ);
    modulate(&sequence, clock, outputFile); // at beggining add synchronization sequence
    modulate(&in, clock, outputFile);

    in.close();

    return EXIT_SUCCESS;
}