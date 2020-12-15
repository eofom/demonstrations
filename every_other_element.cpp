#include <vector>
#include <cassert>
#include <list>
#include <iostream>
#ifdef __cpp_lib_ranges
#include <ranges>
#endif

template <typename Container>
Container everyOtherByIndex(const Container &input, bool skipFirst) {
    Container result;
    for (int i = skipFirst ? 1 : 0; i < input.size(); i += 2) {
        result.push_back(input.at(i));
    }
    return result;
}


template <typename Container>
Container everyOtherByIterator(const Container &input, bool skipFirst) {
    Container result;
    for (auto it = skipFirst ? std::next(input.begin()) : input.begin();
         it != input.end() && it != std::next(input.end());
         ++(++it)) // it += 2 works only for random acess containers
    {
        result.push_back(*it);
    }
    
    return result;
}

template <typename Container>
Container everyOtherUsingLessIterator(const Container &input, bool skipFirst) { // works only for random access containers
    Container result;
    for (auto it = skipFirst ? std::next(input.begin()) : input.begin();
         it < input.end();
         ++(++it))
    {
        result.push_back(*it);
    }

    return result;
}

template <typename Contaier>
Contaier everyOtherViaRangeBasedLoop(const Contaier &input, bool skipFirst) {
    Contaier result;
    bool skipElement = !skipFirst;
    for (const auto &element : input) {
        skipElement = !skipElement;
        if (skipElement) {
            continue;
        }
        result.push_back(element);
    }
    return result;
}

template <typename Contaier>
Contaier everyOtherViaCopyIf(const Contaier &input, bool skipFirst) {
    Contaier result;
    bool skipElement = !skipFirst;
    auto filterEveryOther = [&skipElement](const auto &elem) {
        skipElement = !skipElement;
        return !skipElement;
    };
    std::copy_if(input.begin(), input.end(), std::back_inserter(result), filterEveryOther);
    return result;
}

#ifdef __cpp_lib_ranges
template <typename Contaier>
Contaier everyOtherViaRanges(const Contaier &input, bool skipFirst) { // C++20
    Contaier result;
    bool skipElement = !skipFirst;
    auto filterEveryOther = [&skipElement](const auto &elem) {
        skipElement = !skipElement;
        return !skipElement;
    };
    for (const auto &element : input | std::views::filter(filterEveryOther)) {
        result.push_back(element);
    }

    return result;
}
#endif

int main() {
    std::vector<int> testVector = {1, 2, 3, 4, 5};
    for (bool skipFirst : {true, false}) {
        auto referenceVector = everyOtherByIndex(testVector, skipFirst);
        assert(referenceVector == everyOtherByIterator(testVector, skipFirst));
        assert(referenceVector == everyOtherUsingLessIterator(testVector, skipFirst));
        assert(referenceVector == everyOtherViaRangeBasedLoop(testVector, skipFirst));
        assert(referenceVector == everyOtherViaCopyIf(testVector, skipFirst));
#ifdef __cpp_lib_ranges
        assert(referenceVector == everyOtherViaRanges(testVector, skipFirst));
#endif
    }

    std::list<int> testList(testVector.begin(), testVector.end());
    for (bool skipFirst : {true, false}) {
        auto referenceVector = everyOtherByIndex(testVector, skipFirst);
        auto referenceList = std::list<int>(referenceVector.begin(), referenceVector.end());
        auto everyOtherInListByIterator = everyOtherByIterator(testList, skipFirst);
        if (skipFirst) {
            assert(referenceList == everyOtherInListByIterator);
        } else {
            std::cout << "why isnt this true: " << std::boolalpha << (referenceList == everyOtherInListByIterator) << "?\n";
        }
//        assert(referenceList == everyOtherUsingLessIterator(testList, skipFirst)); // doesnt work
        assert(referenceList == everyOtherViaRangeBasedLoop(testList, skipFirst));
        assert(referenceList == everyOtherViaCopyIf(testList, skipFirst));
#ifdef __cpp_lib_ranges
        assert(referenceList == everyOtherViaRanges(testList, skipFirst));
#endif
    }

    return 0;
}
