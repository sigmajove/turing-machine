#include "choose_guess.h"

#include <algorithm>
#include <format>
#include <iostream>
#include <map>
#include <random>
#include <set>

#include "analyzer.h"
#include "verifiers.h"

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

std::pair<Triple, std::vector<std::size_t>> ChooseGuess(
    const Analyzer::Output& output, Analyzer& analyzer, std::mt19937& gen) {
  // We next find the next guess to suggest to the player. We would like
  // to get the answer in a few probes as possible. However, getting the
  // very best answer is a hard problem that I declined to solve. Instead
  // I use some heuristics to return a pretty good result.

  std::map<Triple, std::set<std::size_t>> guesses;

  for (std::size_t card = 0; card < output.distribution.size(); ++card) {
    const auto& m = output.distribution[card];
    // There needs to be at least two keys to form a query that
    // distinguishes between those two keys.
    std::cout << std::format("Card {} has size {} \n", card, m.size());
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
    std::cout << std::format("For card {} diff {} and {}\n", card, largest_key,
                             second_largest_key);
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

  const Triple* pick_t = nullptr;
  const std::set<std::size_t>* pick_s = 0;

  // num_seen is the number of candidates we have looked at
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

  if (pick_s == nullptr) {
    throw std::runtime_error("Did not find triple set combo");
  }

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
        std::uniform_int_distribution<> dist2(1,
                                              static_cast<int>(picked.size()));
        picked[dist2(gen) - 1] = q;
      }
    }
  }
  std::sort(picked.begin(), picked.end());
  return std::make_pair(*pick_t, std::move(picked));
}
