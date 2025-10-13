#include <algorithm>
#include <format>
#include <iostream>
#include <random>
#include <vector>

#include "../verifiers.h"

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
  std::cout << "Good:";
  for (std::size_t card = 0; card < verifiers.size(); ++card) {
    const Verifier& v = verifiers[card];
    bool all_good = true;
    for (int i = 1; i <= 5; ++i) {
      for (int j = 1; j <= 5; ++j) {
        for (int k = 1; k <= 5; ++k) {
          std::size_t success = 0;
          for (Criterion c : v) {
            if (c(i, j, k)) {
              success += 1;
            }
          }
          if (success != 1) {
            all_good = false;
          }
        }
      }
    }
    if (all_good) {
      std::cout << " " << card + 1;
    }
  }
  std::cout << "\n";
  return 0;
}

int main() {
  std::mt19937 rand(12345);
  for (std::size_t card = 1; card <= 25; ++card) {
    if (card == 14 || card == 15) {
      continue;
    }
    const Verifier& v = verifiers[card - 1];
    std::cout << std::format("\n{{{},{{\n", card);
    for (std::size_t i = 0; i < v.size(); ++i) {
      std::cout << "{";
      std::size_t count = 0;
      for (const Triple& t : RandTriples(rand)) {
        if (v[i](t[0], t[1], t[2])) {
          if (count != 0) {
            std::cout << ", ";
          }
          std::cout << std::format("{{{},{},{}}}", t[0], t[1], t[2]);
          if (++count >= 5) break;
        }
      }
      std::cout << "},\n";
    }
    std::cout << "}},\n";
  }
  return 0;
}
