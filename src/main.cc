#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <time.h>
#include <sstream>
#include <iomanip>

#define DEV 1 // set dev mode
#define DEBUG 1 // set debug mode

#include "arguments.hh"
#include "game.hh"
#include "agents/pacman/pathfinding_pacman_agent.hh"
#include "agents/pacman/input_pacman_agent.hh"
#include "agents/pacman/random_pacman_agent.hh"
#include "agents/pacman/rl/rl_pacman_agent.hh"
#include "agents/pacman/rl/nn_pacman_agent.hh"

struct StatisticInfo {
    double won;
    double completion;
    double mse;

    StatisticInfo() : won(0), completion(0.0), mse(0.0) {}
    StatisticInfo(const GameResult& r, double mse) : won((uint)r.won), completion(r.completion), mse(mse) {}
    StatisticInfo(const GameResult& r) : StatisticInfo(r, 0.0) {}

    void operator=(const StatisticInfo& o) {
        won = o.won;
        completion = o.completion;
        mse = o.mse;
    }

    void operator+=(const StatisticInfo& o) {
        won += o.won;
        completion += o.completion;
        mse += o.mse;
    }

    void operator-=(const StatisticInfo& o) {
        won -= o.won;
        completion -= o.completion;
        mse -= o.mse;
    }

    void operator/=(double x) {
        won /= x;
        completion /= x;
        mse /= x;
    }

    StatisticInfo operator/(double x) const {
        StatisticInfo s = *this;
        s /= x;
        return s;
    }
};

struct Statistics {
    uint precision;
    uint observation_count;
    queue<StatisticInfo> Q;
    StatisticInfo totals;
    StatisticInfo totals_always;

    Statistics(uint precision) : precision(precision), observation_count(0) {}

    inline void new_observation(const StatisticInfo& info) {
        totals += info;
        totals_always += info;

        Q.push(info);

        if (observation_count >= precision) {
            StatisticInfo last = Q.front();
            Q.pop();

            totals -= last;
        }

        ++observation_count;
    }

    inline StatisticInfo avg() const {
        return totals/max(1, (int)min(precision, observation_count));
    }

    inline StatisticInfo avg_always() const {
        return totals_always/observation_count;
    }
};

void print_info(const Statistics& s, bool mse) {
    StatisticInfo average_always = s.avg_always();
    StatisticInfo average = s.avg();
    cout << 100*average.won << "% || " << 100*average.completion << "%";
    if (mse) cout << " || " << average.mse;

    cout << " :: [" << int(s.totals_always.won) << "/" << s.observation_count << "] ";

    cout << 100*average_always.won << "% || "
         << 100*average_always.completion << "%";
    if (mse) cout << " || " << average_always.mse;
}

string pad(string s, char c, uint length) {
    string m;
    stringstream ss(m);
    ss << std::setfill(c) << std::setw(length) << s;
    return ss.str();
}

string id() {
    stringstream ss;
    ss << time(0) << pad(to_string(rand()%100), '0', 2);
    return ss.str();
}

void write_json(vector<pair<string, string>> data, ostream& os) {
    os << "{";
    for (uint i = 0; i < data.size(); ++i) {
        if (i > 0) os << ",";

        os << endl
           << "    \"" << data[i].first << "\": " << data[i].second;
    }
    os << endl << "}";
}

