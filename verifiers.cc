#include "verifiers.h"

#include <array>
#include <cstddef>
#include <format>
#include <stdexcept>

Verifier GetVerifier(std::size_t v) {
  if (v < 1 || v > verifiers.size()) {
    throw std::runtime_error(
        std::format("{} is not a valid verifier number", v));
  }
  // Convert 1-based card numbers to 0-based subscripts.
  const Verifier result = verifiers[v - 1];

  if (result.empty()) {
    throw std::runtime_error(std::format("Card {} is not implemented", v));
  }

  return result;
}

int HowMany(int i, int j, int k, int x) {
  int count = 0;
  if (i == x) {
    ++count;
  }
  if (j == x) {
    ++count;
  }
  if (k == x) {
    ++count;
  }
  return count;
}

int HowManyMod2(int i, int j, int k, int modulus) {
  int count = 0;
  if (i % 2 == modulus) {
    ++count;
  }
  if (j % 2 == modulus) {
    ++count;
  }
  if (k % 2 == modulus) {
    ++count;
  }
  return count;
}

int Multiples(int i, int j, int k) {
  if (i == j && j == k) {
    return 3;
  }
  if (i == j || i == k || j == k) {
    return 2;
  }
  return 0;
}

const std::array<Verifier, 48> verifiers = {
    {// Card 1
     {[](int i, int j, int k) { return i == 1; },
      [](int i, int j, int k) { return i > 1; }},

     // Card 2
     {[](int i, int j, int k) { return i < 3; },
      [](int i, int j, int k) { return i == 3; },
      [](int i, int j, int k) { return i > 3; }},

     // Card 3
     {[](int i, int j, int k) { return j < 3; },
      [](int i, int j, int k) { return j == 3; },
      [](int i, int j, int k) { return j > 3; }},

     // Card 4
     {[](int i, int j, int k) { return j < 4; },
      [](int i, int j, int k) { return j == 4; },
      [](int i, int j, int k) { return j > 4; }},

     // Card 5
     {[](int i, int j, int k) { return i % 2 == 0; },
      [](int i, int j, int k) { return i % 2 == 1; }},

     // Card 6
     {[](int i, int j, int k) { return j % 2 == 0; },
      [](int i, int j, int k) { return j % 2 == 1; }},

     // Card 7
     {[](int i, int j, int k) { return k % 2 == 0; },
      [](int i, int j, int k) { return k % 2 == 1; }},

     // Card 8
     {[](int i, int j, int k) { return HowMany(i, j, k, 1) == 0; },
      [](int i, int j, int k) { return HowMany(i, j, k, 1) == 1; },
      [](int i, int j, int k) { return HowMany(i, j, k, 1) == 2; },
      [](int i, int j, int k) { return HowMany(i, j, k, 1) == 3; }},

     // Card 9
     {[](int i, int j, int k) { return HowMany(i, j, k, 3) == 0; },
      [](int i, int j, int k) { return HowMany(i, j, k, 3) == 1; },
      [](int i, int j, int k) { return HowMany(i, j, k, 3) == 2; },
      [](int i, int j, int k) { return HowMany(i, j, k, 3) == 3; }},

     // Card 10
     {[](int i, int j, int k) { return HowMany(i, j, k, 4) == 0; },
      [](int i, int j, int k) { return HowMany(i, j, k, 4) == 1; },
      [](int i, int j, int k) { return HowMany(i, j, k, 4) == 2; },
      [](int i, int j, int k) { return HowMany(i, j, k, 4) == 3; }},

     // Card 11
     {[](int i, int j, int k) { return i < j; },
      [](int i, int j, int k) { return i == j; },
      [](int i, int j, int k) { return i > j; }},

     // Card 12
     {[](int i, int j, int k) { return i < k; },
      [](int i, int j, int k) { return i == k; },
      [](int i, int j, int k) { return i > k; }},

     // Card 13
     {[](int i, int j, int k) { return j < k; },
      [](int i, int j, int k) { return j == k; },
      [](int i, int j, int k) { return j > k; }},

     // Card 14
     {[](int i, int j, int k) { return i < j && i < k; },
      [](int i, int j, int k) { return j < i && j < k; },
      [](int i, int j, int k) { return k < i && k < j; }},

     // Card 15
     {[](int i, int j, int k) { return i > j && i > k; },
      [](int i, int j, int k) { return j > i && j > k; },
      [](int i, int j, int k) { return k > i && k > j; }},

     // Card 16
     {[](int i, int j, int k) {
        return HowManyMod2(i, j, k, 0) > HowManyMod2(i, j, k, 1);
      },
      [](int i, int j, int k) {
        return HowManyMod2(i, j, k, 0) < HowManyMod2(i, j, k, 1);
      }},

     // Card 17
     {[](int i, int j, int k) { return HowManyMod2(i, j, k, 0) == 0; },
      [](int i, int j, int k) { return HowManyMod2(i, j, k, 0) == 1; },
      [](int i, int j, int k) { return HowManyMod2(i, j, k, 0) == 2; },
      [](int i, int j, int k) { return HowManyMod2(i, j, k, 0) == 3; }},

     // Card 18
     {[](int i, int j, int k) { return (i + j + k) % 2 == 0; },
      [](int i, int j, int k) { return (i + j + k) % 2 == 1; }},

     // Card 19
     {[](int i, int j, int k) { return i + j < 6; },
      [](int i, int j, int k) { return i + j == 6; },
      [](int i, int j, int k) { return i + j > 6; }},

     // Card 20
     {[](int i, int j, int k) { return Multiples(i, j, k) == 3; },
      [](int i, int j, int k) { return Multiples(i, j, k) == 2; },
      [](int i, int j, int k) { return Multiples(i, j, k) == 0; }},

     // Card 21
     {[](int i, int j, int k) { return Multiples(i, j, k) != 2; },
      [](int i, int j, int k) { return Multiples(i, j, k) == 2; }},

     // Card 22
     {[](int i, int j, int k) { return i < j && j < k; },
      [](int i, int j, int k) { return i > j && j > k; },
      [](int i, int j, int k) {
        return !(i < j && j < k) && !(i > j && j > k);
      }},

     // Card 23
     {[](int i, int j, int k) { return i + j + k < 6; },
      [](int i, int j, int k) { return i + j + k == 6; },
      [](int i, int j, int k) { return i + j + k > 6; }},

     // Card 24
     {[](int i, int j, int k) { return i + 1 == j && j + 1 == k; },
      [](int i, int j, int k) { return (i + 1 == j) ^ (j + 1 == k); },
      [](int i, int j, int k) { return i + 1 != j && j + 1 != k; }},

     // Card 25
     {[](int i, int j, int k) { return abs(i - j) != 1 && abs(j - k) != 1; },
      [](int i, int j, int k) {
        // Note: 121 return true, but 123 and 321 return false.
        if (i + 1 == j && j + 1 == k || i - 1 == j && j - 1 == k) {
          return false;
        }
        return (abs(i - j) == 1) || (abs(j - k) == 1);
      },
      [](int i, int j, int k) {
        return i + 1 == j && j + 1 == k || i - 1 == j && j - 1 == k;
      }},

     // Card 26
     {[](int i, int j, int k) { return i < 3; },
      [](int i, int j, int k) { return j < 3; },
      [](int i, int j, int k) { return k < 3; }},

     // Card 27
     {[](int i, int j, int k) { return i < 4; },
      [](int i, int j, int k) { return j < 4; },
      [](int i, int j, int k) { return k < 4; }},

     // Card 28
     {[](int i, int j, int k) { return i == 1; },
      [](int i, int j, int k) { return j == 1; },
      [](int i, int j, int k) { return k == 1; }},

     // Card 29
     {[](int i, int j, int k) { return i == 3; },
      [](int i, int j, int k) { return j == 3; },
      [](int i, int j, int k) { return k == 3; }},

     // Card 30
     {[](int i, int j, int k) { return i == 4; },
      [](int i, int j, int k) { return j == 4; },
      [](int i, int j, int k) { return k == 4; }},

     // Card 31
     {[](int i, int j, int k) { return i > 1; },
      [](int i, int j, int k) { return j > 1; },
      [](int i, int j, int k) { return k > 1; }},

     // Card 32
     {[](int i, int j, int k) { return i > 3; },
      [](int i, int j, int k) { return j > 3; },
      [](int i, int j, int k) { return k > 3; }},

     // Card 33
     {[](int i, int j, int k) { return i % 2 == 0; },
      [](int i, int j, int k) { return j % 2 == 0; },
      [](int i, int j, int k) { return k % 2 == 0; },
      [](int i, int j, int k) { return i % 2 == 1; },
      [](int i, int j, int k) { return j % 2 == 1; },
      [](int i, int j, int k) { return k % 2 == 1; }},

     // Card 34
     {[](int i, int j, int k) { return i <= j && i <= k; },
      [](int i, int j, int k) { return j <= i && j <= k; },
      [](int i, int j, int k) { return k <= i && k <= j; }},

     // Card 35
     {[](int i, int j, int k) { return i >= j && i >= k; },
      [](int i, int j, int k) { return j >= i && j >= k; },
      [](int i, int j, int k) { return k >= i && k >= j; }},

     // Card 36
     {[](int i, int j, int k) { return (i + j + k) % 3 == 0; },
      [](int i, int j, int k) { return (i + j + k) % 4 == 0; },
      [](int i, int j, int k) { return (i + j + k) % 5 == 0; }},

     // Card 37
     {[](int i, int j, int k) { return i + j == 4; },
      [](int i, int j, int k) { return i + k == 4; },
      [](int i, int j, int k) { return j + k == 4; }},

     // Card 38
     {[](int i, int j, int k) { return i + j == 6; },
      [](int i, int j, int k) { return i + k == 6; },
      [](int i, int j, int k) { return j + k == 6; }},

     // Card 39
     {[](int i, int j, int k) { return i == 1; },
      [](int i, int j, int k) { return j == 1; },
      [](int i, int j, int k) { return k == 1; },
      [](int i, int j, int k) { return i > 1; },
      [](int i, int j, int k) { return j > 1; },
      [](int i, int j, int k) { return k > 1; }},

     // Card 40
     {[](int i, int j, int k) { return i < 3; },
      [](int i, int j, int k) { return j < 3; },
      [](int i, int j, int k) { return k < 3; },
      [](int i, int j, int k) { return i == 3; },
      [](int i, int j, int k) { return j == 3; },
      [](int i, int j, int k) { return k == 3; },
      [](int i, int j, int k) { return i > 3; },
      [](int i, int j, int k) { return j > 3; },
      [](int i, int j, int k) { return k > 3; }},

     // Card 41
     {[](int i, int j, int k) { return i < 4; },
      [](int i, int j, int k) { return j < 4; },
      [](int i, int j, int k) { return k < 4; },
      [](int i, int j, int k) { return i == 4; },
      [](int i, int j, int k) { return j == 4; },
      [](int i, int j, int k) { return k == 4; },
      [](int i, int j, int k) { return i > 4; },
      [](int i, int j, int k) { return j > 4; },
      [](int i, int j, int k) { return k > 4; }},

     // Card 42
     {[](int i, int j, int k) { return i < j && i < k; },
      [](int i, int j, int k) { return j < i && j < k; },
      [](int i, int j, int k) { return k < i && k < j; },
      [](int i, int j, int k) { return i > j && i > k; },
      [](int i, int j, int k) { return j > i && j > k; },
      [](int i, int j, int k) { return k > i && k > j; }},

     // Card 43
     {[](int i, int j, int k) { return i < j; },
      [](int i, int j, int k) { return i == j; },
      [](int i, int j, int k) { return i > j; },
      [](int i, int j, int k) { return i < k; },
      [](int i, int j, int k) { return i == k; },
      [](int i, int j, int k) { return i > k; }},

     // Card 44
     {[](int i, int j, int k) { return j < i; },
      [](int i, int j, int k) { return j == i; },
      [](int i, int j, int k) { return j > i; },
      [](int i, int j, int k) { return j < k; },
      [](int i, int j, int k) { return j == k; },
      [](int i, int j, int k) { return j > k; }},

     // Card 45
     {[](int i, int j, int k) { return HowMany(i, j, k, 1) == 0; },
      [](int i, int j, int k) { return HowMany(i, j, k, 1) == 1; },
      [](int i, int j, int k) { return HowMany(i, j, k, 1) == 2; },
      [](int i, int j, int k) { return HowMany(i, j, k, 3) == 0; },
      [](int i, int j, int k) { return HowMany(i, j, k, 3) == 1; },
      [](int i, int j, int k) { return HowMany(i, j, k, 3) == 2; }},

     // Card 46
     {[](int i, int j, int k) { return HowMany(i, j, k, 3) == 0; },
      [](int i, int j, int k) { return HowMany(i, j, k, 3) == 1; },
      [](int i, int j, int k) { return HowMany(i, j, k, 3) == 2; },
      [](int i, int j, int k) { return HowMany(i, j, k, 4) == 0; },
      [](int i, int j, int k) { return HowMany(i, j, k, 4) == 1; },
      [](int i, int j, int k) { return HowMany(i, j, k, 4) == 2; }},

     // Card 47
     {[](int i, int j, int k) { return HowMany(i, j, k, 1) == 0; },
      [](int i, int j, int k) { return HowMany(i, j, k, 1) == 1; },
      [](int i, int j, int k) { return HowMany(i, j, k, 1) == 2; },
      [](int i, int j, int k) { return HowMany(i, j, k, 4) == 0; },
      [](int i, int j, int k) { return HowMany(i, j, k, 4) == 1; },
      [](int i, int j, int k) { return HowMany(i, j, k, 4) == 2; }},

     // Card 48
     {[](int i, int j, int k) { return i < j; },
      [](int i, int j, int k) { return i < k; },
      [](int i, int j, int k) { return j < k; },
      [](int i, int j, int k) { return i == j; },
      [](int i, int j, int k) { return i == k; },
      [](int i, int j, int k) { return j == k; },
      [](int i, int j, int k) { return i > j; },
      [](int i, int j, int k) { return i > k; },
      [](int i, int j, int k) { return j > k; }}}};
