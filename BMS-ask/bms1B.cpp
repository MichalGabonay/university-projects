/*
 * File: bms1B.cpp
 * 1. project to BMS (Wireless and Mobile Networks)
 * ASK demodulation
 * Michal Gabonay (xgabon00@stud.fit.vutbr.cz)
 * Faculty of Information Technology 
 * University of Technology Brno  
 * date: 12.10.2018
 */

#include <cstdlib>
#include <math.h>
#include <iostream>
#include <algorithm>
#include <array>
#include <fstream>

#include "sndfile.hh"

#define AMPLITUDE (1.0 * 0x7F000000)

using namespace std;

// get nuber of samples for one baud (period)
// work as simple finite-state machine
// synchronization sequence is 00110011
// so input signal will start with zeroes followed by non zero values
// sequence 11 can start or end with 0 and inside where cut x axis is also 0
// must take this in mind
//
// int *buffer - sequence of samples for input signal
// int num_all_samples - nuber of samples in input signal
double getBaudLen(int *buffer, int num_all_samples)
{
	int samplet_per_baud = 0;
	int last_symbol = 0;
	int state = 0;

	if (buffer[0] != 0)
	{
		return EXIT_FAILURE;
	}

	for (int i = 0; i < num_all_samples; ++i)
	{
		if (buffer[i] == 0 && state == 0 && last_symbol == 0)
		{
			samplet_per_baud++;
		} 
		else if (buffer[i] != 0 && state == 0 && last_symbol == 0)
		{
			samplet_per_baud++;
			state = 1;
			last_symbol = 1;
		}
		else if (buffer[i] != 0 && state == 1 && last_symbol == 1)
		{
			samplet_per_baud++;
		}
		else if (buffer[i] == 0 && state == 1 && last_symbol == 1)
		{
			samplet_per_baud++;
			last_symbol = 0;
		}
		else if (buffer[i] != 0 && state == 1 && last_symbol == 0)
		{
			samplet_per_baud++;
			last_symbol = 1;
		}
		else if (buffer[i] == 0 && state == 1 && last_symbol == 0)
		{
			return ceil((samplet_per_baud - 2) / 2.0);
		}
	}
	return EXIT_FAILURE;
}

// MAIN
int main(int argc, char** argv) {

	string input_file;
	string output_file;
    SndfileHandle inputFile;
    int sampleRate;
    int baud_len;
    int num_of_baunds;
    int frames;
    int *buffer;
    int pos = 0;
    double local_amplitude;

    if (argc != 2)
    {
        fprintf (stderr, "errorr: first and only parameter has to be the input file!\n");
        return EXIT_FAILURE;
    } else {
        input_file = string(argv[1]);
    }

    output_file = input_file.substr(0, input_file.find_last_of(".")).append(".txt"); // create wav file name from input file name
    
    
    inputFile = SndfileHandle(input_file);
    
    sampleRate = inputFile.samplerate();
    buffer = new int[sampleRate];
    inputFile.read(buffer, sampleRate);
    frames = inputFile.frames();

    // get nuber of symbols for one baud (period)
    baud_len = getBaudLen(buffer, frames-1);

    if (baud_len == EXIT_FAILURE)
    {
    	fprintf(stderr, "errorr: wrong snchronization sequence!\n");
    	return EXIT_FAILURE;
    }

    num_of_baunds = frames / baud_len;
  
    string output = "";

    for (int i = 0; i < num_of_baunds; i++)
    {
        // get amplitude of period (baud)
    	local_amplitude = (*max_element(buffer + pos, buffer + pos + baud_len))/AMPLITUDE;
    	pos = pos + baud_len;

    	if (i > 3)
    	{
    		if (local_amplitude == 0) // zero amplitude
    		{
    			output.append("00");
    		} 
    		else if (local_amplitude > 0.3 && local_amplitude < 0.4) // 1/3 of amplitude
    		{
    			output.append("01");
    		}
    		else if (local_amplitude > 0.6 && local_amplitude < 0.7) // 2/3 of amplitude
    		{
    			output.append("10");
    		}
    		else if (local_amplitude == 1) // full amplitude
    		{
    			output.append("11");
    		}
    	}
    }

    // save output to txt file
    ofstream outfile (output_file);
    outfile << output << endl;
    outfile.close();
    
    delete [] buffer;
    return EXIT_SUCCESS;
}