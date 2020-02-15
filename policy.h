#include "action.h"
#include "MonteCarlo.h"

class Policy {
public:
	Policy() = delete;
	

	static Pair Greedy (board &before, const bool &piece) {
		
		std::random_device rd;
		std::default_random_engine engine(rd());
		
		if ( before.check_Piece_onBoard(piece) == FAIL ) return {};
		//find whether can eat piece or not

		std::vector<Pair> pos = before.eat_piece(piece);
		if ( !pos.empty() ) {
			// take eat action first
			return pos[0];	
		}
		else {
			//take move action second
			pos = before.move_piece(piece);
			if ( !pos.empty() ) {
				std::shuffle(pos.begin(), pos.end(), engine);
				return pos[0];
			}
		}
		return {};

	}

	static Pair MCTS (board &before, const bool &piece, const int &simulation_times) {
		if ( before.check_Piece_onBoard(piece)==FAIL ) return {};//lose
		
		MonteCarloTree tree;
		tree.reset(before);

		const int &simulationtime = simulation_times;
		int count_sim = 0;
		while (count_sim < simulationtime) {
			tree.tree_policy();
			count_sim++;
		}
		
		tree.root->showchild();

		Pair best_move = tree.root->best_child();
		return best_move;
	}
};
