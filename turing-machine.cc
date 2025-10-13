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

using Criterion = std::function<bool(int, int, int)>;
using Verifier = std::vector<Criterion>;
using Triple = std::array<int, 3>;

// A puzzle where a Criterion has been selected for each Verifier;
using Selected = std::vector<Criterion>;

int HowMany(int i, int j, int k, int x) {
  int count = 0;
  if (i == x) {
    ++count;
  }
  if (j == x) {
    ++count;
  }
  if (k == x) {
    ++count;
  }
  return count;
}

int HowManyMod2(int i, int j, int k, int modulus) {
  int count = 0;
  if (i % 2 == modulus) {
    ++count;
  }
  if (j % 2 == modulus) {
    ++count;
  }
  if (k % 2 == modulus) {
    ++count;
  }
  return count;
}

int Multiples(int i, int j, int k) {
  if (i == j && j == k) {
    return 3;
  }
  if (i == j || i == k || j == k) {
    return 2;
  }
  return 0;
}

const std::array<Verifier, 48> verifiers = {
    {// Card 1
     {[](int i, int j, int k) { return i == 1; },
      [](int i, int j, int k) { return i > 1; }},

     // Card 2
     {[](int i, int j, int k) { return i < 3; },
      [](int i, int j, int k) { return i == 3; },
      [](int i, int j, int k) { return i > 3; }},

     // Card 3
     {[](int i, int j, int k) { return j < 3; },
      [](int i, int j, int k) { return j == 3; },
      [](int i, int j, int k) { return j > 3; }},

     // Card 4
     {[](int i, int j, int k) { return j < 4; },
      [](int i, int j, int k) { return j == 4; },
      [](int i, int j, int k) { return j > 4; }},

     // Card 5
     {[](int i, int j, int k) { return i % 2 == 0; },
      [](int i, int j, int k) { return i % 2 == 1; }},

     // Card 6
     {[](int i, int j, int k) { return j % 2 == 0; },
      [](int i, int j, int k) { return j % 2 == 1; }},

     // Card 7
     {[](int i, int j, int k) { return k % 2 == 0; },
      [](int i, int j, int k) { return k % 2 == 1; }},

     // Card 8
     {[](int i, int j, int k) { return HowMany(i, j, k, 1) == 0; },
      [](int i, int j, int k) { return HowMany(i, j, k, 1) == 1; },
      [](int i, int j, int k) { return HowMany(i, j, k, 1) == 2; },
      [](int i, int j, int k) { return HowMany(i, j, k, 1) == 3; }},

     // Card 9
     {[](int i, int j, int k) { return HowMany(i, j, k, 3) == 0; },
      [](int i, int j, int k) { return HowMany(i, j, k, 3) == 1; },
      [](int i, int j, int k) { return HowMany(i, j, k, 3) == 2; },
      [](int i, int j, int k) { return HowMany(i, j, k, 3) == 3; }},

     // Card 10
     {[](int i, int j, int k) { return HowMany(i, j, k, 4) == 0; },
      [](int i, int j, int k) { return HowMany(i, j, k, 4) == 1; },
      [](int i, int j, int k) { return HowMany(i, j, k, 4) == 2; },
      [](int i, int j, int k) { return HowMany(i, j, k, 4) == 3; }},

     // Card 11
     {[](int i, int j, int k) { return i < j; },
      [](int i, int j, int k) { return i == j; },
      [](int i, int j, int k) { return i > j; }},

     // Card 12
     {[](int i, int j, int k) { return i < k; },
      [](int i, int j, int k) { return i == k; },
      [](int i, int j, int k) { return i > k; }},

     // Card 13
     {[](int i, int j, int k) { return j < k; },
      [](int i, int j, int k) { return j == k; },
      [](int i, int j, int k) { return j > k; }},

     // Card 14
     {[](int i, int j, int k) { return i < j && i < k; },
      [](int i, int j, int k) { return j < i && j < k; },
      [](int i, int j, int k) { return k < i && k < j; }},

     // Card 15
     {[](int i, int j, int k) { return i > j && i > k; },
      [](int i, int j, int k) { return j > i && j > k; },
      [](int i, int j, int k) { return k > i && k > j; }},

     // Card 16
     {[](int i, int j, int k) {
        return HowManyMod2(i, j, k, 0) > HowManyMod2(i, j, k, 1);
      },
      [](int i, int j, int k) {
        return HowManyMod2(i, j, k, 0) < HowManyMod2(i, j, k, 1);
      }},

     // Card 17
     {[](int i, int j, int k) { return HowManyMod2(i, j, k, 0) == 0; },
      [](int i, int j, int k) { return HowManyMod2(i, j, k, 0) == 1; },
      [](int i, int j, int k) { return HowManyMod2(i, j, k, 0) == 2; },
      [](int i, int j, int k) { return HowManyMod2(i, j, k, 0) == 3; }},

     // Card 18
     {[](int i, int j, int k) { return (i + j + k) % 2 == 0; },
      [](int i, int j, int k) { return (i + j + k) % 2 == 1; }},

     // Card 19
     {[](int i, int j, int k) { return i + j < 6; },
      [](int i, int j, int k) { return i + j == 6; },
      [](int i, int j, int k) { return i + j > 6; }},

     // Card 20
     {[](int i, int j, int k) { return Multiples(i, j, k) == 3; },
      [](int i, int j, int k) { return Multiples(i, j, k) == 2; },
      [](int i, int j, int k) { return Multiples(i, j, k) == 0; }},

     // Card 21
     {[](int i, int j, int k) { return Multiples(i, j, k) != 2; },
      [](int i, int j, int k) { return Multiples(i, j, k) == 2; }},

     // Card 22
     {[](int i, int j, int k) { return i < j && j < k; },
      [](int i, int j, int k) { return i > j && j > k; },
      [](int i, int j, int k) {
        return !(i < j && j < k) && !(i > j && j > k);
      }},

     // Card 23
     {[](int i, int j, int k) { return i + j + k < 6; },
      [](int i, int j, int k) { return i + j + k == 6; },
      [](int i, int j, int k) { return i + j + k > 6; }},

     // Card 24
     {[](int i, int j, int k) { return i + 1 == j && j + 1 == k; },
      [](int i, int j, int k) { return (i + 1 == j) ^ (j + 1 == k); },
      [](int i, int j, int k) { return i + 1 != j && j + 1 != k; }},

     // Card 25
     {[](int i, int j, int k) { return abs(i - j) != 1 && abs(j - k) != 1; },
      [](int i, int j, int k) { return (abs(i - j) == 1) ^ (abs(j - k) == 1); },
      [](int i, int j, int k) {
        return i + 1 == j && j + 1 == k || i - 1 == j && j - 1 == k;
      }},

     // Card 26
     {[](int i, int j, int k) { return i < 3; },
      [](int i, int j, int k) { return j < 3; },
      [](int i, int j, int k) { return k < 3; }},

     // Card 27
     {[](int i, int j, int k) { return i < 4; },
      [](int i, int j, int k) { return j < 4; },
      [](int i, int j, int k) { return k < 4; }},

     // Card 28
     {[](int i, int j, int k) { return i == 1; },
      [](int i, int j, int k) { return j == 1; },
      [](int i, int j, int k) { return k == 1; }},

     // Card 29
     {[](int i, int j, int k) { return i == 3; },
      [](int i, int j, int k) { return j == 3; },
      [](int i, int j, int k) { return k == 3; }},

     // Card 30
     {[](int i, int j, int k) { return i == 4; },
      [](int i, int j, int k) { return j == 4; },
      [](int i, int j, int k) { return k == 4; }},

     // Card 31
     {[](int i, int j, int k) { return i > 1; },
      [](int i, int j, int k) { return j > 1; },
      [](int i, int j, int k) { return k > 1; }},

     // Card 32
     {[](int i, int j, int k) { return i > 3; },
      [](int i, int j, int k) { return j > 3; },
      [](int i, int j, int k) { return k > 3; }},

     // Card 33
     {[](int i, int j, int k) { return i % 2 == 0; },
      [](int i, int j, int k) { return j % 2 == 0; },
      [](int i, int j, int k) { return k % 2 == 0; },
      [](int i, int j, int k) { return i % 2 == 1; },
      [](int i, int j, int k) { return j % 2 == 1; },
      [](int i, int j, int k) { return k % 2 == 1; }},

     // Card 34
     {[](int i, int j, int k) { return i <= j && i <= k; },
      [](int i, int j, int k) { return j <= i && j <= k; },
      [](int i, int j, int k) { return k <= i && k <= j; }},

     // Card 35
     {[](int i, int j, int k) { return i >= j && i >= k; },
      [](int i, int j, int k) { return j >= i && j >= k; },
      [](int i, int j, int k) { return k >= i && k >= j; }},

     // Card 36
     {[](int i, int j, int k) { return (i + j + k) % 3 == 0; },
      [](int i, int j, int k) { return (i + j + k) % 4 == 0; },
      [](int i, int j, int k) { return (i + j + k) % 5 == 0; }},

     // Card 37
     {[](int i, int j, int k) { return i + j == 4; },
      [](int i, int j, int k) { return i + k == 4; },
      [](int i, int j, int k) { return j + k == 4; }},

     // Card 38
     {[](int i, int j, int k) { return i + j == 6; },
      [](int i, int j, int k) { return i + k == 6; },
      [](int i, int j, int k) { return j + k == 6; }},

     // Card 39
     {[](int i, int j, int k) { return i == 1; },
      [](int i, int j, int k) { return j == 1; },
      [](int i, int j, int k) { return k == 1; },
      [](int i, int j, int k) { return i > 1; },
      [](int i, int j, int k) { return j > 1; },
      [](int i, int j, int k) { return k > 1; }},

     // Card 40
     {[](int i, int j, int k) { return i < 3; },
      [](int i, int j, int k) { return j < 3; },
      [](int i, int j, int k) { return k < 3; },
      [](int i, int j, int k) { return i == 3; },
      [](int i, int j, int k) { return j == 3; },
      [](int i, int j, int k) { return k == 3; },
      [](int i, int j, int k) { return i > 3; },
      [](int i, int j, int k) { return j > 3; },
      [](int i, int j, int k) { return k > 3; }},

     // Card 41
     {[](int i, int j, int k) { return i < 4; },
      [](int i, int j, int k) { return j < 4; },
      [](int i, int j, int k) { return k < 4; },
      [](int i, int j, int k) { return i == 4; },
      [](int i, int j, int k) { return j == 4; },
      [](int i, int j, int k) { return k == 4; },
      [](int i, int j, int k) { return i > 4; },
      [](int i, int j, int k) { return j > 4; },
      [](int i, int j, int k) { return k > 4; }},

     // Card 42
     {[](int i, int j, int k) { return i < j && i < k; },
      [](int i, int j, int k) { return j < i && j < k; },
      [](int i, int j, int k) { return k < i && k < j; },
      [](int i, int j, int k) { return i > j && i > k; },
      [](int i, int j, int k) { return j > i && j > k; },
      [](int i, int j, int k) { return k > i && k > j; }},

     // Card 43
     {[](int i, int j, int k) { return i < j; },
      [](int i, int j, int k) { return i == j; },
      [](int i, int j, int k) { return i > j; },
      [](int i, int j, int k) { return i < k; },
      [](int i, int j, int k) { return i == k; },
      [](int i, int j, int k) { return i > k; }},

     // Card 44
     {[](int i, int j, int k) { return j < i; },
      [](int i, int j, int k) { return j == i; },
      [](int i, int j, int k) { return j > i; },
      [](int i, int j, int k) { return j < k; },
      [](int i, int j, int k) { return j == k; },
      [](int i, int j, int k) { return j > k; }},

     // Card 45
     {[](int i, int j, int k) { return HowMany(i, j, k, 1) == 0; },
      [](int i, int j, int k) { return HowMany(i, j, k, 1) == 1; },
      [](int i, int j, int k) { return HowMany(i, j, k, 1) == 2; },
      [](int i, int j, int k) { return HowMany(i, j, k, 3) == 0; },
      [](int i, int j, int k) { return HowMany(i, j, k, 3) == 1; },
      [](int i, int j, int k) { return HowMany(i, j, k, 3) == 2; }},

     // Card 46
     {[](int i, int j, int k) { return HowMany(i, j, k, 3) == 0; },
      [](int i, int j, int k) { return HowMany(i, j, k, 3) == 1; },
      [](int i, int j, int k) { return HowMany(i, j, k, 3) == 2; },
      [](int i, int j, int k) { return HowMany(i, j, k, 4) == 0; },
      [](int i, int j, int k) { return HowMany(i, j, k, 4) == 1; },
      [](int i, int j, int k) { return HowMany(i, j, k, 4) == 2; }},

     // Card 47
     {[](int i, int j, int k) { return HowMany(i, j, k, 1) == 0; },
      [](int i, int j, int k) { return HowMany(i, j, k, 1) == 1; },
      [](int i, int j, int k) { return HowMany(i, j, k, 1) == 2; },
      [](int i, int j, int k) { return HowMany(i, j, k, 4) == 0; },
      [](int i, int j, int k) { return HowMany(i, j, k, 4) == 1; },
      [](int i, int j, int k) { return HowMany(i, j, k, 4) == 2; }},

     // Card 48
     {[](int i, int j, int k) { return i < j; },
      [](int i, int j, int k) { return i < k; },
      [](int i, int j, int k) { return j < k; },
      [](int i, int j, int k) { return i == j; },
      [](int i, int j, int k) { return i == k; },
      [](int i, int j, int k) { return j == k; },
      [](int i, int j, int k) { return i > j; },
      [](int i, int j, int k) { return i > k; },
      [](int i, int j, int k) { return j > k; }}}};

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

