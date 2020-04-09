#pragma once
#include <torch/torch.h>
#include "board.h"

class CNNImpl : public torch::nn::Module {
public:
	CNNImpl() : 

	CNN_Net(
		torch::nn::Conv2d(torch::nn::Conv2dOptions(3, 64, 3).stride(1).padding(1).bias(false)), // shape: [ 64 * 6 * 6 ]
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

		torch::nn::Linear(512, 1)
	) {}

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

// transform function
void generate_states(float *board_stacks, const board &next, const PIECE piece) {
	
	// static float board_stacks[(stacks*2 + 1) * board::SIZE];
	
	int idx = 0;
	for(int i=0; i<36; i++)
		*(board_stacks + (idx++)) = next.black_board()(i);
	for(int i=0; i<36; i++)
		*(board_stacks + (idx++)) = next.white_board()(i);
	for(int i=0; i<36; i++)
		*(board_stacks + (idx++)) = (piece == BLACK) ? 1:0;

	// return board_stacks;
}


