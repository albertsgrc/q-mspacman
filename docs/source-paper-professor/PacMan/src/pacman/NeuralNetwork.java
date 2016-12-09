/*
 *  Ms. Pac-Man Framework
 *
 *  Created by Luuk Bom & Ruud Henken under supervision by Marco Wiering,
 *  Department of Artificial Intelligence, University of Groningen.
 *
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  NEURALNETWORK
 *  This class is called to hold information on a single neural network,
 *  along with functions for propagating, training, saving and loading the network.
 *
 */

package pacman;

import global.Globals;
import java.io.*;
import java.util.ArrayList;
import java.util.LinkedList;

public class NeuralNetwork {
=
    private double[][] hiddenWeights;
    private double[][] outputWeights;
    private double[] hiddenBias;
    private double[] outputBias;
    private double[] inputActivation;
    private double[] hiddenActivation;
    private double[] outputActivation;
    private int inputs;
    private int hiddens;
    private int outputs;

    public NeuralNetwork(int inputs, int hiddens, int outputs) {
        this.inputs = inputs;
        this.hiddens = hiddens;
        this.outputs = outputs;
        this.hiddenWeights = new double[inputs][hiddens];
        this.outputWeights = new double[hiddens][outputs];
        this.hiddenBias = new double[hiddens];
        this.outputBias = new double[outputs];

        this.trainQueue = new LinkedList();

        double initRange = 0;
        initRange = Math.abs(Globals.randMin) + Math.abs(Globals.randMax);

        //randomize weights
        for (int i = 0; i < inputs; i++) {
            for (int h = 0; h < hiddens; h++) {
                this.hiddenWeights[i][h] = Math.random() * initRange + Globals.randMin; //random tussen -0.3 en 0.3
            }
        }
        for (int h = 0; h < hiddens; h++) {
            for (int o = 0; o < outputs; o++) {
                this.outputWeights[h][o] = Math.random() * initRange + Globals.randMin; //random tussen -0.3 en 0.3
            }
        }
        for (int h = 0; h < hiddens; h++) {
            this.hiddenBias[h] = Math.random() * initRange + Globals.randMin; //random tussen -0.3 en 0.3
        }
        for (int o = 0; o < outputs; o++) {
            this.outputBias[o] = Math.random() * initRange + Globals.randMin; //random tussen -0.3 en 0.3
        }
    }

    public NeuralNetwork(int inputs, int hiddens, int outputs, double[][] hiddenWeights, double[][] outputWeights, double[] hiddenBias, double[] outputBias) {
        this.hiddenWeights = hiddenWeights;
        this.outputWeights = outputWeights;
        this.hiddenBias = hiddenBias;
        this.outputBias = outputBias;
        this.inputs = inputs;
        this.hiddens = hiddens;
        this.outputs = outputs;
    }

    public double[] compute(double[] inputActivation) {
        this.inputActivation = inputActivation;
        this.hiddenActivation = this.computeHiddenUnits(this.inputActivation, this.hiddenWeights, this.hiddens, this.hiddenBias);
        this.outputActivation = this.computeOutputUnits(this.hiddenActivation, this.outputWeights, this.outputs, this.outputBias);
        double[] out = new double[this.outputActivation.length];
        System.arraycopy(this.outputActivation, 0, out, 0, this.outputActivation.length);
        return out;
    }

    private class enqueueItem {
        double[] input;
        double[] desired;
        double learningRate;
    }

    private LinkedList trainQueue;

    public void enqueue(double[] input, double[] desired, double learningRate) {
        enqueueItem newItem = new enqueueItem();
        newItem.input = input;
        newItem.desired = desired;
        newItem.learningRate = learningRate;

        trainQueue.addFirst(newItem);
    }

    public void process_queue(boolean effect) {
        while (!trainQueue.isEmpty()) {
            enqueueItem oldItem = (enqueueItem) trainQueue.pop();
            if (effect == true) {
                this.train(oldItem.input, oldItem.desired, oldItem.learningRate);
            }
        }
    }

