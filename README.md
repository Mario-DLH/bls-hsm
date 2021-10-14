# bls-hsm

## Design :page_with_curl:
The project emulate the behaviour of a command API that internally uses [blst library](https://github.com/supranational/blst#blst). They have been implemented with Segger Embedded Studio for the execution in Nordic Semiconductor nRF9160 board.


## Installation
You can run `./setup.sh [-c "compiler path"] [-i] -b "board identifier"` to build the project.
`-c "compiler path"` option will define the path of the arm compiler. `-i` option is used to automatically check if the compiler is installed and install it otherwise, using it in the building process.
`setup.sh` will run sequentially `build_blst.sh`, `dependencies.sh`, `build.sh` and `flash.sh`. After running `build_blst.sh` and `dependencies.sh` once, only `build.sh` and `flash.sh` are needed.

Output example:
```
user@user:~/bls-hsm$ ./setup.sh -c /usr/bin/arm-none-eabi-gcc -b nrf5340dk_nrf5340_cpuapp_ns
Compiler selected: /usr/bin/arm-none-eabi-gcc
+ /usr/bin/arm-none-eabi-gcc -O -fno-builtin-memcpy -fPIC -Wall -Wextra -Werror -mcpu=cortex-m33 -fno-pie -c ./blst/src/server.c
+ /usr/bin/arm-none-eabi-gcc -O -fno-builtin-memcpy -fPIC -Wall -Wextra -Werror -mcpu=cortex-m33 -fno-pie -c ./blst/build/assembly.S
+ /usr/bin/arm-none-eabi-gcc -O -fno-builtin-memcpy -fPIC -Wall -Wextra -Werror -mcpu=cortex-m33 -fno-pie -nostdlib -r assembly.o server.o -o blst.o
+ /usr/bin/arm-none-eabi-objcopy --localize-symbols=/tmp/localize.blst.11736 blst.o
+ /usr/bin/arm-none-eabi-ar rc libblst.a blst.o
Blst library built
.
.
.
-- runners.nrfjprog: Board with serial number xxxxxxxxx flashed successfully.
user@user:~/bls-hsm$
```

It's also possible to use nRF Connect SDK as it is explained in this [guide](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/gs_assistant.html).


## Usage
The commands that are supported are:
- *keygen*: generates a 32-bytes secret key and a 48-bytes public key randomly.
  ```
  uart:~$ keygen
  Public key:
  0xa2c0acfbfc35763cf0ca221f2f44a42b3767dc168d00a99f3952ac5ad05cc25f4d8069a79b002ae665b9ad35ce800a0e
  ```
- *publickey*: shows the last public key that has been generated.
  ```
  uart:~$ publickey
   Public key:
   0xa2c0acfbfc35763cf0ca221f2f44a42b3767dc168d00a99f3952ac5ad05cc25f4d8069a79b002ae665b9ad35ce800a0e
  ```
- *signature "publickey" "message"*: produces a 96-bytes signature with the message that has been introduced and after choosing a public key that has had to be generated before.
  ```
  uart:~$ signature 0xa2c0acfbfc35763cf0ca221f2f44a42b3767dc168d00a99f3952ac5ad05c
  c25f4d8069a79b002ae665b9ad35ce800a0e 5656565656565656565656565656565656565656565
  656565656565656565656
  Signature:
  0xa60e2f24827943379f8377d6bf7a126ef8def56b08ca4cdd958954b5f56091f7cd49d251481f4cd6316396a2a4f4398c09e2cfda6ea16416dffdac687cf06db0e4d3a0ec83b4016c835b27f84325342199f724abd092cb7957177f5f30dcbe19
  ```
- *verify "public_key" "message" "signature"*: signature verification.
  ```
  uart:~$ verify 0x8077842bcf8d16d842d3b9b09ad78f717468577ddb189c02ded347d551aad50
  cbbdf37920b522a164cdf426bed9ac321 5656565656565656565656565656565656565656565656
  565656565656565656 0x8cea6d7e2e07efd6b6e56c2dabd04ec07dead82b5806e63b85945d1d75a
  6f53bc9d2dd6df5fc45ed79a5fd80c8d6a61509162faf1a9b0bd894461c4a6a4ac5db427e18694b1
  d2be94cd64bb25426238ec85bf4767d892fea09825c07414421ec
  Success
  ```
- *getkeys*: returns the public keys that have been generated.
  ```
  uart:~$ getkeys
  {'keys':['a2c0acfbfc35763cf0ca221f2f44a42b3767dc168d00a99f3952ac5ad05cc25f4d8069a79b002ae665b9ad35ce800a0e']}
  ```


## Implementations :pick:
**1. cli**: This project uses blst static library that has been compiled for Cortex-M33 architecture.

PS. The *prj.conf* file has been modified because default size caused stack overflow from the UART thread. Current size is 49152 bytes.

**2. secure_module**: This module contains blst function calls that involve usage and storage of secret keys, using Secure Partition Manager (SPM).

## Test
"test" folder contains a test coded in [Go](https://golang.org/) language. In order to run it, you must install Go and run `go run .\main.go .\utils.go [-v] COMport` command in a terminal or `go mod init test`, `go mod tidy`, `go build` and then `test.exe [-v] COMport`. Optional argument `-v` will show a detailed output of the tests. `COMport` is the board's serial port name (e.g. COM4, /dev/ttyS3).
This test will do the following:
- Generate 10 keypairs (the maximum allowed by the board) and check that all keys are different.
- Attempt to generate an extra key pair and confirm the board refuses to do that.
- Perform a signature of a message with the wrong size an confirm the board refuses to do that.
- Perform a signature of a message with the right size and check that the signature is properly verified.

Output example:
```
user@user:~/bls-hsm/test$ go mod init test
go: creating new go.mod: module test
user@user:~/bls-hsm/test$ go mod tidy
go: finding github.com/tarm/serial latest
go: downloading golang.org/x/sys v0.0.0-20210630005230-0f9fa26af87c
go: extracting golang.org/x/sys v0.0.0-20210630005230-0f9fa26af87c
user@user:~/bls-hsm/test$ go build
go: finding golang.org/x/sys v0.0.0-20210630005230-0f9fa26af87c
user@user:~/bls-hsm/test$ ./test /dev/ttyACM2
Running tests...
Delete previous keys..........PASSED
Generate 10 keys..............PASSED
2.209466751s elapsed
Retrieve generated keys.......PASSED
Check keys are different......PASSED
Try to generate extra key.....PASSED
Sign msg with wrong length....PASSED
Sign correct msg..............PASSED
873.448751ms elapsed
Verify signature..............PASSED
2.970017188s elapsed
Delete keys...................PASSED
RESULTS:
----------------------------------------
Total.........................9/9
----------------------------------------
```

## To Do: :ballot_box_with_check:
- [x] Benchmark command.
- [x] Shell script for setting up the work environment.
- [x] Include Nordic Security Backend in order to take advantage of the 144-byte random number used for the 32-byte vector in secret key generation function.
- [x] Execution of the code in ARM TrustZone.
