//
// Created by Albert Segarra Roca on 16/12/16.
//

#ifndef SRC_NEURAL_NETWORK_HH
#define SRC_NEURAL_NETWORK_HH

#include <vector>
#include <string>
#include <cmath>
#include <fstream>

#include "utils.hh"
#include "arguments.hh"
using namespace std;

#define hiddenat(layer, neuron) hidden[(layer)*n_hidden_neurons + (neuron)]
#define inputweightat(neuron_from, neuron_to) weights[(neuron_from)*n_hidden_neurons + (neuron_to)]
#define hiddenweightat(layer_to, neuron_from, neuron_to) weights[n_inputs*n_hidden_neurons + ((layer_to) - 1)*n_hidden_neurons*n_hidden_neurons + (neuron_from)*n_hidden_neurons + (neuron_to)]
#define outputweightat(neuron_from, neuron_to) weights[n_hidden_neurons*(n_inputs + (n_hidden_layers - 1)*n_hidden_neurons) + n_outputs*(neuron_from) + (neuron_to)]
#define hiddenbiasat(layer, neuron) bias[(layer)*n_hidden_neurons + (neuron)]
#define outputbiasat(neuron) bias[n_hidden_layers*n_hidden_neurons + (neuron)]
#define hiddendeltaat(layer, neuron) delta_hidden[(layer)*n_hidden_neurons + (neuron)]

class Neural_Network {
private:
    void reserve() {
        hidden = (double*) malloc(sizeof(double)*n_hidden_layers*n_hidden_neurons);
        output = (double*) malloc(sizeof(double)*n_outputs);
        weights = (double*) malloc(sizeof(double)*n_weights);
        bias = (double*) malloc(sizeof(double)*n_bias);
        delta_output = (double*) malloc(sizeof(double)*n_outputs);
        delta_hidden = (double*) malloc(sizeof(double)*n_hidden_layers*n_hidden_neurons);
    }
public:
    bool reserved;

    double* input;
    double* hidden; // Matrix
    double* output;

    double* weights;
    double* bias;

    double* delta_output;
    double* delta_hidden;

    uint n_inputs;
    uint n_hidden_layers;
    uint n_hidden_neurons;
    uint n_outputs;
    uint n_weights;
    uint n_bias;

    double learning_rate;

    ~Neural_Network() {
        if (reserved) {
            free(input);
            free(hidden);
            free(output);
            free(weights);
            free(bias);
            free(delta_output);
            free(delta_hidden);
        }
    }

    Neural_Network() : reserved(false) {}
    Neural_Network(const string& path, double learning_rate) : reserved(true), learning_rate(learning_rate) { load(path); }

    Neural_Network(uint n_inputs, uint n_hidden_layers, uint n_hidden_neurons, uint n_outputs, double learning_rate)
        : n_inputs(n_inputs), n_hidden_layers(n_hidden_layers),
          n_hidden_neurons(n_hidden_neurons), n_outputs(n_outputs),
          n_weights(n_hidden_neurons*(n_inputs + (n_hidden_layers - 1)*n_hidden_neurons + n_outputs)),
          n_bias(n_hidden_neurons*n_hidden_layers + n_outputs),
          learning_rate(learning_rate) {

        reserve();

        uniform_real_distribution<> distribution(Arguments::min_weight_init, Arguments::max_weight_init);
        for (uint i = 0; i < n_weights; ++i)
            weights[i] = distribution(Arguments::random_generator);

        for (uint i = 0; i < n_bias; ++i) bias[i] = distribution(Arguments::random_generator);
    }

    static inline double sigmoid(double x) {
        return (1.0/(1.0 + exp(-x)));
    }

    static inline double derivative_sigmoid(double x) {
        return x*(1.0 - x);
    }

    inline void set_learning_rate(double v) { learning_rate = v; }

