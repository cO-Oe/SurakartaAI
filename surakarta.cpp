/**
The main program of Surakarta Game
-------------------------
This is the Project of Department of Computer Science in NCTU 
Construced by Clive Wu and Mark Chang
Instructed by Professor I-Chen Wu
**/
#include <iostream>
#include <iterator>
#include <unistd.h>
#include <typeinfo> 
#include <fstream>
#include "board.h"
#include "action.h"
#include "agent.h"
#include "episode.h"
#include "statistic.h"

using namespace std;
int main(int argc, char* argv[]) {
	cout << "Surakarta Demo: ";
	// copy(argv, argv + argc, ostream_iterator<char *>(cout, " "));
	cout << "\n\n";

	size_t total = 1, block = 0;
	for (int i = 1; i < argc; i++) {
		string para(argv[i]);
		if (para.find("--total=") == 0) {
			total = stoull(para.substr(para.find("=") + 1));
		}
		else if (para.find("--block=") == 0) {
			block = stoull(para.substr(para.find("=") + 1));
		}
	}

	statistic stat(total, block);

	player env('B');//2
	player play('W');//1

	while (!stat.is_finished()) {

		board b;
		play.open_episode("~:B");
		env.open_episode("~:W");

		stat.open_episode("W:B");
		episode& game = stat.back(); 

		while ( true ) {
			agent& who = game.take_turns(play, env);

			action a = who.take_action(b);

			game.apply_action(a);

			if ( a.apply(b) == -1 ) {
				cout << b << '\n'; //last board
				break;
			}
		
		//	cout<< b << '\n';
		}

		agent& win = game.last_turns(play, env);
		string winner = (win.get_piece() == 1 ? "play" : "env" );

		cout << "Winner:" << winner << '\n';

		stat.close_episode(winner);

		play.close_episode(winner);
		env.close_episode(winner);
	}
	stat.show();
	return 0;
}