    public void train(double[] input, double[] desired, double learningRate) {
        // Compute input, hidden and output based on input
        double[] out = compute(input);

        double[] deltaOutput = new double[this.outputs];
        for (int o = 0; o < this.outputs; o++) {
            // r - y
            deltaOutput[o] = desired[o] - this.outputActivation[o];
        }
        double[] deltaHidden = new double[this.hiddens];
        for (int h = 0; h < this.hiddens; h++) {
            deltaHidden[h] = 0.0;
            for (int o = 0; o < this.outputs; o++) {
                if (deltaOutput[o] != 0 && this.outputWeights[h][o] != 0 && this.hiddenActivation[h] != 0) {
                    // SUM(r - y) * v * z * (1 - z)
                    deltaHidden[h] += deltaOutput[o] * this.outputWeights[h][o] * this.hiddenActivation[h] * (1.0 - this.hiddenActivation[h]);
                }
            }
        }
        for (int h = 0; h < this.hiddens; h++) {
            for (int o = 0; o < this.outputs; o++) {
                if (deltaOutput[o] != 0 && this.hiddenActivation[h] != 0) {
                    // v = v + n * (r - y) * z
                    this.outputWeights[h][o] += learningRate * deltaOutput[o] * this.hiddenActivation[h];
                }
            }
        }
        for (int o = 0; o < this.outputs; o++) {
            if (deltaOutput[o] != 0) {
                this.outputBias[o] += learningRate * deltaOutput[o];
            }
        }
        for (int i = 0; i < this.inputs; i++) {
            for (int h = 0; h < this.hiddens; h++) {
                if (deltaHidden[h] != 0 && this.inputActivation[i] != 0) {
                    // w = w + n * (SUM(r - y) * v * z * (1 - z)) * x
                    this.hiddenWeights[i][h] += learningRate * deltaHidden[h] * this.inputActivation[i];
                }
            }
        }
        for (int h = 0; h < this.hiddens; h++) {
            if (deltaHidden[h] != 0) {
                this.hiddenBias[h] += learningRate * deltaHidden[h];
            }
        }
    }

    private double[] computeHiddenUnits(double[] inputLayer, double[][] inputToHiddenWeights, int numberOfHiddens, double[] biasHidden) {
        double[] bias = biasHidden;
        double[] input = inputLayer;
        double[][] weights = inputToHiddenWeights;
        int hiddens = numberOfHiddens;

        double[] newHidden = new double[hiddens];
        double value;
        for (int h = 0; h < hiddens; h++) {
            value = bias[h];
            for (int i = 0; i < input.length; i++) {
                if (weights[i][h] != 0 && input[i] != 0) {
                    //       SUM(w * x)
                    value += weights[i][h] * input[i];
                }
            }
            if (value == 0) {
                value = 0.5;
            } else {
                // z = sigmoid(SUM(w * x))
                value = 1.0 / (1.0 + Math.pow(Math.E, -value));
            }
            newHidden[h] = value;
        }

        return newHidden;
    }

    private double[] computeOutputUnits(double[] hiddenLayer, double[][] hiddenToOutputWeights, int numberOfOutputs, double[] biasOutput) {
        double[] bias = biasOutput;
        double[] hidden = hiddenLayer;
        double[][] weights = hiddenToOutputWeights;
        int outputs = numberOfOutputs;

        double[] newOutput = new double[outputs];
        double value;
        for (int o = 0; o < outputs; o++) {
            value = bias[o];
            for (int h = 0; h < hidden.length; h++) {
                if (weights[h][o] != 0 && hidden[h] != 0) {
                    // y = SUM(v * z)
                    value += weights[h][o] * hidden[h];
                }
            }
            newOutput[o] = value;
        }
        return newOutput;
    }
}
