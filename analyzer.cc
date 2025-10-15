#include "analyzer.h"

#include <array>
#include <cstddef>
#include <format>
#include <iostream>
#include <map>
#include <optional>
#include <regex>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "verifiers.h"

ParseResult Parse(const std::string& input) {
  ParseResult result;
  std::istringstream stream(input);
  bool first_line = true;
  std::string line;
  while (std::getline(stream, line)) {
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
      ParseResult::Line& line = result.lines.emplace_back();
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

  return result;
}

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
    const bool trace = false;
    if (trace) {
      std::cout << "selected";
      for (auto z : selected_) {
        std::cout << " " << z;
      }
      std::cout << "\n";
    }
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
          if (trace) {
            std::cout << i << " is redundant\n";
          }
          bad = true;
          break;
        }
      }
      if (!bad) {
        std::vector<std::set<std::size_t>>& xxx =
            candidates.try_emplace(*answer, cards_.size()).first->second;
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

std::vector<Triple> GetCandidates(const std::string& input) {
  const ParseResult parsed = Parse(input);
  // The numbers of the cards in the input.
  Analyzer analyzer(parsed.cards);

  for (auto& [query, query_result] : parsed.lines) {
    for (auto& [card, answer] : query_result) {
      analyzer.Restrict(query, card, answer);
    }
  }
  analyzer.GenerateCombinations(0);

  std::vector<Triple> result;
  result.reserve(analyzer.candidates.size());
  for (const auto [key, val] : analyzer.candidates) {
    result.push_back(key);
  }
  return result;
}
