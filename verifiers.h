#pragma once

#include <array>
#include <functional>
#include <vector>

using Criterion = std::function<bool(int, int, int)>;
using Verifier = std::vector<Criterion>;

const extern std::array<Verifier, 48> verifiers;
