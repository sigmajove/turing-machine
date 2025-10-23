#include <algorithm>
#include <cstddef>
#include <format>
#include <fstream>
#include <iostream>
#include <random>
#include <regex>
#include <set>
#include <sstream>
#include <vector>

#include "../test/control_cards.h"
#include "../test/evaluator.h"
#include "../verifiers.h"

// This is throwaway code to build the verifier tests.

std::vector<Triple> RandTriples(std::mt19937& rand) {
  std::vector<Triple> result;
  result.reserve(125);
  for (int i = 1; i <= 5; ++i) {
    for (int j = 1; j <= 5; ++j) {
      for (int k = 1; k <= 5; ++k) {
        const Triple t{i, j, k};
        result.push_back(t);
      }
    }
  }

  std::shuffle(result.begin(), result.end(), rand);
  return result;
}

int moon() {
  for (std::size_t card = 0; card < verifiers.size(); ++card) {
    bool notable = false;
    const Verifier& v = verifiers[card];
    for (int i = 1; i <= 5; ++i) {
      for (int j = 1; j <= 5; ++j) {
        for (int k = 1; k <= 5; ++k) {
          bool all_miss = true;
          for (Criterion c : v) {
            all_miss = all_miss & !c(i, j, k);
          }
          if (all_miss) {
            notable = true;
          }
        }
      }
    }
    if (notable) {
      std::cout << "Card " << card + 1 << "\n";
    }
  }
  return 0;
}

int main2() {
  std::mt19937 rand(12345);
  for (std::size_t card = 14; card <= 48; ++card) {
    if (16 <= card && card <= 25) {
      continue;
    }
    const Verifier& v = verifiers[card - 1];
    std::vector<std::array<std::vector<Triple>, 2>> bucket(v.size());
    for (const Triple& t : RandTriples(rand)) {
      for (std::size_t i = 0; i < v.size(); ++i) {
        std::vector<Triple>& dest = bucket[i][!(v[i](t[0], t[1], t[2]))];
        if (dest.size() < 5) {
          dest.push_back(t);
        }
      }
    }
    std::cout << std::format("\n{{{},{{\n", card);
    for (const auto& b : bucket) {
      std::cout << "{";
      for (const auto& c : b) {
        std::cout << "\n{";
        for (const Triple& t : c) {
          std::cout << std::format("{{{},{},{}}}, ", t[0], t[1], t[2]);
        }
        std::cout << "}";
      }
#if 0
#endif
      std::cout << "},\n";
    }
    std::cout << "}},\n";
  }
  return 0;
}

#if 0
int grid(std::size_t card_no) {
  const std::set<std::size_t> ignore{0, 1, 10, 21, 105, 116, 125, 126, 133};
  auto iter = control_cards[card_no].begin();
  const auto fetch = [card_no, &ignore, &iter] {
    if (ignore.contains((iter - control_cards[card_no].begin()) + 1)) {
      ++iter;
      return '?';
    }
    const bool b = *iter++;
    return b ? '*' : '.';
  };

  for (std::size_t j = 0; j < 2; j++) {
    std::cout << "XX";
    for (size_t i = 0; i < 10; ++i) {
      std::cout << fetch();
    }
    std::cout << "\n";
  }
  for (std::size_t j = 0; j < 8; j++) {
    for (size_t i = 0; i < 12; ++i) {
      std::cout << fetch();
    }
    std::cout << "\n";
  }
  std::cout << "XX";
  for (size_t i = 0; i < 9; ++i) {
    std::cout << fetch();
  }
  std::cout << "X\n";
  std::cout << "XX";
  for (size_t i = 0; i < 8; ++i) {
    std::cout << fetch();
  }
  std::cout << "XX\n";
  return 0;
}
#endif

int main(int argc, char* argv[]) {
#if 0
  grid(51);
  return 0;
#endif
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

    std::istringstream stream(file_contents);
    std::string line1;
    if (!std::getline(stream, line1)) {
      throw std::runtime_error(std::format("First line missing"));
    }
    std::string line2;
    if (!std::getline(stream, line2)) {
      throw std::runtime_error(std::format("Second line missing"));
    }
    std::string line3;
    if (std::getline(stream, line3)) {
      throw std::runtime_error(std::format("More then two lines"));
    }

    // Read line1. It should be a list of card ids.
    std::istringstream iss(line1);
    std::vector<int> words;
    std::string word;

    while (iss >> word) {
      if (!std::regex_match(word, std::basic_regex("\\d+"))) {
        throw std::runtime_error(std::format("{} not a number", word));
      }
      const int label = std::stoi(word);
      if (card_labels.find(label) == card_labels.end()) {
        throw std::runtime_error(std::format("{} not a card id", label));
      }
      words.push_back(std::stoi(word));
    }
    Evaluator eval(words);

    // Read line2. It should be a list of card ids.
    std::istringstream iss2(line2);
    if (!(iss2 >> word)) {
      throw std::runtime_error("nothing on line 2");
    }
    if (!std::regex_match(word, std::basic_regex("[1-5][1-5][1-5]"))) {
      throw std::runtime_error(std::format("{} is not a valid code", word));
    }
    const Triple code{word[0] - '0', word[1] - '0', word[2] - '0'};
    if (!(iss2 >> word)) {
      throw std::runtime_error("missing letters");
    }
    for (const char c : word) {
      const bool answer = eval.Query(code, c);
      std::cout << std::format("Code {}{}{} letter {} returns {}\n", code[0],
                               code[1], code[2], c, answer);
    }
  } catch (const std::runtime_error& e) {
    std::cerr << e.what() << "\n";
    return 1;
  }
  return 0;
}