class Analyzer {
 public:
  explicit Analyzer(const std::vector<std::size_t>& cards);
  std::size_t size() const { return selected_.size(); }

  void Restrict(const Triple& code, std::size_t card, bool success);

  void GenerateCombinations(std::size_t start);
  std::map<Triple, std::vector<std::set<std::size_t>>> result_;

 private:
  std::vector<Verifier> cards_;
  std::vector<std::size_t> selected_;
};

Analyzer::Analyzer(const std::vector<std::size_t>& cards)
    : selected_(cards.size()) {
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
    const bool trace =
        (selected_[0] == 1 && selected_[1] == 2 && selected_[2] == 1 &&
         (selected_[3] == 0 || selected_[3] == 2 || selected_[3] == 4) &&
         (selected_[4] == 6 || selected_[4] == 4 || selected_[4] == 2));
    if (trace) {
      std::cout << "trace";
      for (auto i : selected_) {
        std::cout << " " << i;
      }
      std::cout << "\n";
    }
    // Construct a vector of criteria
    Selected s(selected_.size());
    for (std::size_t i = 0; i < selected_.size(); ++i) {
      s[i] = (cards_[i])[selected_[i]];
    }
    if (const auto answer = solve(s, trace)) {
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
  } catch (const std::runtime_error& e) {
    std::cerr << e.what() << "\n";
    return 1;
  }
  return 0;
}
