#include "MonteCarlo.h"
#include "NN.h"
// #include "train.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <iomanip>
class episode;

class Policy {
public:
	Policy() = delete;
	

	static Pair Greedy (const board &before, const PIECE &piece) {
		
		std::random_device rd;
		std::default_random_engine engine(rd());
		std::cout << "Greedy take action\n";
		if ( before.check_Piece_onBoard(piece) == FAIL ) return {};
		//find whether can eat piece or not
		board tmp = before;
		std::vector<Pair> pos = tmp.find_piece(piece, EAT);
		if ( !pos.empty() ) {
			// take eat action first
			return pos[0];	
		}
		else {
			//take move action second
			pos = tmp.find_piece(piece, MOVE);
			if ( !pos.empty() ) {
				std::shuffle(pos.begin(), pos.end(), engine);
				return pos[0];
			}
		}
		return {};

	}
	static Pair Manual (const board &before, const PIECE &piece) {
		board tmp = before;
		auto moves = tmp.get_available_move(piece);
		std::cout << "Available moves: \n";
		int cnt = 0;
		for(auto &mv_pair : moves) {
			std::cout << "(" << std::setw(2) << int(mv_pair.prev) << ", " << std::setw(2) << int(mv_pair.next) << ") ";
			if (++cnt % 4 == 0){
				std::cout << '\n';
				cnt = 0;
			}
		}
		std::cout << '\n';
		std::cout << "Please input moves: (format:pos1, pos2)\n";
		int x1, x2;
		std::cin >> x1 >> x2;
		
		return {x1, x2};
	}

	static Pair MCTS (board &before, const PIECE &piece, const int &simulation_times) {
		if ( before.check_Piece_onBoard(piece)==FAIL ) return {};//lose
		
		MonteCarloTree tree;
		tree.reset(before);
		std::cout << "MCTS take action\n";
		const int &simulationtime = simulation_times;
		int count_sim = 0;
		while (count_sim < simulationtime) {
			tree.tree_policy();
			count_sim++;
		}
		
		// tree.root->showchild();

		Pair best_move = tree.root->best_child();
		return best_move;
	}

	static Pair NN (const board &before, const PIECE &piece, auto &prev_board, std::string &mode) {
		// 10% epsilon to random move
		const int epsilon = (mode=="train") ? 10 : 0 ;
		Pair best_move{};

		std::cout << "Net take action: \n";

		std::random_device rd;
		std::default_random_engine engine(rd());
		std::uniform_int_distribution<int> distribution(0,100);

		int prob = distribution(engine);

		if ( prob >= epsilon) {	// 1-epsilon : NN output
			board now_b = before;
					
			board prev_b;
			if (prev_board.size() >= 2)
				prev_b = *( prev_board.end() - 2);
			/*
			if (piece == WHITE) {
				now_b.flip_color();
				prev_b.flip_color();
			}
			*/
			// get legal action
				
			auto moves = now_b.get_available_move(piece);
			if ( moves.size() == 0  )
				return {};
			double max_val = -2.0;
			const unsigned stack_size = 3;	
			
			std::vector<Pair> bags;
			std::vector<board> input_boards(stack_size);
			input_boards[0] = prev_b;
			input_boards[1] = now_b;
			// std::cout << "prev_b:\n" << prev_b;
			// std::cout << "now_b:\n" << now_b;
			
			// enumerate all moves
			for (auto &mv : moves) {
				board next = now_b;
				next.move(mv.prev, mv.next, piece);

				input_boards[2] = next;
				float tensor_stack[ board::SIZE * ( stack_size * 2 + 1) ];
				
				generate_states(tensor_stack, input_boards, piece);
				torch::Tensor boards = torch::from_blob(tensor_stack, {1, 7, 6, 6}).to(device); // shape: [batch_size, stacks, row, col]
				// std::cout << boards << '\n';
				// std::cout << "pieces: " << piece << '\n';
				torch::Tensor pred_val = Net->forward(boards).to(device);

				double pred = pred_val[0].item<double>();
				// std::cerr << "Q value: " << pred << '\n';	

				// find the best V(s)
				if ( pred > max_val) {
					max_val = pred;
					best_move = mv;
				}
			}
//			board ttpp = now_b;
//			ttpp.move(best_move.prev, best_move.next, BLACK);
//			std::cout << "next_b:\n" << ttpp;

			// transform coordinate
			/*
			if (piece==WHITE) {
				auto transform_coord = [COL=board::COL] (char &pos) { 
					pos = ( (COL-1) - (pos/COL))*COL + ((COL-1) - (pos%COL));
				};
				transform_coord(best_move.prev);
				transform_coord(best_move.next);
			}
			*/
		}
		else { // epsilon : random move
			board tmp = before;
		 	std::vector<Pair> legal_mv = tmp.get_available_move(piece);

		 	if ( !legal_mv.empty() ) {
		 		std::shuffle(legal_mv.begin(), legal_mv.end(), engine);
		 		best_move = legal_mv[0];
		 	}
		}
		
		return best_move;		
	}
};
