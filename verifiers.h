#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <vector>

using Criterion = std::function<bool(int, int, int)>;
using Verifier = std::vector<Criterion>;
using Triple = std::array<int, 3>;

const extern std::array<Verifier, 48> verifiers;

// Maps the (one-based) verifier card number to the implementation of that card.
Verifier GetVerifier(std::size_t v);

void SetTrace(bool v);
