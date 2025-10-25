#include "test_games.h"

// A TestGame consists of N criterion cards (the little rectangluar ones),
// N corresponding verifier cards (the big square ones), and the answer.
// Applies the our algorithm, returning true if it got the right answer.
bool RunGame(const TestGame& test);
