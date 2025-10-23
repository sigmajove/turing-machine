#pragma once
#include <array>
#include <cstddef>
#include <map>

#include "../verifiers.h"

// These are the 95 cards that reveal the answers to queries.
// Each card has 133 check or x boxes. How they are addressed is TBD.
using ControlCard = std::array<bool, 133>;
extern const std::array<ControlCard, 95> control_cards;

// Maps a three-digit query to a square in the verifier card.
extern const std::map<Triple, std::size_t> query_to_hole;

// Maps the number(s) printed on the control card to the zero-based index of the
// card.
extern const std::map<int, std::size_t> card_labels;
