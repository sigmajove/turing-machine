#pragma once

#include <cstddef>
#include <random>
#include <utility>
#include <vector>

#include "analyzer.h"
#include "verifiers.h"

// Selects the next guess.
// Returns a triple and the indicies of the verifiers to query.
std::pair<Triple, std::vector<std::size_t>> ChooseGuess(
    const Analyzer::Output& output, Analyzer& analyzer, std::mt19937& gen);
