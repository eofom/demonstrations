#include <chrono>
#include <algorithm>
#include <iostream>
#include <vector>
#include <stack>
#include <random>

#undef NDEBUG
#include <cassert>

constexpr double kPopMarker = 0.9834111245;

template<typename Func, typename ...Args>
double timeExecution(int iterations, Func func, Args ...args) {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        func(args...);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    return duration.count();
}

struct MinPair {
    MinPair(double val, double min) : val(val), min(min) {}
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

    bool empty() const {
        return stack.empty();
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

    bool empty() const {
        return vals.empty();
    }

    std::stack<double> vals;
    std::stack<double> mins;
};

struct OptimizedMinStack {
    void push(double val) {
        double min = vals.empty() ? val : std::min(val, mins.top());
        vals.emplace(val);
        if (min == val) {
            mins.emplace(min);
        }
    }

    MinPair pop() {
        MinPair res{vals.top(), mins.top()};
        if (vals.top() == mins.top()) {
            mins.pop();
        }
        vals.pop();
        return res;
    }

    bool empty() const {
        return vals.empty();
    }

    std::stack<double> vals;
    std::stack<double> mins;
};

template<typename Stack>
void testMinStackAgainstPairMinStack(const std::vector<double> &vals, int seed) {
    std::mt19937 generator(seed);
    Stack testedStack;
    PairMinStack referenceStack;
    int valIndex = 0;
    while (valIndex < vals.size() && !referenceStack.empty()) {
        bool canPop = !referenceStack.empty();
        bool canPush = valIndex < vals.size();
        bool shouldPush =
                !canPop || canPush && static_cast<bool>(std::uniform_int_distribution<int>(0, 1)(generator));
        if (shouldPush) {
            testedStack.push(vals.at(valIndex));
            referenceStack.push(vals.at(valIndex));
            ++valIndex;
        } else {
            auto testedPair = testedStack.pop();
            auto referencePair = referenceStack.pop();
            assert(testedPair.val == referencePair.val);
            assert(testedPair.min == referencePair.min);
        }
    }
}

struct FakeMinStack {
    void push(double val) {
        stack.emplace(val);
    }

    MinPair pop() {
        auto res = stack.top();
        stack.pop();
        return {res, 0.};
    }

    bool empty() const {
        return stack.empty();
    }

    std::stack<double> stack;
};

template <typename Stack>
void testStack(const std::vector<double> &vals) {
    Stack stack;
    for (const auto &val : vals) {
        if (val == kPopMarker) {
            if (!stack.empty()) {
                stack.pop();
            }
        }
        stack.push(val);
    }
}

int main() {
    std::random_device trueRandomGenerator{};
    std::mt19937 generator{trueRandomGenerator()};

    std::vector<double> vals;
    int testSize = 1'000'000;
    double popProbability = 0.5;
    std::discrete_distribution<int> popDistribution({1 - popProbability, popProbability});
    std::uniform_real_distribution<double> pushDistribution(-1., 1.);
    for (int i = 0; i < testSize; ++i) {
        if (popDistribution(generator)) {
            vals.emplace_back(kPopMarker);
        } else {
            vals.emplace_back(pushDistribution(generator));
        }
    }

    testMinStackAgainstPairMinStack<OptimizedMinStack>(vals, trueRandomGenerator());
    testMinStackAgainstPairMinStack<DoubleStackMinStack>(vals, trueRandomGenerator());

    int testIterations = 100;
    std::cout << "PairMinStack time: " << timeExecution(testIterations, testStack<PairMinStack>, vals) << "s\n";
    std::cout << "DoubleStackMinStack time: " << timeExecution(testIterations, testStack<DoubleStackMinStack>, vals) << "s\n";
    std::cout << "OptimizedMinStack time: " << timeExecution(testIterations, testStack<OptimizedMinStack>, vals) << "s\n";
    std::cout << "FakeMinStack time: " << timeExecution(testIterations, testStack<FakeMinStack>, vals) << "s\n";
}

//PairMinStack time: 144.286s
//DoubleStackMinStack time: 187.129s
//OptimizedMinStack time: 99.6452s
//FakeMinStack time: 86.0847s