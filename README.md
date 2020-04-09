# SurakartaAI
This Repository build a [Surakarta Game](https://en.wikipedia.org/wiki/Surakarta_(game)) framework and implement [MCTS (Monte Carlo Tree Search)](https://en.wikipedia.org/wiki/Monte_Carlo_tree_search) technique to train AI.
In the future, we will combine MCTS with [Deep Q-Learning](https://en.wikipedia.org/wiki/Q-learning#Deep_Q-learning) or other appropriate RL, ML, DRL technique.
## How to Run?
**Important: You should install [C++ libtorch libaray](https://pytorch.org/cppdocs/installing.html) first**
1. git clone this Repository
2. ```cd SurakartaAI```
3. ``` mkdir build```
4. ``` cd build```
5. ``` cmake -DCMAKE_PREFIX_PATH=/path/to/yourlibtorch ..```
6. ``` cmake --build .```
7. ```./surakarta (--total=1000 --block=100 (default argument))``` 
## How to Modify Strategy?
- By deault, The two agent **player** and **envir** move according to two different strategy, **MCTS** and **Eat First Greedy**.
- Modify ```agent.h``` and choose the strategy you want in ```policy.h```
