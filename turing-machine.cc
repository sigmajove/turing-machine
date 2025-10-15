#include <algorithm>
#include <array>
#include <cstddef>
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <optional>
#include <regex>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "analyzer.h"
#include "verifiers.h";

// Returns all possible codes that will distinguish between the two criteria.
std::vector<Triple> Probe(Criterion c1, Criterion c2) {
  std::vector<Triple> result;
  for (int i = 1; i <= 5; ++i) {
    for (int j = 1; j <= 5; ++j) {
      for (int k = 1; k <= 5; ++k) {
        if (c1(i, j, k) != c2(i, j, k)) {
          result.push_back(Triple{i, j, k});
        }
      }
    }
  }
  return result;
}

int main(int argc, char* argv[]) {
  try {
    if (argc != 2) {
      throw std::runtime_error("There should be on argument");
    }

    // Get the input from the file argument.
    std::ifstream file(argv[1]);  // Open the file
    if (!file.is_open()) {        // Check if file opened successfully
      throw std::runtime_error(std::format("Could not open file {}", argv[1]));
    }
    std::stringstream buffer;
    buffer << file.rdbuf();  // Read entire file into buffer
    const std::string file_contents = buffer.str();
    file.close();

    const ParseResult input = Parse(file_contents);

    // The numbers of the cards in the input.
    Analyzer analyzer(input.cards);

    for (auto& [query, result] : input.lines) {
      for (auto& [card, answer] : result) {
        analyzer.Restrict(query, card, answer);
      }
    }
    analyzer.GenerateCombinations(0);

    // Compute max widths
    std::vector<std::size_t> maxw(analyzer.size());
    for (auto& [key, value] : analyzer.result_) {
      for (std::size_t i = 0; i < maxw.size(); ++i) {
        maxw[i] = std::max(maxw[i], value[i].size());
      }
    }

    for (auto& [key, value] : analyzer.result_) {
      for (int i : key) {
        std::cout << i;
      }
      std::cout << " [";
      bool needs_space = false;
      std::size_t index = 0;
      for (const std::set<std::size_t>& xxx : value) {
        if (needs_space) {
          std::cout << " ";
        } else {
          needs_space = true;
        }
        std::cout << std::string(maxw[index] - xxx.size(), ' ');
        for (std::size_t el : xxx) {
          std::cout << el;
        }
        ++index;
      }
      std::cout << "]\n";
    }

    switch (analyzer.result_.size()) {
      case 0:
        std::cout << "No answer!\n";
        return 0;
      case 1:
        std::cout << "Time to guess\n";
        return 0;
    }

#if 0
    std::cout << "\nDistribution\n";
    for (const auto& m : analyzer.distrib_) {
      std::cout << "===============\n";
      for (auto [k, v] : m) {
        std::cout << k << " -> " << v << "\n";
      }
    }
    std::cout << "===============\n";
#endif

    // We next find the next guess to suggest to the player. We would like
    // to get the answer in a few probes as possible. However, getting the
    // very best answer is a hard problem that I declined to solve. Instead
    // I use some heuristics to return a pretty good result.

    std::map<Triple, std::set<std::size_t>> guesses;

    for (std::size_t card = 0; card < analyzer.distrib_.size(); ++card) {
      const auto& m = analyzer.distrib_[card];
      // There needs to be at least two keys to form a query that
      // distinguishes between those two keys.
      if (m.size() <= 1) {
        continue;
      }

      // Iterate over m, finding they keys of the largest and second largest
      // values.
      std::size_t largest_value = 0;
      std::size_t largest_key = 0;
      std::size_t second_largest_value = 0;
      std::size_t second_largest_key = 0;
      for (auto [key, value] : m) {
        if (value == 0) {
          // Because we create the keys on demand when we see them,
          // there should never be a key with a count of zero.
          throw std::runtime_error("count should not be zero");
        }
        // In the following tests, we use >= rather than >, so that
        // if there are only two elements in the map, and they are equal,
        // both will show up as largest and second_largest. We don't
        // care about the order.
        if (value >= largest_value) {
          second_largest_value = largest_value;
          second_largest_key = largest_key;
          largest_value = value;
          largest_key = key;
        } else if (value >= second_largest_value) {
          second_largest_key = key;
          second_largest_value = value;
        }
      }
      if (largest_value == 0 || second_largest_value == 0) {
        throw std::runtime_error("values should not be zero");
      }

      const Verifier& v = analyzer.get_verifier(card);
      for (const Triple& t : Probe(v[largest_key], v[second_largest_key])) {
        guesses.try_emplace(t).first->second.insert(card);
      }
    }

    // Find the maximal guess set.
    std::size_t guess_size = 0;
    for (const auto& [t, s] : guesses) {
      guess_size = std::max(guess_size, s.size());
    }
    // At most three guesses.
    guess_size = std::min(guess_size, static_cast<std::size_t>(3));

    // Find a guess at that big.
    std::cout << "\nSuggested guesses\n";
    for (const auto& [t, s] : guesses) {
      if (s.size() >= guess_size) {
        std::cout << std::format("{}{}{}: ", t[0], t[1], t[2]);

        std::size_t used = 0;
        for (auto q : s) {
          std::cout << static_cast<char>('a' + q);
          if (++used >= guess_size) {
            break;
          }
        }
        std::cout << std::format(" (size: {})", s.size());
        std::cout << "\n";
      }
    }

#if 0
    std::cout << "\nAll of them\n";
    for (const auto& [t, s] : guesses) {
      std::cout << std::format("{}{}{}: ", t[0], t[1], t[2]);
      for (auto i : s) {
        std::cout << i;
      }
      std::cout << "\n";
    }
#endif

  } catch (const std::runtime_error& e) {
    std::cerr << e.what() << "\n";
    return 1;
  }
  return 0;
}
