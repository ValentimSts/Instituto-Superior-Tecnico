
+------------------------------------------------+
|                  README.txt                    |
|  File explaining the execution and structure   |
|               of the Project                   |
|  Group 36 (Filipa Magalhaes & Valentim Santos) |
+------------------------------------------------+



-------------------- CONTENTS: -------------------
-----> FILES:

-> Makefile
    - "make": creates both player and GS executables;
    - "make clean": deletes both executables (player and GS), as well as
                    every file in the directories 'games' and 'scores'.

-> "player.c": Player application.
-> "GS.c": Game server.

-> "word_eng.txt": Sequence of possible words to be guessed.


-----> DIRECTORIES:

-> 'common'
    - contains the "common.h" file.

-> 'games'
    [- players directories containing each player's game files]

-> 'scores'
    [- players scores files]

-> 'images'
    - contains the images (hints) to be provided to the player when requested.


--------------- USER INSTRUCTIONS: ---------------

 1) Compile the program by running 'make';
 2) Run './player [-p] [-n]' and './GS word_eng.txt [-p] [-v]' on seperate terminals;
 3) Invoke the desired commands on the './player' terminal.


-------------------- TO NOTE: --------------------

 1) All the images must be stored in the 'images/' directory;
 2) The word_eng.txt file must have the appropriate hint paths,
    (e.g. dog.jpg -> images/dog.jpg).


--------------- PROGRAM STRUCTURE: ---------------

+
|
|_____common
|  |_____common.h
|
|
|
|_____player.c
|
|
|
|_____GS.c
|
|_____word_eng.txt
|
|_____images
|  |____[game images]
|
|_____games
|  |_____[player dirs]
|     |_____[player's game files]
|
|_____scores
|  |_____[player scores]
|
|
|
|_____Makefile
|
+