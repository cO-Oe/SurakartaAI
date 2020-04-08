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
		// torch::Tensor max_val = torch::tensor({-2.0});
		double max_val = -2.0;
		Pair best_move{};
		// get V(s)
		std::cerr << "Net take action: \n\n";

		// for(const auto& p : Net->parameters()) {
		// 	std::cerr << p << '\n';
		// }
		for (auto &mv : moves) {
			board next = now;
			next.move(mv.prev, mv.next, piece);

			const int stacks = 3; // black + white + take_turn
			// if(piece == WHITE)
				// memset(turns, 1, sizeof(turns));
			// else
				// memset(turns, 0, sizeof(turns));

			board test;
					
			float tmp_stack[3*6*6];
			float* tensor_stack = tmp_stack;

			int idx = 0;
			for(int i=0; i<36; i++)
				*(tensor_stack + (idx++)) = next.black_board().convert()[i];
			for(int i=0; i<36; i++)
				*(tensor_stack + (idx++)) = next.white_board().convert()[i];
			for(int i=0; i<36; i++)
				*(tensor_stack + (idx++)) = (piece == BLACK) ? 1:0;


			torch::Tensor boards = torch::from_blob(tensor_stack, {3, 6, 6});
			
			// std::cerr << boards << '\n';
			torch::Tensor pred_val = Net->forward(boards);
			double pred = pred_val.item<double>();
			std::cerr << "Move: (" << int(mv.prev) << ", " << int(mv.next) << ") Value : " << pred << '\n'; 
			
			if (pred > max_val) {
				max_val = pred;
				best_move = mv;				
			}
		}
		std::cerr << '\n';
		
		return best_move;		
	}
};
