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

int main(int argc, char* argv[]) {
	std::cout << "Surakarta Demo: ";
	std::copy(argv, argv + argc, std::ostream_iterator<char *>(std::cout, " "));
	std::cout << "\n\n";

	size_t total = 5, block = 0;
	for (int i{1}; i < argc; i++) {
		std::string para(argv[i]);
		if (para.find("--total=") == 0) {
			total = std::stoull(para.substr(para.find("=") + 1));
		}
		else if (para.find("--block=") == 0) {
			block = std::stoull(para.substr(para.find("=") + 1));
		}
	}

	statistic stat(total, block);

	player play {BLACK};//0
	envir env {WHITE};//1

	while (!stat.is_finished()) {

		board b;
		// play.open_episode("~:B");
		// env.open_episode("~:W");

		stat.open_episode("W:B");
		episode& game = stat.back(); 
		
		while ( true ) {
			// player first (left)
			agent& who = game.take_turns(play, env);
		
			Pair mv = who.take_action(b);
			
			if (mv != Pair{}){
				game.record_action(mv);
			}
			// end game
			else {
				agent& win = game.winner(env, play);
				// std::string winner = win.name();
				stat.close_episode("end", win, b);
				break;
			}
			std::cout << b << '\n';
		}
		// agent& win = game.last_turns(play, env, b);
		// std::string winner = (win.get_piece() == 1 ? "play" : "env" );


		// stat.close_episode(winner);

		// play.close_episode(winner);
		// env.close_episode(winner);
	}
	return 0;
}
