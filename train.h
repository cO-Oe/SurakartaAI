#pragma once
#include "NN.h"
#include "episode.h"
#include "board.h"

#include <vector>

class BoardDataSet : public torch::data::Dataset<BoardDataSet> {
    private:
        std::vector<board> states_; 
        std::vector<int> labels_;
        std::vector<PIECE> pieces_;
    public:
        explicit BoardDataSet(const std::vector<board> st, const std::vector<PIECE> piece, const std::vector<int> label) 
        : states_(st),
          labels_(label),
          pieces_(piece) {};


        torch::data::Example<> get(size_t index) override {
            board next = states_[index];

            // copy board value
            const int stacks = 3;
            float tensor_stack[stacks * board::SIZE];
            generate_states(tensor_stack, next, pieces_[index]);
            
            // set state
            torch::Tensor state_tensor = torch::from_blob(tensor_stack, {3, 6, 6}).to(device);
            
            // set label
            int64_t label = labels_[index];
            torch::Tensor label_tensor = torch::full({1}, label).to(device);

            return {state_tensor, label_tensor};
        };

        torch::optional<size_t> size() const override {
            return states_.size();
        };
};

void train_Net(const episode &game, const int num_epoch = 10) {
    // std::cout << "Sep1\n";
    // std::cout << Net->parameters() << '\n';

    // load dataset    
    const int64_t batch_size = 16;
    std::vector<int> game_labels(game.ep_boards.size(), game.result);

    auto data_set = BoardDataSet(game.ep_boards, game.ep_pieces, game_labels).map(torch::data::transforms::Stack<>());
    
    auto data_loader = torch::data::make_data_loader(data_set, torch::data::DataLoaderOptions().batch_size(batch_size).workers(2));
    
    // construct optimizer
    torch::optim::Adam optimizer(Net->parameters(), torch::optim::AdamOptions(1e-3));
    
    std::cerr << "Start to train Network: \n\n";
    for(int epoch=1; epoch <= num_epoch; epoch++) {
        float mse = 0.0;

        for (torch::data::Example<>& batch : *data_loader) {
            auto boards_ = batch.data;
            auto labels_ = batch.target.squeeze();  // reduce dim from (1, x) to (x)
            boards_ = boards_.to(torch::kF32).to(device);
            labels_ = labels_.to(torch::kF32).to(device);
            // std::cerr << "boards is : " << boards_ << '\n';
            optimizer.zero_grad();
            auto output = Net->forward(boards_);
	    //optimizer.zero_grad();
            // std::cerr << "label is: " << labels_ << "\n Shape is: " << labels_.sizes() << '\n';
            // std::cerr << "output is: " << output << "\n Shape is: " << output.sizes() << '\n';
            // criterion = torch::nn::MSELoss();

            auto loss = torch::mse_loss(output, labels_).to(device);

            loss.backward();
            optimizer.step();

            mse += loss.template item<float>();
            loss.template item<float>();

        }
        mse /= 16.0;
        std::cout << "Epoch " << epoch << ": " << "Mean square error: " << mse << '\n';

    }
    // std::cout << "Sep2\n";
    // std::cout << Net->parameters() << '\n';
}
