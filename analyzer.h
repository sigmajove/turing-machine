#pragma once

#include <array>
#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "verifiers.h"

class Analyzer {
 public:
  using GuessCandidates = std::map<Triple, std::vector<std::set<std::size_t>>>;

  // The output of the analyzer.
  struct Output {
    // All useful codes, and for each code all the useful
    // criterion cards.
    GuessCandidates candidates;

    // For each criteron card, a count of the number of times each test
    // on the card shows up.
    std::vector<std::map<std::size_t, std::size_t>> distribution;

    explicit Output(std::size_t s) : distribution(s) {}

    // Confirm this type has a default move constructor;
    Output(Output&&) = default;
  };

  explicit Analyzer(const std::vector<std::size_t>& criteria_cards);

  std::size_t size() const { return selected_.size(); }

  const Verifier& get_verifier(std::size_t which) const {
    return verifiers_[which];
  }

  Output Run() {
    Output result(verifiers_.size());
    GenerateCombinations(0, result);
    return result;
  }

  void Restrict(const Triple& code, std::size_t card, bool success);

 private:
  void GenerateCombinations(std::size_t start, Output& output);
  void Dump(const GuessCandidates &c);

  std::vector<Verifier> verifiers_;
  std::vector<std::size_t> selected_;
};

struct ParseResult {
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

ParseResult Parse(const std::string& input);

std::vector<Triple> GetCandidates(const std::string& input);
