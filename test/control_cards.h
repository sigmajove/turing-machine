#pragma once
#include <array>

// These are the 95 cards that reveal the answers to queries.
// Each card has 131 check or x boxes. How they are addressed is TBD.
extern std::array<std::array<bool, 131>, 95> control_cards;
