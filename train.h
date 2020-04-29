#pragma once
#include "NN.h"
#include "episode.h"
#include "board.h"
#include <string.h>
#include <vector>

// transform function

class BoardDataSet : public torch::data::Dataset<BoardDataSet> {
    private:
        std::vector<board> states_;
        std::vector<int> labels_;
        std::vector<PIECE> pieces_;
    public:
        explicit BoardDataSet(const std::vector<board> st, const std::vector<int> label)
        : states_(st),
          labels_(label) {}


        torch::data::Example<> get(size_t index) override {
            board next = states_[index];

            // copy board value
            // const int stacks = 3;
            // generate_states(tensor_stack, next);
            float tensor_stack[36];
            memset(tensor_stack, 0, sizeof(tensor_stack));
            for (int i=0; i<board::SIZE; i++) {
                auto p = next(i);
                if (p==SPACE)
                    tensor_stack[i]  = 0;
                else if(p==BLACK)
                    tensor_stack[i]  = 1;
                else if(p==WHITE)
                    tensor_stack[i]  = -1;
            }
            // set state
            torch::Tensor state_tensor = torch::from_blob(tensor_stack, {1, 6, 6}).to(device);
            // std::cerr << "After generated: " << state_tensor << '\n';
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

    // load dataset
    const int64_t batch_size =16;

    // std::cerr << game.train_boards.size() << ' ' << game.train_result.size() << '\n';
    auto data_set = BoardDataSet(game.train_boards, game.train_result).map(torch::data::transforms::Stack<>());

    auto data_loader = torch::data::make_data_loader(data_set, torch::data::DataLoaderOptions().batch_size(batch_size).workers(4));

    // construct optimizer
    torch::optim::Adam optimizer(Net->parameters(), torch::optim::AdamOptions(1e-3));

    std::cerr << "Start to train Network: \n\n";

    for(int epoch=1; epoch <= num_epoch; epoch++) {
        float mse = 0.0;
        int batch_num = 0;

        for (torch::data::Example<>& batch : *data_loader) {
            auto boards_ = batch.data;
            auto labels_ = batch.target.squeeze();  // reduce dim from (1, x) to (x)
            // std::cout << "board: "<< boards_;
            // std::cout << "label: " << labels_;
            boards_ = boards_.to(torch::kF32).to(device);
            labels_ = labels_.to(torch::kF32).to(device);
            // std::cerr << "boards is : " << boards_ << '\n';
            auto output = Net->forward(boards_);
            std::cerr << "label is: " << labels_ << "\n Shape is: " << labels_.sizes() << '\n';
            std::cerr << "output is: " << output << "\n Shape is: " << output.sizes() << '\n';
            auto loss = torch::mse_loss(output, labels_).to(device);
            // std::cerr << "batch " <<  batch_num+1 <<  " loss: " << loss << '\n';
            // std::cerr << "output: " << output << " " << "label: " << labels_ << '\n';

            // update SGD
            optimizer.zero_grad();
            loss.backward();
            optimizer.step();

            mse += loss.template item<float>();
            loss.template item<float>();
            batch_num++;
        }
        mse /= batch_num;
        std::cout << "Epoch " << epoch << ": " << "Mean square error: " << mse << '\n';

    }
}
