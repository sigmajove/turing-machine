#pragma once

#include <utility>
#include <vector>

#include "../verifiers.h"

struct TestGame {
  std::vector<std::pair<int, int>> cards;
  Triple answer;
};

extern std::vector<TestGame> test_games;
