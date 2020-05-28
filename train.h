#pragma once
#include "episode.h"
#include <string.h>
#include <vector>
#include <iomanip>

// transform function
class BoardDataSet : public torch::data::Dataset<BoardDataSet> {
    private:
        std::vector<board> states_;
		std::vector<board> states_flip;
        std::vector<PIECE> pieces_;
        std::vector<int> labels_;
		const unsigned stack_size = 3;
    public:
        explicit BoardDataSet(const std::vector<board> st, const std::vector<PIECE> pieces, const std::vector<int> label)
        : states_(st),
		  pieces_(pieces),
          labels_(label) {}


        torch::data::Example<> get(size_t index) override {
            board next = states_[index];
			auto piece = pieces_[index];

			std::vector<board> input_board;
			for(int i = 0; i < stack_size; i++) {
				input_board.push_back( states_[ index + i ] );
				// std::cout << i+1 << ":\n" << input_board[i] << '\n';
			}

            float tensor_stack[ board::SIZE * (stack_size * 2 + 1) ];
            memset(tensor_stack, 0, sizeof(tensor_stack));
			
			// Convert board value to C-Style array
          	generate_states(tensor_stack, input_board, piece);
			
			for(int i=0; i<stack_size; i++) {
				std::cout << "Board: " << i+1 << '\n';
				std::cout << input_board[i];
			}
			
			// Convert C-array to Tensor
            torch::Tensor state_tensor = torch::from_blob(tensor_stack, {7, 6, 6}).to(device);
			std::cout << "Input Tensor: \n" << state_tensor << '\n';
			// Convert label to Tensor
            int64_t label = labels_[index + (stack_size-1)];
			std::cout << "label: " << label << '\n';
            torch::Tensor label_tensor = torch::full({1}, label).to(device);
            return {state_tensor, label_tensor};
        };

        torch::optional<size_t> size() const override {
			assert(states_.size() > (stack_size-1) );
			return states_.size() - (stack_size-1);
        };
};



void train_Net(const episode &game) {

    // Set arguments
	const int num_epoch = 10;
    const int64_t batch_size = 64;
	const double learning_rate = 0.001;

    // Package board and label to train dataset
	auto data_set = BoardDataSet(game.train_boards_, game.train_pieces_, game.train_result).map(torch::data::transforms::Stack<>());
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