int main(int argc, char* argv[]) {
    cout.setf(ios::fixed);
    cout.precision(1);

    Arguments::init(argc, argv);
    Arguments::postprocess();

    srand(Arguments::random_seed);

    Game game;

    game.load_maze();

    Agent* pacman_ai;

    switch (Arguments::pacman_ai_agent) {
        case PATHFINDING: pacman_ai = new Pathfinding_Pacman_Agent(); break;
        case INPUT: pacman_ai = new Input_Pacman_Agent(); break;
        case RANDOM: pacman_ai = new Random_Pacman_Agent(); break;
        case RL: pacman_ai = new RL_Pacman_Agent(); break;
        case NN: pacman_ai = new NN_Pacman_Agent(Arguments::neural_network_path); break;
        default: ensure(false, "Invalid pacman AI Agent enum value");
    }

    game.set_ai(pacman_ai);

    Statistics s_log(Arguments::logging_statistics_precision);
    Statistics s_test(Arguments::test_statistics_precision);

    double max_win_ratio = -1;
    vector<Neural_Network> max_nns(Arguments::test_sampling);
    vector<Neural_Network> last_nns(Arguments::test_sampling);


    bool is_rl = Arguments::pacman_ai_agent == RL;
    cout << "Wins || Completion" << (is_rl ? " || MSE" : "") <<  " (Last " << s_log.precision << " :: Always)" << endl;

    /** TRAINING STAGE **/
    int i_start_evaluation = Arguments::plays*Arguments::nn_evaluation_start;
    for (int i = 0; i < Arguments::plays; ++i) {
        if (is_rl and i%Arguments::test_sampling_interval == 0)
            last_nns[(i%Arguments::test_statistics_precision)/Arguments::test_sampling_interval].from_weights(((RL_Pacman_Agent *) (pacman_ai))->nn);

        game.play();

        double mse = is_rl ? ((RL_Pacman_Agent*)(pacman_ai))->mse_sum_last/game.state.round : 0;
        StatisticInfo info(game.result, mse);
        s_log.new_observation(info);
        s_test.new_observation(info);

        double win_ratio_test;
        if (i >= i_start_evaluation and is_rl) {
            if (Arguments::nn_evaluation_attribute == WINS) win_ratio_test = s_test.avg().won;
            else win_ratio_test = s_test.avg().completion;

            if (win_ratio_test > max_win_ratio) {
                max_win_ratio = win_ratio_test;
                for (uint j = 0; j < max_nns.size(); ++j) max_nns[j].from_weights(last_nns[j]);
            }
        }

        if (i%Arguments::logging_update_rate == Arguments::logging_update_rate - 1) {
            cout << "\r";
            print_info(s_log, is_rl);
            if (is_rl and i >= i_start_evaluation) cout << " ** Max wins: " << 100*max_win_ratio;
            cout << "  ";
            cout.flush();
        }

        game.reset();
    }

    string nn_id;

    double maxtestcompletion;
    double maxtestwins;
    /** TESTING STAGE **/
    if (is_rl and max_nns[0].reserved) {
        int maxtest = -1;
        double maxtestevaluation = -1;

        for (uint j = 0; j < max_nns.size(); ++j) {
            cout << endl << "Testing best agent: Wins || Completion ("
                 << Arguments::test_statistics_precision - j*Arguments::test_sampling_interval << " before)" << endl;

            Game game_test;
            game_test.load_maze();
            pacman_ai = new NN_Pacman_Agent(max_nns[j]);
            game_test.set_ai(pacman_ai);

            Statistics s_log_test(Arguments::logging_statistics_precision);
            StatisticInfo avg;

            for (int i = 0; i < Arguments::n_games_test; ++i) {
                game_test.play();

                s_log_test.new_observation(StatisticInfo(game_test.result));

                if (i%Arguments::logging_update_rate == Arguments::logging_update_rate - 1) {
                    avg = s_log_test.avg_always();
                    cout << "\r[" << int(s_log_test.totals_always.won) << "/" << s_log_test.observation_count << "] "
                         << 100*avg.won << "% || " << 100*avg.completion << "%";

                    cout.flush();
                }

                game_test.reset();
            }

            if (Arguments::nn_evaluation_attribute == WINS and avg.won > maxtestevaluation) {
                maxtestevaluation = avg.won;
                maxtestwins = avg.won;
                maxtestcompletion = avg.completion;
                maxtest = j;

            }
            else if (Arguments::nn_evaluation_attribute == COMPLETION and avg.completion > maxtestevaluation) {
                maxtestevaluation = avg.completion;
                maxtestwins = avg.won;
                maxtestcompletion = avg.completion;
                maxtest = j;
            }
        }

        nn_id = "nn" + id() + "-" + pad(to_string(int(1000*maxtestevaluation)), '0', 3);

        system("mkdir -p ../data/neural-networks");
        string nn_path = "../data/neural-networks/" + nn_id + ".txt";
        max_nns[maxtest].write_file(nn_path);
        cout << endl << "Written best agent's neural network to " << nn_path;
    }

    if (Arguments::non_interactive) {
        vector<pair<string, string>> arguments = Arguments::create_json();

        string arguments_json;
        stringstream ss(arguments_json);
        write_json(arguments, ss);

        system("mkdir -p ../data/statistics");

        string statistics_path;
        if (is_rl)
            statistics_path = "../data/statistics/" + nn_id.substr(2) + ".json";
        else
            statistics_path = "../data/statistics/" + id() + "-test.json";

        ofstream file;
        file.open(statistics_path);

        vector<pair<string, string>> json = {
                { "arguments", ss.str() },
                { "wins", to_string(maxtestwins) },
                { "completion", to_string(maxtestcompletion) },
                { "mse", to_string(s_log.avg().mse) },
                { "neural_network", (is_rl ? ('"' + nn_id + '"') : "null") },
                { "timestamp", to_string(time(0)) }
        };

        write_json(json, file);

        file.close();

        cout << endl << "Written statistics information to " + statistics_path;

        write_json(json, cerr);
    }

    cout << endl;
}