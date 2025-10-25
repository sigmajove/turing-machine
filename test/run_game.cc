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

  std::vector<Verifier> verifiers;
  verifiers.reserve(test.cards.size());
  std::transform(test.cards.begin(), test.cards.end(),
                 std::back_inserter(verifiers),
                 [](const auto& p) { return GetVerifier(p.first); });
  std::cout << "Criteria cards:";
  for (auto c : test.cards) {
    std::cout << " " << c.first;
  }
  std::cout << "\n";

  Analyzer analyzer(verifiers);

  // A deterministic random number generator for testing.
  std::mt19937 gen(12345);

  std::cout << std::format("Looking for answer {}{}{}\n", test.answer[0],
                           test.answer[1], test.answer[2]);
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

    std::cout << "Candidates\n";
    for (const auto& [t, s] : output.candidates) {
      std::cout << std::format("{}{}{}\n", t[0], t[1], t[2]);
    };

    const auto [code, verifiers] = ChooseGuess(output, analyzer, gen);
    std::cout << std::format("Guess {}{}{}", code[0], code[1], code[2]);
    for (const size_t v : verifiers) {
      std::cout << " " << v;
    }
    std::cout << "\n";

    for (const size_t v : verifiers) {
      const bool answer = evaluator.Query(code, static_cast<char>('a' + v));
      std::cout << " " << answer;
      analyzer.Restrict(code, v, answer);
    }
    std::cout << "\n";
  }
  // We don't seem to making progress.
  return false;
}
