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
