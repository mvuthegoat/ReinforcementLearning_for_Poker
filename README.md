# ReinforcementLearning_for_Poker
Find the Game Theory Optimal (GTO) strategy for a Kuhn poker game using C++

## Kuhn Poker rules:
Kuhn Poker is a simple 3-card poker game by Harold E. Kuhn [8]. 
- First, two players each ante 1 chip, i.e. bet 1 chip blind into the pot before the deal. 
- Three cards, Jacks, Queens, Kings, are shuffled, and one card is dealt to each player and held as private information. Play alternates starting with player 1. 
- On a turn, a player may either pass or bet. A player that bets places an additional chip into the pot. When a player passes after a bet, the opponent takes all chips in the pot. When there are two successive passes or two successive bets, both players reveal their cards, and the player with the higher card takes all chips in the pot.

***Source: https://en.wikipedia.org/wiki/Kuhn_poker***


## Algorithms 
2 algorithms used by the solver to find Nash-equilibrium strategies for this Kuhn poker game:

- Counterfactual Regret Minimization (http://modelai.gettysburg.edu/2013/cfr/cfr.pdf)

- State-of-the-art Discouted CFR (https://cdn.aaai.org/ojs/4007/4007-13-7066-1-10-20190704.pdf?_gl=1*le8l1f*_ga*Mzc2NzU4NDM0LjE2ODE3NjcxMDM.*_ga_CKNBPFEYPG*MTY4MTc2NzEwMi4xLjAuMTY4MTc2NzExMS4wLjAuMA..)

## Requirements

To run this project, you need to have the following installed:

- a compiler like GCC (GNU Compiler Collection) that supports C++17 or above

## Usage

To compile the program, first type `make`. 

Then after the program finishes compiling, type the following command in your terminal to change your working directory to the build directory and run the test file

```
cd build && ./test
```

You can select the algorithm for the solver to run by modifying the line `string algo = "discounted_cfr";` in `test.cpp`

Here, 

- `string algo = "discounted_cfr";` selects the Discounted_CFR algorithm
- `string algo = "cfr";` selects the normal CFR algorithm
- Also, the line `trainer.set_param(1.5, 0.5, 2);` sets the parameter for the Discounted_CFR algorithm; you can comment out this line if not using or input your own params if using Discounted_CFR

## Results

The solver will first output the **Average game value** of player 1 and the optimal strategies for both players:

- The **Game_state** column shows the current game state
- The **PASS** column shows the probability that the current player should *check/fold*
- The **BET** column shows the probability that the current player should *call/bet*

Then, the solver prints the **Exploitablity** metric of the GTO (Game Theory Optimal) strategy and the corresponding **Expected Value** for each action

