#include <algorithm>
#include <array>
#include <cstddef>
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <optional>
#include <random>
#include <regex>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "analyzer.h"
#include "choose_guess.h"
#include "verifiers.h"

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

    std::cout << "Cards:";
    for (std::size_t c : input.cards) {
      std::cout << " " << c;
    }
    std::cout << "\n";

    // Echo back the input.
    for (auto& [query, result] : input.lines) {
      std::cout << std::format("Code {}{}{}:", query[0], query[1], query[2]);

      for (auto& [card, answer] : result) {
        std::cout << " " << static_cast<char>('a' + card)
                  << (answer ? '+' : '-');
      }
      std::cout << "\n";
    }

    // The numbers of the cards in the input.
    Analyzer analyzer(input.cards);

    for (auto& [query, result] : input.lines) {
      for (auto& [card, answer] : result) {
        analyzer.Restrict(query, card, answer);
      }
    }

    const auto output = analyzer.Run();

    // Use a truly random number.
    std::random_device rd;
    std::mt19937 gen(rd());
    const auto [code, cards] = ChooseGuess(output, analyzer, gen);
#if 0

    switch (output.candidates.size()) {
      case 0:
        std::cout << "No answer meets all the constraints.\n";
        return 0;
      case 1: {
        const Triple answer = output.candidates.begin()->first;
        std::cout << std::format("The answer is {}{}{}\n", answer[0], answer[1],
                                 answer[2]);
        return 0;
      }
    }

    // We next find the next guess to suggest to the player. We would like
    // to get the answer in a few probes as possible. However, getting the
    // very best answer is a hard problem that I declined to solve. Instead
    // I use some heuristics to return a pretty good result.

    std::map<Triple, std::set<std::size_t>> guesses;

    for (std::size_t card = 0; card < output.distribution.size(); ++card) {
      const auto& m = output.distribution[card];
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

    // We take as many guesses as we can per round. Very occasionally,
    // this policy will cause us to take a guess that does not add any
    // information. I suppose we could rerun the analysis after each guess
    // to determine whether the subsequent guesses are adding information.

    // Random number boilerplate
    std::random_device rd;
    std::mt19937 gen(rd());

    const Triple* pick_t = nullptr;
    const std::set<std::size_t>* pick_s = 0;

    // num_seem is the number of candidates we have looked at
    // where s.size() >= guess_size.
    int num_seen = 0;

    for (const auto& [t, s] : guesses) {
      if (num_seen == 0) {
        pick_t = &t;
        pick_s = &s;
        if (s.size() >= guess_size) {
          num_seen = 1;
        }
      } else if ((s.size() >= guess_size)) {
        ++num_seen;
        std::uniform_int_distribution<> dist(1, num_seen);
        if (dist(gen) == 1) {
          // Execute the code with probability 1 in num_seen+1
          pick_t = &t;
          pick_s = &s;
        }
      }
    }

    std::cout << std::format("Guess {}{}{}: ", (*pick_t)[0], (*pick_t)[1],
                             (*pick_t)[2]);

    // Select guess_size values from *pick_s at random such that
    // each member of *pick_s has an equal chance of being selected.
    // Fun fact:  This was one of my interview question for Google.
    std::vector<std::size_t> picked;
    int k = 0;
    for (auto q : *pick_s) {
      // k is the number of values we have looked at.
      ++k;
      // Unconditionally keep values until we have guess_size.
      if (picked.size() < guess_size) {
        picked.push_back(q);
      } else {
        // Replace one of the picked value with probability
        // num_guesses / k. This ensures that each member of (*picked_s)
        // has a probability of guess_size / picked_s->size() of being
        // chosen.
        std::uniform_int_distribution<> dist(1, k);
        if (dist(gen) <= guess_size) {
          std::uniform_int_distribution<> dist2(
              1, static_cast<int>(picked.size()));
          picked[dist2(gen) - 1] = q;
        }
      }
    }
    std::sort(picked.begin(), picked.end());
#endif
    std::cout << std::format("{}{}{} ", code[0], code[1], code[2]);
    for (std::size_t p : cards) {
      std::cout << static_cast<char>('a' + p);
    }
    std::cout << "\n";

  } catch (const std::runtime_error& e) {
    std::cerr << e.what() << "\n";
    return 1;
  }
  return 0;
}
