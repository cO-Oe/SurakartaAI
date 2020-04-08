#include<torch/torch.h>

torch::nn::Sequential Net(
	// Input Layer { 3 * 6 * 6 } board game size
	
	// torch::nn::Linear(3 * 6 * 6, 64), 
	torch::nn::Conv2d(torch::nn::Conv2dOptions(3, 64, 3).stride(1).padding(1).bias(false)),
	torch::nn::ReLU(),

	// Layer 2
	// torch::nn::Linear(64, 32),
	torch::nn::Conv2d(torch::nn::Conv2dOptions(64, 128, 3).stride(1).padding(1).bias(false)),
	torch::nn::BatchNorm2d(128),
	torch::nn::ReLU(),

	// Layer 3
	// torch::nn::Linear(32, 1),
	torch::nn::Conv2d(torch::nn::Conv2dOptions(256, 1, 3).stride(1).padding(1).bias(false)),
	torch::nn::BatchNorm2d(256),
	torch::nn::Sigmoid()
);


