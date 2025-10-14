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

std::vector<Triple> Matching(const Verifier& v, std::size_t which) {
  if (which >= v.size()) {
    throw std::runtime_error(
        std::format("which {} out of bounds {}", which, v.size()));
  }
  const Criterion c = v[which];
  std::vector<Triple> result;
  for (int i = 1; i <= 5; ++i) {
    for (int j = 1; j <= 5; ++j) {
      for (int k = 1; k <= 5; ++k) {
        if (c(i, j, k)) {
          const Triple t{i, j, k};
          result.push_back(t);
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

    std::map<Triple, std::set<std::size_t>> guesses;

    for (std::size_t card = 0; card < analyzer.distrib_.size(); ++card) {
      const auto& m = analyzer.distrib_[card];
      if (m.size() <= 1) {
        continue;
      }
#if 0
      std::cout << "Processes distribution for card " << card << "\n";
#endif
      for (const auto& [which, count] : m) {
#if 0
        std::cout << which << " " << count << "\n";
#endif
        auto xxx = Matching(analyzer.get_verifier(card), which);
        for (const Triple& t : xxx) {
          guesses.try_emplace(t).first->second.insert(card);
        }
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
