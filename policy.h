#include "MonteCarlo.h"
#include "NN.h"
#include <string.h>

class Policy {
public:
	Policy() = delete;
	

	static Pair Greedy (board &before, const PIECE &piece) {
		
		std::random_device rd;
		std::default_random_engine engine(rd());
		
		if ( before.check_Piece_onBoard(piece) == FAIL ) return {};
		//find whether can eat piece or not

		std::vector<Pair> pos = before.find_piece(piece, EAT);
		if ( !pos.empty() ) {
			// take eat action first
			return pos[0];	
		}
		else {
			//take move action second
			pos = before.find_piece(piece, MOVE);
			if ( !pos.empty() ) {
				std::shuffle(pos.begin(), pos.end(), engine);
				return pos[0];
			}
		}
		return {};

	}

	static Pair MCTS (board &before, const PIECE &piece, const int &simulation_times) {
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

	static Pair NN (board &before, const PIECE &piece) {
		board now = before;

		// get legal action
		auto moves = now.get_available_move(piece);

		double max_val = -2.0;
		Pair best_move{};

		std::cerr << "Net take action: \n\n";

		// enumerate all moves
		for (auto &mv : moves) {
			board next = now;
			next.move(mv.prev, mv.next, piece);

			const int stacks = 3; // black*1 + white*1 + take_turn

			float tensor_stack[stacks * board::SIZE];
			generate_states(tensor_stack, next, piece);

			torch::Tensor boards = torch::from_blob(tensor_stack, {1, 3, 6, 6}); // shape: [batch_size, stacks, row, col]
			torch::Tensor pred_val = Net->forward(boards);

			double pred = pred_val[0].item<double>();
			
			// find the best V(s)
			if (pred > max_val) {
				max_val = pred;
				best_move = mv;				
			}
		}
		
		return best_move;		
	}
};
