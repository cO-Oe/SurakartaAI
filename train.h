#pragma once
#include "NN.h"
#include "episode.h"
#include "board.h"
#include <string.h>
#include <vector>
#include <iomanip>
// transform function

class BoardDataSet : public torch::data::Dataset<BoardDataSet> {
    private:
        std::vector<board> states_;
        std::vector<int> labels_;
        // std::vector<PIECE> pieces_;
    public:
        explicit BoardDataSet(const std::vector<board> st, const std::vector<int> label)
        : states_(st),
          labels_(label) {}


        torch::data::Example<> get(size_t index) override {
            board next = states_[index];

            float tensor_stack[ board::SIZE ];
            memset(tensor_stack, 0, sizeof(tensor_stack));
          
			// Convert board value to C-Style array
			for (int i=0; i<board::SIZE; i++) {
                auto p = next(i);
                if (p == SPACE)
                    tensor_stack[i]  = 0;
                else if(p == BLACK)
                    tensor_stack[i]  = 1;
                else if(p == WHITE)
                    tensor_stack[i]  = -1;
            }
            
			// Convert C-array to Tensor
            torch::Tensor state_tensor = torch::from_blob(tensor_stack, {1, 6, 6}).to(device);

			// Convert label to Tensor
            int64_t label = labels_[index];
            torch::Tensor label_tensor = torch::full({1}, label).to(device);
            return {state_tensor, label_tensor};
        };

        torch::optional<size_t> size() const override {
            return states_.size();
        };
};



void train_Net(const episode &game) {

    // Set arguments
	const int num_epoch = 10;
    const int64_t batch_size = 64;
	const double learning_rate = 0.001;

    // Package board and label to train dataset
	auto data_set = BoardDataSet(game.train_boards, game.train_result).map(torch::data::transforms::Stack<>());
	auto set_size = data_set.size().value();

    auto data_loader = torch::data::make_data_loader<torch::data::samplers::RandomSampler>(data_set, torch::data::DataLoaderOptions().batch_size(batch_size));

    // construct optimizer
    torch::optim::Adam optimizer(Net->parameters(), torch::optim::AdamOptions( learning_rate ));
    
    std::cerr << "Start to train Network: \n\n";

    for(int epoch=1; epoch <= num_epoch; epoch++) {
        double mse = 0.0;
        int batch_num = 0;

        for (torch::data::Example<>& batch : *data_loader) {
            auto boards_ = batch.data.to(device);
            auto labels_ = batch.target.squeeze().to(device);  // reduce dim from (1, x) to (x)

			auto output = Net->forward(boards_).to(device);
           	// std::cout << boards_ << '\n';
			// std::cout << "output: " << output << '\n';
			// std::cout << "labels: " << labels_ << '\n';
			auto loss = torch::mse_loss(output, labels_).to(device);

            mse += loss.item<double>() * boards_.size(0);

			// update SGD
            optimizer.zero_grad();
            loss.backward();
            optimizer.step();

            batch_num++;
        }
        // mse /= batch_num;
        mse /= set_size;
		std::cout << "Epoch " << std::setw(2) << epoch << ": " << "Batch Nums = " << std::setw(2) << batch_num << " Mean square error= " << mse << '\n';

    }
}
