#include "run_game.h"

#include <algorithm>
#include <cstddef>
#include <format>
#include <iostream>
#include <iterator>
#include <random>
#include <vector>

#include "..\analyzer.h"
#include "..\choose_guess.h"
#include "evaluator.h"
#include "test_games.h"

bool RunGame(const TestGame& test) {
  // Output vector of first elements
  std::vector<int> criteria;
  criteria.reserve(test.cards.size());
  std::transform(test.cards.begin(), test.cards.end(),
                 std::back_inserter(criteria),
                 [](const auto& p) { return p.second; });
  Evaluator evaluator(criteria);

  std::vector<std::size_t> criteria_cards;
  criteria_cards.reserve(test.cards.size());
  std::transform(test.cards.begin(), test.cards.end(),
                 std::back_inserter(criteria_cards),
                 [](const std::pair<int, int>& p) {
                   return static_cast<std::size_t>(p.first);
                 });

  Analyzer analyzer(criteria_cards);

  // A deterministic random number generator for testing.
  std::mt19937 gen(12345);

  for (size_t count = 0; count < 100; ++count) {
    const auto output = analyzer.Run();
    switch (output.candidates.size()) {
      case 0:
        // No answer meets all the constraints.
        return false;
      case 1: {
        // We have an answer. See if it is correct.
        const Triple answer = output.candidates.begin()->first;
        return answer == test.answer;
      }
    }

    const auto [code, verifiers] = ChooseGuess(output, analyzer, gen);
    for (const size_t v : verifiers) {
      const bool answer = evaluator.Query(code, static_cast<char>('a' + v));
      analyzer.Restrict(code, v, answer);
    }
  }
  // We don't seem to making progress.
  return false;
}
