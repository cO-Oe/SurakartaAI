/**

The main program of Surakarta Game
------------------------------------------
This is the Project of Surakarta Game AI

Construced by Clive Wu and Mark Chang
Instructed by Professor I-Chen Wu

**/
#include <iterator>
#include <unistd.h>
#include <typeinfo> 
#include <fstream>

#include "agent.h"
#include "episode.h"
#include "statistic.h"
#include "train.h"


int main(int argc, char* argv[]) {
	std::cout << "Surakarta Demo: ";
	std::copy(argv, argv + argc, std::ostream_iterator<char *>(std::cout, " "));
	std::cout << "\n\n";

	size_t total = 5, block = 0;
	std::string load_module;
	for (int i{1}; i < argc; i++) {
		std::string para(argv[i]);
		if (para.find("--total=") == 0) {
			total = std::stoull(para.substr(para.find("=") + 1));
		}
		else if (para.find("--block=") == 0) {
			block = std::stoull(para.substr(para.find("=") + 1));
		}
		else if (para.find("--load=") == 0) {
			load_module = para.substr(para.find("=") + 1);
		}
	}
	if (!load_module.empty()){
		torch::load(Net, load_module);
	}
	
	if (torch::cuda::is_available()) {
		std::cout << "Train on GPU\n";
		device = torch::kCUDA;
	}
	Net->to(device);

	statistic stat(total, block);

	player play {BLACK};//0
	envir env {WHITE};//1
	int cnt = 0;
	episode train_set_game;

	while (!stat.is_finished()) {

		board b;

		stat.open_episode("W:B");
		episode& game = stat.back(); 

		while ( true ) {
			// player first (left)
			agent& who = game.take_turns(play, env);
			board prev_b = b;

			Pair mv = who.take_action(b);
			
			// end game
			if (mv == Pair{})
				break;
			game.record_action(mv, prev_b, who.get_piece());
			train_set_game.record_train_board(b, who.get_piece());
			
			// if (who.get_piece() == BLACK)
				// std::cout << b << '\n';
			// sleep(3);
		}
		agent& win = game.get_winner(env, play);
		stat.close_episode("end", win, b);
		train_set_game.train_close_episode(win, b);

		// train Network 
		if ( (++cnt) % 100  == 0 ){
			std::cout << "Cnt : " << cnt << '\n';
			train_Net(train_set_game); // episode, epochs
			train_set_game.clear();
			torch::save(Net, "model.pt");
			// torch::save(optimizer, "optimizer.pt");
		}
	}
	return 0;
}
