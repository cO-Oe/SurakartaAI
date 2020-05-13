#include "MonteCarlo.h"
#include "NN.h"
// #include "train.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>


class Policy {
public:
	Policy() = delete;
	

	static Pair Greedy (board &before, const PIECE &piece) {
		
		std::random_device rd;
		std::default_random_engine engine(rd());
		std::cout << "Greedy take action\n";
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
		// 10% epsilon
		const int epsilon = 10;
		Pair best_move{};

		std::cerr << "Net take action: \n";

		std::random_device rd;
		std::default_random_engine engine(rd());
		std::uniform_int_distribution<int> distribution(0,100);

		int prob = distribution(engine);

		//if ( prob >= epsilon) {	// 1-epsilon : NN output
			board now = before;

			if (piece == WHITE) {
				now.flip_color();
			}
			//std::cerr << "exploit: " << prob << '\n';

			// get legal action
				
			auto moves = now.get_available_move(BLACK);

			double max_val = -2.0;
			std::vector<Pair> bags;

			// enumerate all moves
			for (auto &mv : moves) {
				board next = now;
				next.move(mv.prev, mv.next, piece);

				const int stacks = 1; // black*1 + white*1 + take_turn

				
				float tensor_stack[stacks * board::SIZE];
				generate_states(tensor_stack, next);
				torch::Tensor boards = torch::from_blob(tensor_stack, {1, 1, 6, 6}).to(device); // shape: [batch_size, stacks, row, col]
				torch::Tensor pred_val = Net->forward(boards).to(device);

				double pred = pred_val[0].item<double>();
				// std::cerr << "Q value: " << pred << '\n';	
				// find the best V(s)
				if ( pred > max_val) {
					max_val = pred;
					best_move = mv;
				}
			}
	//	}
		// else { // epsilon : random move
		// 	// std::cerr << "explore: " << prob << '\n';;
		// 	// std::vector<Pair> ea = before.find_piece(piece, EAT), mv = before.find_piece(piece, MOVE), pos;
		// 	// pos.reserve(ea.size() + mv.size());
		// 	// pos.insert(pos.end(), ea.begin(), ea.end());
		// 	// pos.insert(pos.end(), mv.begin(), mv.end());
		// 	// std::vector<Pair> ea = before.find_piece(piece, EAT), mv = before.find_piece(piece, MOVE), pos;
		// 	std::vector<Pair> legal_mv = before.get_available_move(piece);

		// 	if ( !legal_mv.empty() ) {
		// 		std::shuffle(legal_mv.begin(), legal_mv.end(), engine);
		// 		best_move = legal_mv[0];
		// 	}
		// }

		return best_move;		
	}
};
