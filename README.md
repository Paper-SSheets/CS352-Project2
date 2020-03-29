[comment]: <> (This document was created and tested with JetBrain's MD extension.
               Apologies if it is not well-formatted for you when viewing.)

# Multi-Threaded Text File Encryptor
- An implementation of Project II of the course COM S 352: Introduction to 
  Operating Systems.

## Simple Testing
- The tests for this will describe basic input/output for the program,
  as well as how to setup. Do experiment around, this predominantly 
  follows the structure in the project's documentation.
  
#### Creating the executable.
- Simply type `make` after navigating to the root of the repository.
- You should receive output similar to the following:
  ```
    [smsheets@pyrite-n3 steven_sheets_proj2]$ make
    gcc encrypt.c -o encrypt -lpthread
  ```
  - The `Makefile` also supports `make clean`, which 
    will simply remove the executable.
  ```
    [smsheets@pyrite-n3 steven_sheets_proj2]$ make clean
    rm -f encrypt
  ```
        
#### Simple Testing
- Within the `/docs` folder, I have left the given `infile2` and `outfile2` 
  files for testing. I chose the the larger of the two files for predominant 
  testing, as I expect it to find more possible errors. 
- After creating the executable, run `./encrypt docs/infile2 test_infile2`. 
  Your current state should be similar to the following:
  ```
  [smsheets@pyrite-n3 steven_sheets_proj2]$ ./encrypt docs/infile2 test_outfile2
  Enter buffer size: 
  ```
- From here, let's use the recommended buffer size used in the documentation.
  We will receive a rather large output, as well as creating a new file named
  `test_outfile2` that will appear in the main directory. Your terminal should 
  look like the following.
  ``` 
  [smsheets@pyrite-n3 steven_sheets_proj2]$ ./encrypt docs/infile2 test_outfile2
  Enter buffer size: 1000
  Input file contains
  A: 14
  B: 3
  C: 5
  D: 3
  E: 23
  F: 5
  G: 1
  H: 8
  I: 13
  K: 1
  L: 6
  M: 2
  N: 10
  O: 11
  P: 7
  R: 10
  S: 12
  T: 22
  U: 6
  V: 1
  Y: 2
  Z: 1
  Output file contains
  A: 6
  B: 8
  C: 4
  D: 9
  E: 12
  F: 11
  G: 5
  H: 6
  I: 7
  J: 5
  K: 1
  L: 4
  M: 5
  N: 9
  O: 10
  P: 6
  Q: 5
  R: 4
  S: 18
  T: 10
  U: 12
  V: 3
  X: 1
  Y: 2
  Z: 3
  ```
- Through the magic of `CTRL+C` + `CTRL+F` on the documentation, we ensure
  that this is the expected output. Now, we just have to ensure that the 
  outputted file is also correct. Open up `outfile2` (the given expected file)
  and your newly created file `test_outfile2`. Both their contents should match
  and look like the following: 
  ```
  Ugit hs bmouges ranolf hnqtt ghlf.
  Moud tizt onn-bkpizbfsid bhbqadsesr 
  (ljje ugetd: "{}:<>()*&^%)
  asd sjlpmx cpoifc tp shf nuuouu eimd,
  aoc asd nps rfoossee hn uge dnuoss.
  
  Cd svqe un tfrt bfajmsu z vbqifsy pe bveffq sjyet!

  ```
  Note: The newline at the end of the file is expected.
  