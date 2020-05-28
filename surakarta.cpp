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

void man_help() {
	std::cout << "Usege: ./surakarta [OPTION]...\n\n";
	std::cout << "A Surakarta Game Engine implementing different strategy, ex: Greedy, MCTS, AlphaGo(only value function), enjoy it!\n\n";
	
	std::cout << "[OPTION]...\n";
	std::cout << std::left << std::setw(30);
	std::cout << "  --total=NUM_OF_GAME" << "Set total numbers of games to run. Default value: 5\n\n";
	std::cout << std::left << std::setw(30);
	std::cout << "  --load=PATH_TO_FILE " << "Enter file name (path) to load model (Network parameters)\n" <<  std::setw(30) << " " << "for training and playing\n\n";
	std::cout << std::left << std::setw(30);
	std::cout << "  --save=PATH_TO_FILE " << "Enter file name (path) to save Network parameters\n" << std::setw(30) << " " << "after training.\n\n";
	std::cout << std::left << std::setw(30);
	std::cout << "  --mode=MODE" << "Two Modes: \"train\" for training,  \"eval\" for evaluation.\n" << std::setw(30) << " " << "Default value: \"train\"\n\n";
	std::cout << std::left << std::setw(30);
	std::cout << "  --black=POLICY" << "Three POLICYS: \"Greedy\", \"MCTS\", \"CNN\", \"Manual\" to choose.\n";
	std::cout << std::left << std::setw(30);
	std::cout << "  --white=POLICY" << "\n\n";
}

int main(int argc, char* argv[]) {
	std::cout << "Surakarta Demo: ";
	std::copy(argv, argv + argc, std::ostream_iterator<char *>(std::cout, " "));
	std::cout << "\n\n";

	size_t total = 5, block = 0;
	std::string load_module;
	std::string save_module;
	std::string mode = "train";
	std::string black_policy = "CNN";
	std::string white_policy = "Greedy";
	const int train_epoch = 1;  
	const int save_epoch = 250;
	
	for (int i{1}; i < argc; i++) {
		std::string para(argv[i]);
		if (para.find("--help") == 0) {
			man_help();
			return 0;
		}
		else if (para.find("--total=") == 0) {
			total = std::stoull(para.substr(para.find("=") + 1));
		}
		else if (para.find("--block=") == 0) {
			block = std::stoull(para.substr(para.find("=") + 1));
		}
		else if (para.find("--load=") == 0) {
			load_module = para.substr(para.find("=") + 1);
		}
		else if (para.find("--save=") == 0) {
			save_module = para.substr(para.find("=") + 1);
		}
		else if (para.find("--mode=") == 0) {
			mode = para.substr(para.find("=") + 1);
		}
		else if (para.find("--black=") == 0) {
			black_policy = para.substr(para.find("=") + 1);
		}
		else if (para.find("--white=") == 0) {
			white_policy = para.substr(para.find("=") + 1);
		}
	}
	if (!load_module.empty()){
		torch::load(Net, load_module);
	}
	
	if (torch::cuda::is_available()) {
		std::cout << "Train on GPU\n";
		device = torch::kCUDA;
	}
	else {
		device = torch::kCPU;
		std::cout << "Train on CPU\n";
	}
	Net->to(device);


	statistic stat(total, block);

	player play {BLACK, black_policy};  // 0
	envir env {WHITE, white_policy};  // 1
	int cnt = 0;
	episode train_set_game;
	int save_cnt = 0;

	while (!stat.is_finished()) {

		board b;

		stat.open_episode("W:B");
		episode& game = stat.back(); 
		std::cout << b << '\n';
		while ( true ) {
			// player first (left)
			agent& who = game.take_turns(play, env);
			
			Pair mv = who.take_action( b, game.ep_boards, mode );
			
			// Print for Debug 
			std::cout << who.get_piece() << "'s turn.\t";
	 		std::cout << "Move from (" << mv.prev / 6 << ", " << mv.prev % 6 << ") to (" 
		 	<< mv.next / 6 << ", " << mv.next % 6 << ")\n";
			
			// end game
			if (mv == Pair{} || game.step() > episode::game_threshold)
				break;
			game.record_action(mv, b, who.get_piece());
			train_set_game.record_train_board(b, who.get_piece());
			
			std::cout << b << '\n';
		}

		// check draw game
		if ( game.check_draw(b) ) {
			train_set_game.train_close_episode(new agent());
			stat.close_episode("end", new agent(), b);
		}	
		else {
			agent &win = game.get_winner(env, play, b);
			// winner agent
			std::cout << win.name() << " Wins\n\n";	
			train_set_game.train_close_episode( &win );
			stat.close_episode("end", &win, b);
		}
		// train Network 
		if ( (++cnt) % train_epoch  == 0 && mode=="train" ) {
			std::cout << "Epoch : " << cnt << '\n';
			train_Net(train_set_game); // episode, epochs
			train_set_game.clear();
		}

		// save Network
		if ( (cnt) % save_epoch == 0 && !save_module.empty()) {
			std::cout << "Checkpoint in epoch " << cnt << '\n';
			save_cnt++;
			std::string module_name = save_module + std::to_string(save_cnt*save_epoch) + ".pt";
			torch::save(Net, module_name);
		}
	}
	return 0;
}
