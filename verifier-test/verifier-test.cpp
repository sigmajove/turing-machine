#include <algorithm>
#include <format>
#include <iostream>
#include <random>
#include <vector>

#include "../verifiers.h"

// This is throwaway code to build the verifier tests.

std::vector<Triple> RandTriples(std::mt19937& rand) {
  std::vector<Triple> result;
  result.reserve(125);
  for (int i = 1; i <= 5; ++i) {
    for (int j = 1; j <= 5; ++j) {
      for (int k = 1; k <= 5; ++k) {
        const Triple t{i, j, k};
        result.push_back(t);
      }
    }
  }

  std::shuffle(result.begin(), result.end(), rand);
  return result;
}

int moon() {
  for (std::size_t card = 0; card < verifiers.size(); ++card) {
    bool notable = false;
    const Verifier& v = verifiers[card];
    for (int i = 1; i <= 5; ++i) {
      for (int j = 1; j <= 5; ++j) {
        for (int k = 1; k <= 5; ++k) {
          bool all_miss = true;
          for (Criterion c : v) {
            all_miss = all_miss & !c(i, j, k);
          }
          if (all_miss) {
            notable = true;
          }
        }
      }
    }
    if (notable) {
      std::cout << "Card " << card + 1 << "\n";
    }
  }
  return 0;
}

int main() {
  std::mt19937 rand(12345);
  for (std::size_t card = 14; card <= 48; ++card) {
    if (16 <= card && card <= 25) {
      continue;
    }
    const Verifier& v = verifiers[card - 1];
    std::vector<std::array<std::vector<Triple>, 2>> bucket(v.size());
    for (const Triple& t : RandTriples(rand)) {
      for (std::size_t i = 0; i < v.size(); ++i) {
        std::vector<Triple>& dest = bucket[i][!(v[i](t[0], t[1], t[2]))];
        if (dest.size() < 5) {
          dest.push_back(t);
        }
      }
    }
    std::cout << std::format("\n{{{},{{\n", card);
    for (const auto& b : bucket) {
      std::cout << "{";
      for (const auto& c : b) {
        std::cout << "\n{";
        for (const Triple& t : c) {
          std::cout << std::format("{{{},{},{}}}, ", t[0], t[1], t[2]);
        }
        std::cout << "}";
      }
#if 0
#endif
      std::cout << "},\n";
    }
    std::cout << "}},\n";
  }
  return 0;
}
