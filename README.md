[comment]: <> (This document was created and tested with JetBrain's MD extension.
               Apologies if it is not well-formatted for you when viewing.)

# UNIX Shell
- An implementation of the UNIX Shell project described at the end of 
  Chapter 3: Processes, of the textbook **Operating System Concepts 
  (Abraham Silberschatz, Greg Gagne, Peter B. Galvin) [2018]**.

## How to Test
- The tests for this will describe basic input/output for the program,
  as well as how to setup. Do experiment around, this predominantly 
  follows the structure in the book's documentation.
  
#### Creating the executable.
- Simply type `make` after navigating to the root of the repository.
- You should receive output like this:
  ```
  [smsheets@pyrite-n2 steven_sheets_proj1]$ make
  gcc osh.c -o osh
  ```
  - The `Makefile` also supports `make clean`, which 
    will simply remove the executable.
        
#### Section I: Overview
- After creating the executable, run `./osh`. Your current state
  should look like this:
  ```
  [smsheets@pyrite-n2 steven_sheets_proj1]$ ./osh
  osh>
  ```
  - Run `cat osh.c`, your output should be the source code of this shell.
  
#### Section II: Executing Command in a Child Process
- Run `ps -ael` at the `osh>` prompt. You should get some exciting
  output. The values stored in the `args` array should be:
  ```
  args[0] = "ps"
  args[1] = "-ael"
  args[2] = NULL
  ```

#### Section III: Creating a History Feature
- Our "history" feature is incredibly basic, as it only supports
  running the immediate previously inputted command. There are
  two possibilities worth testing here:
  - There _is not_ a previous command:
    - The easiest way to test this is by running the shell and 
      immediately looking for history. Should look something like this: 
      ```
      osh>exit
      [smsheets@pyrite-n2 steven_sheets_proj1]$ ./osh
      osh>!!
      No commands in history.
      ```
  - There _is_ a previous command:
    - To test this, let's use a simple command and ensure that
      it repeats when the user inputs "!!". Should look something
      like this:
      ```
      [smsheets@pyrite-n2 steven_sheets_proj1]$ ./osh
      osh>ls
      cmake-build-debug  docs       Makefile  osh    output.txt
      CMakeLists.txt     input.txt  old.c     osh.c  README.md
      osh>!!
      cmake-build-debug  docs       Makefile  osh    output.txt
      CMakeLists.txt     input.txt  old.c     osh.c  README.md
      osh>Nioce.
      An error has occurred.
      osh>
      ```

#### Section IV: Redirecting Input and Output
- Here's where testing gets more interesting. I have set up a
  `input.txt` file that contains the alphabet in reverse order.
  To save space/time, I'll just show sample terminal input/output.
  Note that `output.txt` will be overridden in these examples.
  ```
  [smsheets@pyrite-n2 steven_sheets_proj1]$ ./osh
  osh>ls > output.txt
  osh>cat output.txt
  cmake-build-debug
  CMakeLists.txt
  docs
  input.txt
  Makefile
  osh
  osh.c
  output.txt
  README.md
  osh>ls
  cmake-build-debug  docs       Makefile  osh.c       README.md
  CMakeLists.txt     input.txt  osh       output.txt
  osh>
  ```
  
  ```
  [smsheets@pyrite-n2 steven_sheets_proj1]$ ./osh
  osh>sort < input.txt
  a
  b
  c
  d
  e
  f
  g
  h
  i
  j
  k
  l
  m
  n
  o
  p
  q
  r
  s
  t
  u
  v
  w
  x
  y
  z
  osh>
  ```
  
#### Section V: Communication via a Pipe
- To test this, confirm pipe usage works as expected. Sample usage:
```
[smsheets@pyrite-n2 steven_sheets_proj1]$ ./osh
osh>ls -l | less
total 401
drwxr-x---. 3 smsheets domain users    60 Feb 13 18:08 cmake-build-debug
-rwxr-x---. 1 smsheets domain users   143 Feb 13 19:36 CMakeLists.txt
drwxr-x---. 2 smsheets domain users    80 Feb  9 00:24 docs
-rwxr-x---. 1 smsheets domain users    78 Feb 13 17:52 input.txt
-rwxr-x---. 1 smsheets domain users    55 Feb 13 17:46 Makefile
-rwxr-x--x. 1 smsheets domain users 23232 Feb 13 19:28 osh
-rwxr-x---. 1 smsheets domain users  9997 Feb 13 19:28 osh.c
-rwxr-x---. 1 smsheets domain users    88 Feb 13 19:39 output.txt
-rwxr-x---. 1 smsheets domain users  3482 Feb 13 19:40 README.md
...
```
