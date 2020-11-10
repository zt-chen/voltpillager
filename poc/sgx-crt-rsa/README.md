# 5.1 RSA CRT Fault
## Undervolting parameters
We tested this PoC with the following undervolting parameters, the configuration will depends on your CPU operating environment.

```
//Z170 2GHz
configure_glitch_with_delay(1,0.83, 35, 0.63, -30, 0.83, 100);
// Z370 3.4GHz
configure_glitch_with_delay(1, 1.05, 10, 0.81, -20, 1.05, 10);
// NUC 2GHz
 configure_glitch_with_delay(1, 0.94, 35, 0.75, -50, 1.05, 10);
```
## How to run the code
* Step 1: run `make` to compile the code
* Step 2: run the code with `sudo ./app 1000`
    * If there is a successful fault, it will show `Noooo!!!!1111elfoelf` with the faulty result
* Step 3: Put the faulty result to `pt_fault` variable in `Evaluation/eval.py` to do analysis
    * There are already some example result obtained with these parameters in the file
