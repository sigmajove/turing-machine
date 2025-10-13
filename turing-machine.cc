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

#include "verifiers.h";

using Triple = std::array<int, 3>;

// A puzzle where a Criterion has been selected for each Verifier;
using Selected = std::vector<Criterion>;

// Test if a selection of criteria have a solution.
std::optional<Triple> solve(const Selected& selected, bool trace = false) {
  std::optional<Triple> result = std::nullopt;
  std::size_t hits = 0;
  for (int i = 1; i <= 5; ++i) {
    for (int j = 1; j <= 5; ++j) {
      for (int k = 1; k <= 5; ++k) {
        bool missed = false;
        for (Criterion c : selected) {
          if (!c(i, j, k)) {
            missed = true;
            break;
          }
        }
        if (!missed) {
          if (trace) {
            std::cout << std::format(" hit {}{}{}\n", i, j, k);
          }
          // i, j, k accepted by every criterion
          ++hits;
          result = {i, j, k};
        }
      }
    }
  }
  if (hits != 1) {
    return std::nullopt;
  }
  return result;
}

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

class Analyzer {
 public:
  explicit Analyzer(const std::vector<std::size_t>& cards);
  std::size_t size() const { return selected_.size(); }
  const Verifier& get_verifier(std::size_t which) const {
    return cards_[which];
  }

  void Restrict(const Triple& code, std::size_t card, bool success);

  void GenerateCombinations(std::size_t start);
  std::map<Triple, std::vector<std::set<std::size_t>>> result_;

  // For each card, a count of times each number shows up
  // in selection.
  std::vector<std::map<std::size_t, std::size_t>> distrib_;

 private:
  std::vector<Verifier> cards_;
  std::vector<std::size_t> selected_;
};

Analyzer::Analyzer(const std::vector<std::size_t>& cards)
    : selected_(cards.size()), distrib_(cards.size()) {
  for (std::size_t i : cards) {
    // Convert 1-based card numbers to 0-based subscripts.
    const Verifier v = verifiers[i - 1];
    if (v.empty()) {
      throw std::runtime_error(std::format("Card {} is not implemented", i));
    }
    cards_.push_back(v);
  }
}

// Record the output of a query for a particular card.
void Analyzer::Restrict(const Triple& code, std::size_t card, bool success) {
  // Delete from the card every verifier whose output does not
  // match what was observed.
  auto test = [&code, success](Criterion c) {
    return c(code[0], code[1], code[2]) != success;
  };
  auto& vec = cards_[card];
  vec.erase(std::remove_if(vec.begin(), vec.end(), test), vec.end());
}

void Analyzer::GenerateCombinations(std::size_t start) {
  if (start >= selected_.size()) {
    // Construct a vector of criteria
    Selected s(selected_.size());
    for (std::size_t i = 0; i < selected_.size(); ++i) {
      s[i] = (cards_[i])[selected_[i]];
    }
    if (const auto answer = solve(s)) {
      // See if every criterion is necessary.
      bool bad = false;
      for (std::size_t i = 0; i < s.size(); ++i) {
        Selected smaller = s;
        smaller.erase(smaller.begin() + i);
        if (solve(smaller)) {
          bad = true;
          break;
        }
      }
      if (!bad) {
        std::vector<std::set<std::size_t>>& xxx =
            result_.try_emplace(*answer, cards_.size()).first->second;
        for (std::size_t i = 0; i < xxx.size(); ++i) {
          xxx[i].insert(selected_[i]);
        }

        for (std::size_t card = 0; card < selected_.size(); ++card) {
          ++(distrib_[card].try_emplace(selected_[card], 0).first->second);
        }
      }
    }
  } else {
    for (std::size_t i = 0; i < cards_[start].size(); ++i) {
      selected_[start] = i;
      GenerateCombinations(start + 1);
    }
  }
}

struct ReadResult {
  std::vector<std::size_t> cards;
  struct Result {
    Result(int verifier, bool success) : verifier(verifier), success(success) {}
    int verifier;
    bool success;
  };
  struct Line {
    std::array<int, 3> query;
    std::vector<Result> results;
  };
  std::vector<Line> lines;
};

ReadResult ReadFile(const char* filename) {
  ReadResult result;
  std::ifstream file(filename);  // Open the file
  if (!file.is_open()) {         // Check if file opened successfully
    throw std::runtime_error(std::format("Could not open file {}", filename));
  }

  bool first_line = true;
  std::string line;

  while (std::getline(file, line)) {
    // Ignore blank lines
    static const std::regex white_space(R"(\s*)");
    if (std::regex_match(line, white_space)) {
      continue;
    }

    // Ignore comments (beginning with #)
    static const std::regex comment(R"(^\s*#)");
    if (std::regex_search(line, comment)) {
      continue;
    }
    std::stringstream ss(line);
    std::string word;
    if (first_line) {
      first_line = false;
      while (ss >> word) {
        static const std::regex number(R"(\d+)");
        if (!std::regex_match(word, number)) {
          throw std::runtime_error(std::format("{} is not a number", word));
        }
        const std::size_t value = std::stoull(word);
        if (value < 1 || value > 48) {
          throw std::runtime_error(
              std::format("{} is not a card number", value));
        }
        result.cards.push_back(value);
      }
    } else {
      ReadResult::Line& line = result.lines.emplace_back();
      ss >> word;
      static const std::regex digits(R"([1-5]{3})");
      if (!std::regex_match(word, digits)) {
        throw std::runtime_error(std::format("{} is not a valid query", word));
      }
      for (std::size_t i = 0; i < word.size(); ++i) {
        line.query[i] = word[i] - '0';
      }
      while (ss >> word) {
        static const std::regex ans(R"([a-z][-+])");
        if (!std::regex_match(word, ans)) {
          throw std::runtime_error(
              std::format("{} is not a query response", word));
        }
        const int v = word[0] - 'a';
        if (v >= result.cards.size()) {
          throw std::runtime_error(std::format("{} is out of range", word));
        }
        line.results.emplace_back(v, word[1] == '+');
      }
    }
  }

  file.close();
  return result;
}

int main(int argc, char* argv[]) {
  try {
    if (argc != 2) {
      throw std::runtime_error("There should be on argument");
    }
    const ReadResult input = ReadFile(argv[1]);

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
