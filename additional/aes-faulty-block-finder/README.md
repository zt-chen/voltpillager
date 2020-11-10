# faulty-block-finder

That's a small c++ utility that reads the 3 binary files of the plaintext, ciphertext and faulty ciphertext and shows which is the first block that was faulted. It also shows the CBC input to the cipher which is the xored value of the plaintext with the ciphertext of the previous block. This only works if the fault is not in the block 0 (this will require a hardcoded IV);

### Usage
`$ make`  
`$ ./faulty-block-finder <plaintext> <ciphertext> <faultyciphertext>`
