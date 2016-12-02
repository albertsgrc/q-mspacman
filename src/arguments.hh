#ifndef ARGUMENTS_HH
#define ARGUMENTS_HH

#include <string>
#include "utils.hh"
using namespace std;

const string LAYOUT_FOLDER = "./layouts/";

const string DFL_LAYOUT_PATH = LAYOUT_FOLDER + "originalClassic.lay";

/*
    Steps to add a new argument with name "argument"

    1. Add default value conatant: const string DFL_ARGUMENT = <default value>
    2. Declare member: static <type> argument; inside Arguments class
    3. Implement member: <type> argument; after Arguments class declaration
    4. Assign default value inside init function: Arguments::argument = DFL_ARGUMENT;
    5. Detect and parse argument inside assign_argument function: if (key == "argument") argument = <parse>(value);
*/


class Arguments {
public:

    static string layout_path;

    static void init(int argc, char* argv[]);

    static void parse_key_value(const string& arg, string& key, string& value);

    static void assign_argument(const string& key, const string& value);

    static void treat_arg(const string& arg);
};

string Arguments::layout_path;

void Arguments::init(int argc, char* argv[]) {
    Arguments::layout_path = DFL_LAYOUT_PATH;

    for (int i = 1; i < argc; ++i) treat_arg(argv[i]);
}

void Arguments::assign_argument(const string& key, const string& value) {
    if (key == "layout") Arguments::layout_path = LAYOUT_FOLDER + value;
    else error("Invalid argument name '" + key + "'");
}

void Arguments::treat_arg(const string& arg) {
    string key, value;
    parse_key_value(arg, key, value);
    assign_argument(key, value);
}

void Arguments::parse_key_value(const string& arg, string& key, string& value) {
    uint i = 0;

    while (i < arg.length() and arg[i] != '=')
        key.push_back(arg[i++]);

    ++i;

    if (i >= arg.length())
        error("Invalid parameter \'" + arg + "\': Should be of the form key=value");

    while (i < arg.length())
        value.push_back(arg[i++]);
}

#endif
