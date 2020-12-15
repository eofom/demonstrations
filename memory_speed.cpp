#include <chrono>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>
#include <stack>
#include <random>

template<typename Func, typename ...Args>
double timeExecution(Func func, Args ...args) {
    auto start = std::chrono::high_resolution_clock::now();
    func(args...);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    return duration.count();
}

struct MinPair {
    double val;
    double min;
};

struct PairMinStack {
    void push(double val) {
        double min = stack.empty() ? val : std::min(val, stack.top().min);
        stack.emplace(val, min);
    }

    MinPair pop() {
        auto res = stack.top();
        stack.pop();
        return res;
    }

    std::stack<MinPair> stack;
};

struct DoubleStackMinStack {
    void push(double val) {
        double min = vals.empty() ? val : std::min(val, mins.top());
        vals.emplace(val);
        mins.emplace(min);
    }

    MinPair pop() {
        MinPair res{vals.top(), mins.top()};
        vals.pop();
        mins.pop();
        return res;
    }

    std::stack<double> vals;
    std::stack<double> mins;
};

template <typename Stack>
void testStack(const std::vector<double> &vals) {
    Stack stack;
    for (double val : vals) {
        stack.push(val);
    }
    double min = std::numeric_limits<double>::min();
    for (auto it = vals.rbegin(); it != vals.rend(); ++it) {
        auto minPair = stack.pop();
        if (minPair.min < min || minPair.val != *it) {
            throw std::logic_error("vals/stack incostistence");
        }
        min = minPair.min;
    }
}

int main() {
    std::mt19937 generator{std::random_device{}()};

    std::vector<double> vals;
    int testSize = 10000000;
    for (int i = 0; i < testSize; ++i) {
        vals.emplace_back(generator());
    }

    auto pairTime = timeExecution(testStack<PairMinStack>, vals);
    auto doubleStackTime = timeExecution(testStack<DoubleStackMinStack>, vals);

    std::cout << "pair time: " << pairTime << "s\n";
    std::cout << "double stack time: " << doubleStackTime << "s\n";
}