    double* recall() {
        memcpy(hidden, bias, sizeof(double)*n_hidden_neurons*n_hidden_layers);
        memcpy(output, bias + n_hidden_neurons*n_hidden_layers, sizeof(double)*n_outputs);

        // Input to 1st hidden layer
        for (uint input_from = 0; input_from < n_inputs; ++input_from) {
            for (uint hidden_to = 0; hidden_to < n_hidden_neurons; ++hidden_to)
                hiddenat(0, hidden_to) += input[input_from]*inputweightat(input_from, hidden_to);
        }

        for (uint neuron = 0; neuron < n_hidden_neurons; ++neuron)
            hiddenat(0, neuron) = sigmoid(hiddenat(0, neuron));

        // layer - 1 to layer
        for (uint layer_to = 1; layer_to < n_hidden_layers; ++layer_to) {
            for (uint neuron_from = 0; neuron_from < n_hidden_neurons; ++neuron_from)
                for (uint neuron_to = 0; neuron_to < n_hidden_neurons; ++neuron_to)
                    hiddenat(layer_to, neuron_to) += hiddenat(layer_to - 1, neuron_from)
                                                     *
                                                     hiddenweightat(layer_to, neuron_from, neuron_to);

            for (uint neuron = 0; neuron < n_hidden_neurons; ++neuron)
                hiddenat(layer_to, neuron) = sigmoid(hiddenat(layer_to, neuron));
        }

        // Last layer to output
        uint last_layer = n_hidden_layers - 1;
        for (uint hidden_from = 0; hidden_from < n_hidden_neurons; ++hidden_from)
            for (uint output_to = 0; output_to < n_outputs; ++output_to)
                output[output_to] += hiddenat(last_layer, hidden_from)*outputweightat(hidden_from, output_to);

        return output;
    }

    double* recall(double* input_values) {
        input = input_values;

        memcpy(hidden, bias, sizeof(double)*n_hidden_neurons*n_hidden_layers);
        memcpy(output, bias + n_hidden_neurons*n_hidden_layers, sizeof(double)*n_outputs);

        // Input to 1st hidden layer
        for (uint input_from = 0; input_from < n_inputs; ++input_from) {
            for (uint hidden_to = 0; hidden_to < n_hidden_neurons; ++hidden_to)
                hiddenat(0, hidden_to) += input[input_from]*inputweightat(input_from, hidden_to);
        }

        for (uint neuron = 0; neuron < n_hidden_neurons; ++neuron)
            hiddenat(0, neuron) = sigmoid(hiddenat(0, neuron));

        // layer - 1 to layer
        for (uint layer_to = 1; layer_to < n_hidden_layers; ++layer_to) {
            for (uint neuron_from = 0; neuron_from < n_hidden_neurons; ++neuron_from)
                for (uint neuron_to = 0; neuron_to < n_hidden_neurons; ++neuron_to)
                    hiddenat(layer_to, neuron_to) += hiddenat(layer_to - 1, neuron_from)
                                                     *
                                                     hiddenweightat(layer_to, neuron_from, neuron_to);

            for (uint neuron = 0; neuron < n_hidden_neurons; ++neuron)
                hiddenat(layer_to, neuron) = sigmoid(hiddenat(layer_to, neuron));
        }

        // Last layer to output
        uint last_layer = n_hidden_layers - 1;
        for (uint hidden_from = 0; hidden_from < n_hidden_neurons; ++hidden_from)
            for (uint output_to = 0; output_to < n_outputs; ++output_to)
                output[output_to] += hiddenat(last_layer, hidden_from)*outputweightat(hidden_from, output_to);

        return output;
    }

    double backpropagate(double* given, double* expected) {
        double mse = 0;

        // Delta for outputs + weight update for outputs
        for (uint neuron_output = 0; neuron_output < n_outputs; ++neuron_output) {
            delta_output[neuron_output] = expected[neuron_output] - given[neuron_output];
            mse += delta_output[neuron_output]*delta_output[neuron_output];

            outputbiasat(neuron_output) += learning_rate*delta_output[neuron_output];

            for (uint neuron_hidden = 0; neuron_hidden < n_hidden_neurons; ++neuron_hidden)
                outputweightat(neuron_hidden, neuron_output) +=
                        learning_rate*delta_output[neuron_output]*hiddenat(n_hidden_layers - 1, neuron_hidden);
        }

        // Delta for last layer + weight update for last layer
        uint last_hidden_layer = n_hidden_layers - 1;
        for (uint hidden_to = 0; hidden_to < n_hidden_neurons; ++hidden_to) {
            double delta = 0;

            for (uint output_from = 0; output_from < n_outputs; ++output_from)
                delta += delta_output[output_from]*
                         outputweightat(hidden_to, output_from)*
                         derivative_sigmoid(hiddenat(last_hidden_layer, hidden_to));

            hiddendeltaat(last_hidden_layer, hidden_to) = delta;
            hiddenbiasat(last_hidden_layer, hidden_to) += learning_rate*delta;

            if (n_hidden_layers > 1)
                for (uint neuron_hidden_previous = 0; neuron_hidden_previous < n_hidden_neurons; ++neuron_hidden_previous)
                    hiddenweightat(last_hidden_layer, neuron_hidden_previous, hidden_to) +=
                        learning_rate*delta*hiddenat(last_hidden_layer - 1, neuron_hidden_previous);
        }

        // Delta for layer layer_to+1 to layer_to + weight update for layer_to
        for (int layer_to = n_hidden_layers - 2; layer_to >= 0; layer_to--) {
            for (uint neuron_to = 0; neuron_to < n_hidden_neurons; ++neuron_to) {
                double delta = 0;

                for (uint neuron_from = 0; neuron_from < n_hidden_neurons; ++neuron_from) {
                    delta += hiddendeltaat(layer_to+1, neuron_from)*
                             hiddenweightat(layer_to+1, neuron_to, neuron_from)*
                             derivative_sigmoid(hiddenat(layer_to, neuron_to));
                }

                hiddendeltaat(layer_to, neuron_to) = delta;
                hiddenbiasat(layer_to, neuron_to) += learning_rate*delta;

                if (layer_to > 0)
                    for (uint neuron_previous = 0; neuron_previous < n_hidden_neurons; ++neuron_previous)
                        hiddenweightat(layer_to, neuron_previous, neuron_to) +=
                            learning_rate*delta*hiddenat(layer_to - 1, neuron_previous);
            }
        }

        for (uint input_from = 0; input_from < n_inputs; ++input_from)
            for (uint hidden_to = 0; hidden_to < n_hidden_neurons; ++hidden_to)
                inputweightat(input_from, hidden_to) += learning_rate*hiddendeltaat(0, hidden_to)*input[input_from];

        return mse/n_outputs;
    }

