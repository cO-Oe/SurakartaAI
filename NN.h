#pragma once
#include <torch/torch.h>
#include "board.h"


torch::Device device (torch::kCPU);

class CNNImpl : public torch::nn::Module {
public:
	CNNImpl() : 

	CNN_Net(
		// Input shape [7 * 6 * 6]
		torch::nn::Conv2d(torch::nn::Conv2dOptions(7, 512, 3).stride(1).padding(1).bias(false)), 
		torch::nn::BatchNorm2d(512),
		torch::nn::ReLU(),
		// shape: [ 512 * 6 * 6 ]
		
		// Layer 2
		torch::nn::Conv2d(torch::nn::Conv2dOptions(512, 512, 3).stride(1).padding(0).bias(false)), 
		torch::nn::BatchNorm2d(512),
		torch::nn::ReLU(),
		// shape: [ 512 * 4 * 4 ]

		// Layer 3
		torch::nn::Conv2d(torch::nn::Conv2dOptions(512, 512, 3).stride(1).padding(0).bias(false)), 
		torch::nn::BatchNorm2d(512),
		torch::nn::ReLU()
		// shape: [ 512 * 2 * 2 ]
	) , 

	FC_Net(
		torch::nn::Linear(512 * 2 * 2, 1024),
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

// Convert Board to C-array
void generate_states(float *board_stacks, const std::vector<board> &next, const bool piece) {
	for(int layer = 0; layer < 2; layer++) {
		for(int st = 0; st < 3; st++) {
			for (int i = 0; i < board::SIZE; i++) {
				auto square = next[st](i);
				if (square == BLACK && layer==0)
					*(board_stacks + ( ( layer*3 + st )*36 + i) ) = 1;
				else if (square == WHITE && layer==1)
					*(board_stacks + ( ( layer*3 + st )*36 + i) ) = 1;
				else
        			*(board_stacks + ( ( layer*3 + st )*36 + i) ) = 0;
			}
		}
	}
	for(int i=0; i < board::SIZE; i++) {
		*(board_stacks + (6*36 + i)) = (piece==BLACK) ? 0 : 1;
	}
}

