#pragma once
#include <vector>

#include "../verifiers.h"
#include "control_cards.h"

class Evaluator {
 public:
  explicit Evaluator(const std::vector<int>& cards);

  // letter is points to the card codes, starting with 'a'
  bool Query(const Triple& code, char letter);

 private:
  std::vector<const ControlCard*> cards_;
};
