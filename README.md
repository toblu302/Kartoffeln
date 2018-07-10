## Kartoffeln
A UCI-compatible Chess Engine written in C++

### About
Uses 64-bit bitboards to represent the current state of the game.

#### Search
* Negamax with alpha-beta cutoff
* Iterative deepening
* Most Valuable Victim - Least Valuable Attacker move ordering.

#### Evaluation
* Material counting
* PSQT (midgame and endgame with interpolation)

### Custom UCI commands
perft [depth], i.e "perft 4".

### Thanks
Special thanks to the friendly folks on ##chessprogramming
