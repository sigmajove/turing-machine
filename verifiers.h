#pragma once

#include <array>
#include <functional>
#include <vector>

using Criterion = std::function<bool(int, int, int)>;
using Verifier = std::vector<Criterion>;
using Triple = std::array<int, 3>;

const extern std::array<Verifier, 48> verifiers;
