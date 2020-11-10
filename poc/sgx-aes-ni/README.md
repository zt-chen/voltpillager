# 5.1 SGX AES NI 
Run the program with `sudo ./app <iterations> <step>`

This program run aes-ni in enclave while lowering the voltage outside the enclave. 

`iteration` specify how may iterations of AES to run

`step` specify how many voltage steps to test, each step is 5mV. The starting voltage is 640mV

Each voltage step will be test for 3 times.

For example:

`sudo ./app 2000 10` will run 2000 iterations of AES while lowering the voltage start from 640mV. Each voltage is tested for 3 times, if the fault it not found, it will continue test lower voltage with step of 5mV. The last voltage it test is 640mV - 5mV*9. 

On Z170(2GHz), we obtained the faulty AES result with the following configuration 

```
configure_glitch_with_delay(1, 0.835, 29, 0.630, -25, 0.835, 1300);
```

## Evaluation of faulty result
Please use `additional/dfa-aes-master` to evaluate the faulty result. It shows that the correct key is in the result.

```
ubuntu@vm-dev:~/dfa-aes-master/analysis$ ./dfa 4 1 faulty_result_Z170.txt
(0) Analysing ciphertext pair:

a398f202faece6e70614d0d85985ac57 687f48a4f2b1869eb2710aacf5a88484

Number of core(s): 4
----------------------------------------------------
Fault location: 1
Applying standard filter. Done.
Size of keyspace: 7549747200 = 2^32.813781
Applying improved filter. Done.
Size of keyspace: 432 = 2^8.754888

432 masterkeys written to keys-0.csv

ubuntu@vm-dev:~/dfa-aes-master/analysis$ cat keys-0.csv | grep 0102
000102030405060708090a0b0c0d0e0f
```
With two faulty result, we can cross match the dfa result by:
```
cat keys-1.csv|sort >  pair1_keys.csv
cat keys-2.csv|sort >  pair2_keys.csv
comm -1 -2 pair1_keys.csv pair2_keys.csv
```

The correct key is shown in the result

