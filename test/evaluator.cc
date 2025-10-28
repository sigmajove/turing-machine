#include "evaluator.h"

#include <format>
#include <iostream>
#include <set>
#include <stdexcept>

#include "control_cards.h"

void grid(std::size_t card_no) {
  const std::set<std::size_t> ignore{0, 1, 10, 21, 105, 116, 125, 126, 133};
  auto iter = control_cards[card_no].begin();
  const auto fetch = [card_no, &ignore, &iter] {
    if (ignore.contains((iter - control_cards[card_no].begin()) + 1)) {
      ++iter;
      return '?';
    }
    const bool b = *iter++;
    return b ? '*' : '.';
  };

  for (std::size_t j = 0; j < 2; j++) {
    std::cout << "XX";
    for (size_t i = 0; i < 10; ++i) {
      std::cout << fetch();
    }
    std::cout << "\n";
  }
  for (std::size_t j = 0; j < 8; j++) {
    for (size_t i = 0; i < 12; ++i) {
      std::cout << fetch();
    }
    std::cout << "\n";
  }
  std::cout << "XX";
  for (size_t i = 0; i < 9; ++i) {
    std::cout << fetch();
  }
  std::cout << "X\n";
  std::cout << "XX";
  for (size_t i = 0; i < 8; ++i) {
    std::cout << fetch();
  }
  std::cout << "XX\n";
  std::cout << "================================\n";
}

Evaluator::Evaluator(const std::vector<int>& cards) {
  cards_.reserve(cards.size());
  for (int c : cards) {
    const auto iter = card_labels.find(c);
    if (iter == card_labels.end()) {
      throw std::runtime_error(std::format("Unknown control card {}", c));
    }
#if 0
    grid(iter->second);
#endif
    cards_.push_back(&control_cards[iter->second]);
  }
}

bool Evaluator::Query(const Triple& code, char letter) {
  const auto iter = query_to_hole.find(code);
  if (iter == query_to_hole.end()) {
    throw std::runtime_error(
        std::format("Invaid code {}{}{}", code[0], code[1], code[2]));
  }
  const int i = letter - 'a';
  if (i < 0 || i >= cards_.size()) {
    throw std::runtime_error(std::format("Invaid letter {}", letter));
  }
  return (*cards_[i])[iter->second - 1];
}
