#ifndef UTILS_HH
#define UTILS_HH

// MOST USED INCLUDES
#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <queue>
#include <deque>
#include <stack>
#include <iostream>
#include <limits>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <cassert>
#include <cmath>
#include <sys/resource.h>
#include "arguments.hh"

using namespace std;

typedef unsigned char uchar;
typedef unsigned int uint;

uniform_real_distribution<> distribution_real(0,1);

// Matrix Class
template <typename T>
class Matrix {
	int r, c;
	vector<vector<T> > t;

public:

	Matrix() : r(0), c(0) {}

	Matrix(int rr, int cc, T init=T()) :
		r(rr), c(cc), t(vector<vector<T> >(r, vector<T>(c, init))) {}

	int rows() const {
		return r;
	}

	int cols() const {
		return c;
	}

	vector<T>& operator[](int i) {
		return t[i];
	}

	const vector<T>& operator[](int i) const {
		return t[i];
	}

	void operator=(const Matrix<T>& m) {
		r = m.r; c = m.c;
		t = vector<vector<T>>(r);
		for (int i = 0; i < r; ++i)
			t[i] = m[i];
	}

	friend ostream& operator<<(ostream& s, const Matrix<T>& m) {
		for (const vector<T> &v : m.t) {
			for (T x : v) s << x << " ";
			s << endl;
		}
		return s;
	}
};

template<typename T> ostream& operator<<(ostream& s, const vector<T> &v) {
	for (int x : v) s << x << " ";
	return s;
}

inline int     readint     () { int    n; cin >> n; return n; }
inline char    readchar	   () {	char   c; cin >> c; return c; }
inline bool    readbool    () { bool   b; cin >> b; return b; }
inline double  readdouble  () { double d; cin >> d; return d; }

const double INFINITE  = numeric_limits<double>::infinity();
const int MAXINT = numeric_limits<int>::max();

inline double randdouble() {
	return distribution_real(Arguments::random_generator);
}

// Returns a random integer in [a, b]
inline int randint(int a, int b) {
	return uniform_int_distribution<>(a, b)(Arguments::random_generator);
}

// Returns a random integer in [0, n-1]
inline int randint(int n) {
	return uniform_int_distribution<>(0, n-1)(Arguments::random_generator);
}

// Returns true with probability p
inline bool randbit(double p) {
	return randdouble() < p;
}

// Returns an nxn matrix with random reals
Matrix<double> randmatrix(int n) {
	Matrix<double> m(n, n);
	for (int i = 0; i < n; ++i)
		for (double& x : m[i]) x = randdouble();
	return m;
}

// Returns a vector of n random reals
vector<double> randvector(int n) {
	vector<double> v(n);
	for (double& x : v) x = randdouble();
	return v;
}

// Returns a vector with n random integers in range [a, b]
vector<int> randvector(int n, int a, int b) {
	vector<int> v(n);
	for (int& x : v) x = randint(a, b);
	return v;
}

inline double clamp(double x, double l, double r) {
	if (x <= l) return 0.0;
	if (x >= r) return 1.0;
	return (x - l)/(r - l);
}

template <typename T>
bool between(T x, T l, T r) {
	return l <= x and x <= r;
}

#define foreach(it, c) for (__typeof((c).begin()) it=(c).begin(); it != (c).end(); ++it)

#ifndef DEBUG
#define DEBUG 0 // set debug mode
#endif

#if DEBUG
#define _debug(...) {\
    char str[200];\
    sprintf(str, __VA_ARGS__);\
    std::cout << "Debug[" << __FILE__ << "][" << __FUNCTION__ << "][Line " << __LINE__ << "] " << str << std::endl;\
}
#else
#define _debug(...)
#endif

#ifndef DEV
#define DEV 1 // set dev mode
#endif

#if DEV
#define ensure(condition, ...) {\
	if (not (condition)) {\
		char str[200];\
    	sprintf(str, __VA_ARGS__);\
    	std::cout << "Error[" << __FILE__ << "][" << __FUNCTION__ << "][Line " << __LINE__ << "] " << str << std::endl;\
		exit(1);\
	}\
}
#else
#define ensure(...)
#endif

#endif
