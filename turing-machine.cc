#include <algorithm>
#include <array>
#include <cstddef>
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <optional>
#include <random>
#include <regex>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "analyzer.h"
#include "choose_guess.h"
#include "verifiers.h"

int main(int argc, char* argv[]) {
  try {
    if (argc != 2) {
      throw std::runtime_error("There should be on argument");
    }

    // Get the input from the file argument.
    std::ifstream file(argv[1]);  // Open the file
    if (!file.is_open()) {        // Check if file opened successfully
      throw std::runtime_error(std::format("Could not open file {}", argv[1]));
    }
    std::stringstream buffer;
    buffer << file.rdbuf();  // Read entire file into buffer
    const std::string file_contents = buffer.str();
    file.close();

    const ParseResult input = Parse(file_contents);

    std::cout << "Cards:";
    for (std::size_t c : input.cards) {
      std::cout << " " << c;
    }
    std::cout << "\n";

    // Echo back the input.
    for (auto& [query, result] : input.lines) {
      std::cout << std::format("Code {}{}{}:", query[0], query[1], query[2]);

      for (auto& [card, answer] : result) {
        std::cout << " " << static_cast<char>('a' + card)
                  << (answer ? '+' : '-');
      }
      std::cout << "\n";
    }

    // The numbers of the cards in the input.
    Analyzer analyzer(input.cards);

    for (auto& [query, result] : input.lines) {
      for (auto& [card, answer] : result) {
        analyzer.Restrict(query, card, answer);
      }
    }

    const auto output = analyzer.Run();

    switch (output.candidates.size()) {
      case 0:
        std::cout << "No answer meets all the constraints.\n";
        return 0;
      case 1: {
        const Triple answer = output.candidates.begin()->first;
        std::cout << std::format("The answer is {}{}{}\n", answer[0], answer[1],
                                 answer[2]);
        return 0;
      }
    }

    // Use a truly random number.
    std::random_device rd;
    std::mt19937 gen(rd());
    const auto [code, cards] = ChooseGuess(output, analyzer, gen);

    std::cout << std::format("{}{}{} ", code[0], code[1], code[2]);
    for (std::size_t p : cards) {
      std::cout << static_cast<char>('a' + p);
    }
    std::cout << "\n";

  } catch (const std::runtime_error& e) {
    std::cerr << e.what() << "\n";
    return 1;
  }
  return 0;
}
