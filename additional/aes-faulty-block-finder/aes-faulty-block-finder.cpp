#include <iostream>
#include <fstream>
#include <string.h>
#include <iomanip>
using namespace std;


int main (int argc, char *argv[], char *envp[] ){

  if ( argc != 4 ){
	  cout<< "Usage:"<< argv[0]<< " <plaintext> <ciphertext> <faultyciphertext>\n";
  }

    
  ifstream pt_file, ct_file, fct_file ;
  pt_file.open(argv[1], std::ifstream::in|std::ifstream::binary);
  ct_file.open(argv[2], std::ifstream::in|std::ifstream::binary);
  fct_file.open(argv[3], std::ifstream::in|std::ifstream::binary);

  if ( !pt_file.is_open() || !ct_file.is_open() || !fct_file.is_open()){
	  cout<<"One of the files could not be opened\n";
	  return -1;
  }

  uint8_t pt_buffer[16], ct_buffer[16], fct_buffer[16], ct_backup[16];
  int block_num = 0;

  do{

	  pt_file.read((char*)&pt_buffer[0], 16);
	  ct_file.read((char*)&ct_buffer[0], 16);
	  fct_file.read((char*)&fct_buffer[0], 16);


	  if ( memcmp(ct_buffer, fct_buffer, 16) != 0){

		  cout << "The plaintext: \n";
		  for ( int i=0; i< 16; i++){
			  cout << setw(2) << setfill('0') << hex << (int) pt_buffer[i];
			  //cout << (int) pt_buffer[i];
		  }

		  cout << "\nThe input to the cipher (plaintext XOR previous cipher text): \n";
		  for ( int i=0; i< 16; i++){
			  cout << setw(2) << setfill('0') << hex << ((int) pt_buffer[i] ^ (int) ct_backup[i]);
			  //cout <<  ((int) pt_buffer[i] ^ (int) ct_backup[i]);
		  }

		  cout << "\nThe correct ciphertext: \n";
		  for ( int i=0; i< 16; i++){
			  cout << setw(2) << setfill('0') << hex << (int) ct_buffer[i];
			  //cout << (int) ct_buffer[i];
		  }


		  cout << "\nThe faulty ciphertext: \n";
		  for ( int i=0; i< 16; i++){
			  cout << setw(2) << setfill('0') << hex << (int) fct_buffer[i];
			  //cout << (int) fct_buffer[i];
		  }

		  cout << "\nThe error was reported at block: " << block_num << "\n";
		  break;
	  }

	  memcpy(ct_backup, ct_buffer, 16);
	  block_num++;
	  
  }while(pt_file.gcount() > 0);


  return 0;
}