    double train(double* input_values, double* expected) {
        return backpropagate(recall(input_values), expected);
    }

    void load(string path) {
        ifstream file;
        file.open(path);
        file >> n_inputs >> n_hidden_layers >> n_hidden_neurons >> n_outputs;

        if (not reserved) reserve();

        for (uint i = 0; i < n_bias; ++i)    file >> bias[i];
        for (uint i = 0; i < n_weights; ++i) file >> weights[i];

        file.close();
    }

    void write_file(const string& path) {
        ofstream file;
        file.open(path);
        write(file);
        file.close();
    }

    void write(ostream& o) {
        o << n_inputs << ' ' << n_hidden_layers << ' ' << n_hidden_neurons << ' ' << n_outputs << endl << endl;

        for (uint layer = 0; layer < n_hidden_layers; ++layer) {
            o << hiddenbiasat(layer, 0);
            for (uint neuron = 1; neuron < n_hidden_neurons; ++neuron)
                o << ' ' << hiddenbiasat(layer, neuron);
            o << endl;
        }

        o << outputbiasat(0);
        for (uint i = 1; i < n_outputs; ++i)
            o << ' ' << outputbiasat(i);

        o << endl << endl;

        for (uint input_from = 0; input_from < n_inputs; ++input_from) {
            o << inputweightat(input_from, 0);
            for (uint hidden_to = 1; hidden_to < n_hidden_neurons; ++hidden_to)
                o << ' ' << inputweightat(input_from, hidden_to);
            o << endl;
        }
        o << endl << endl;

        // layer - 1 to layer
        for (uint layer_to = 1; layer_to < n_hidden_layers; ++layer_to) {
            for (uint neuron_from = 0; neuron_from < n_hidden_neurons; ++neuron_from) {
                o << hiddenweightat(layer_to, neuron_from, 0);
                for (uint neuron_to = 1; neuron_to < n_hidden_neurons; ++neuron_to)
                    o << ' ' << hiddenweightat(layer_to, neuron_from, neuron_to);
                o << endl;
            }
            o << endl;
        }
        o << endl;

        for (uint hidden_from = 0; hidden_from < n_hidden_neurons; ++hidden_from) {
            o << outputweightat(hidden_from, 0);
            for (uint hidden_to = 1; hidden_to < n_outputs; ++hidden_to)
                o << ' ' << outputweightat(hidden_from, hidden_to);

            o << endl;
        }
    }

    void write_neurons(ostream& o) {
        o << "Inputs: ";
        for (uint i = 0; i < n_inputs; ++i) o << input[i] << ' ';
        o << endl << endl;
        for (uint i = 0; i < n_hidden_layers; ++i) {
            o << "Hidden layer " << i << ": ";
            for (uint j = 0; j < n_hidden_neurons; ++j) o << hiddenat(i, j) << ' ';
            o << endl;
        }
        o << endl << "Outputs: ";
        for (uint i = 0; i < n_outputs; ++i) o << output[i] << ' ';
        o << endl;
    }
};

#endif //SRC_NEURAL_NETWORK_HH
