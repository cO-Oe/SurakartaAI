#pragma once
#include <torch/torch.h>
#include "board.h"


torch::Device device (torch::kCPU);

class CNNImpl : public torch::nn::Module {
public:
	CNNImpl() : 

	CNN_Net(
		torch::nn::Conv2d(torch::nn::Conv2dOptions(1, 64, 3).stride(1).padding(1).bias(false)), // shape: [ 64 * 6 * 6 ]
		torch::nn::BatchNorm2d(64),
		torch::nn::ReLU(),

		// Layer 2
		// torch::nn::Linear(64, 32),
		torch::nn::Conv2d(torch::nn::Conv2dOptions(64, 128, 3).stride(1).padding(1).bias(false)), // shape: [ 128 * 6 * 6 ]
		torch::nn::BatchNorm2d(128),
		torch::nn::ReLU(),

		// Layer 3
		// torch::nn::Linear(32, 1),
		torch::nn::Conv2d(torch::nn::Conv2dOptions(128, 256, 3).stride(1).padding(1).bias(false)), // shape: [ 256 * 6 * 6 ]
		torch::nn::BatchNorm2d(256),
		torch::nn::ReLU()
	) , 

	FC_Net(
		torch::nn::Linear(256 * 6 * 6, 1024),
		torch::nn::ReLU(),

		torch::nn::Linear(1024, 512),
		torch::nn::ReLU(),

		torch::nn::Linear(512, 1),
		torch::nn::Tanh()
	) {
		register_module("CNN_Net", CNN_Net);
		register_module("FC_Net", FC_Net);
	}

	torch::Tensor forward(torch::Tensor out)  {
		out = CNN_Net->forward(out); // shape: [ batch_size * 256 * 6 * 6 ]

		out = out.view({ out.sizes()[0] /* batch size*/, -1});
		out = FC_Net->forward(out);

		return out;
	}
	torch::nn::Sequential CNN_Net, FC_Net;

};
// Network MACRO
TORCH_MODULE(CNN);

// Declaration Network
CNN Net;


void generate_states(float *board_stacks, const board &next) {
	
	// static float board_stacks[(stacks*2 + 1) * board::SIZE];
	for (int i=0; i<36; i++) {
		auto p = next(i);
		if (p==SPACE)
        	*(board_stacks + (i) ) = 0;
		else if(p==BLACK)
        	*(board_stacks + (i) ) = 1;

		else if(p==WHITE)
        	*(board_stacks + (i) ) = 2;

	}	


	// int idx = 0;
	// for(int i=0; i<36; i++)
	// 	*(board_stacks + (idx++)) = next.black_board()(i);
	// for(int i=0; i<36; i++)
	// 	*(board_stacks + (idx++)) = next.white_board()(i);
	// for(int i=0; i<36; i++)
	// 	*(board_stacks + (idx++)) = (piece == BLACK) ? 1:0;

}

