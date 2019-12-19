# Puissance4 

A connect 4 game made in `C` using the `ncurses` library.

Developped by :
-Tarek AINOUZ
-Kévin DONIN DE ROSIERE
-Hamza MEKNASSI

## Installation

Run the `make` command to compile.
Start the server in a first terminal using `./serveur portNumber numberOfGames`
Where `portNumber` is the UDP port number used by the server.
And `numberOfGames` is the number of games able to be played simultaneously.

Start the first client in another terminal using this line :
`./client portNumber`

finally start a second one in another terminal.

## Files:
- `client.c` -the client
- `serveur.c` -the server
- `messages.h` -File containing the date structures used for every exchange
- `makefile` -File used to compile

## In the future
The game is entirely in french, I might add an english translation for both the game and the comments.
