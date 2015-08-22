#include <array>
#include <cmath>
#include <cctype>

#include "snap.h"


enum char_type {
  UPPER_LETTER,
  LOWER_LETTER,
  HYPHEN,
  PUNCTUATION,
  SPACE,
  OTHER,
  NUMBER,
};

std::array<char_type, 128> char_types { OTHER, // NUL 0
    OTHER, // SOH 1
    OTHER, // STX 2
    OTHER, // ETX 3
    OTHER, // EOT 4
    OTHER, // ENQ 5
    OTHER, // ACK 6
    OTHER, // BEL 7
    OTHER, // BS 8
    OTHER, // TAB 9
    OTHER, // LF 10
    OTHER, // VT 11
    OTHER, // FF 12
    OTHER, // CR 13
    OTHER, // SO 14
    OTHER, // SI 15
    OTHER, // DLE 16
    OTHER, // DC1 17
    OTHER, // DC2 18
    OTHER, // DC3 19
    OTHER, // DC4 20
    OTHER, // NAK 21
    OTHER, // SYN 22
    OTHER, // ETB 23
    OTHER, // CAN 24
    OTHER, // EM 25
    OTHER, // SUB 26
    OTHER, // ESC 27
    OTHER, // FS 28
    OTHER, // GS 29
    OTHER, // RS 30
    OTHER, // US 31
    SPACE, // Space 32
    PUNCTUATION, // ! 33
    OTHER, // " 34
    OTHER, // # 35
    OTHER, // $ 36
    OTHER, // % 37
    OTHER, // & 38
    OTHER, // ' 39
    OTHER, // ( 40
    OTHER, // ) 41
    OTHER, // * 42
    OTHER, // + 43
    PUNCTUATION, // , 44
    HYPHEN, // - 45
    PUNCTUATION, // . 46
    OTHER, // / 47
    NUMBER, // 0 48
    NUMBER, // 1 49
    NUMBER, // 2 50
    NUMBER, // 3 51
    NUMBER, // 4 52
    NUMBER, // 5 53
    NUMBER, // 6 54
    NUMBER, // 7 55
    NUMBER, // 8 56
    NUMBER, // 9 57
    OTHER, // : 58
    PUNCTUATION, // ; 59
    OTHER, // < 60
    OTHER, // = 61
    OTHER, // > 62
    PUNCTUATION, // ? 63
    OTHER, // @ 64
    UPPER_LETTER, // A 65
    UPPER_LETTER, // B 66
    UPPER_LETTER, // C 67
    UPPER_LETTER, // D 68
    UPPER_LETTER, // E 69
    UPPER_LETTER, // F 70
    UPPER_LETTER, // G 71
    UPPER_LETTER, // H 72
    UPPER_LETTER, // I 73
    UPPER_LETTER, // J 74
    UPPER_LETTER, // K 75
    UPPER_LETTER, // L 76
    UPPER_LETTER, // M 77
    UPPER_LETTER, // N 78
    UPPER_LETTER, // O 79
    UPPER_LETTER, // P 80
    UPPER_LETTER, // Q 81
    UPPER_LETTER, // R 82
    UPPER_LETTER, // S 83
    UPPER_LETTER, // T 84
    UPPER_LETTER, // U 85
    UPPER_LETTER, // V 86
    UPPER_LETTER, // W 87
    UPPER_LETTER, // X 88
    UPPER_LETTER, // Y 89
    UPPER_LETTER, // Z 90
    OTHER, // [ 91
    OTHER, // \ 92
    OTHER, // ] 93
    OTHER, // ^ 94
    OTHER, // _ 95
    OTHER, // ` 96
    LOWER_LETTER, // a 97
    LOWER_LETTER, // b 98
    LOWER_LETTER, // c 99
    LOWER_LETTER, // d 100
    LOWER_LETTER, // e 101
    LOWER_LETTER, // f 102
    LOWER_LETTER, // g 103
    LOWER_LETTER, // h 104
    LOWER_LETTER, // i 105
    LOWER_LETTER, // j 106
    LOWER_LETTER, // k 107
    LOWER_LETTER, // l 108
    LOWER_LETTER, // m 109
    LOWER_LETTER, // n 110
    LOWER_LETTER, // o 111
    LOWER_LETTER, // p 112
    LOWER_LETTER, // q 113
    LOWER_LETTER, // r 114
    LOWER_LETTER, // s 115
    LOWER_LETTER, // t 116
    LOWER_LETTER, // u 117
    LOWER_LETTER, // v 118
    LOWER_LETTER, // w 119
    LOWER_LETTER, // x 120
    LOWER_LETTER, // y 121
    LOWER_LETTER, // z 122
    OTHER, // { 123
    OTHER, // | 124
    OTHER, // } 125
    OTHER, // ~ 126
    OTHER // DEL 127
};

namespace snap {
  namespace word {
    std::vector<std::vector<std::string>> tokenize(const std::string &text) {
      int N = text.length();
      std::vector<std::vector<std::string>> phrases;
      phrases.emplace_back();
      std::string currentWord;
      for (int i = 0; i < N; ++i) {
        char c = text[i];
        if (char_types[c] == PUNCTUATION || char_types[c] == OTHER || char_types[c] == NUMBER) {
          // word has ended          
          if (currentWord.size() > 2) phrases.back().push_back(currentWord);          
          currentWord.clear();
          if (!phrases.back().empty()) phrases.emplace_back();
        } else if (char_types[c] == UPPER_LETTER || char_types[c] == LOWER_LETTER) {
          if (char_types[c] == UPPER_LETTER) c = tolower(c);
          currentWord += c;
        } else if (char_types[c] == SPACE || char_types[c] == HYPHEN) {
          if (currentWord.size() > 2) phrases.back().push_back(currentWord);
          currentWord.clear();
        }         
      }
      if (currentWord.size() > 2) phrases.back().push_back(currentWord);
      if (phrases.back().empty()) phrases.pop_back();
      return phrases;
    }

    std::map<std::string, int> count_words(const std::vector<std::vector<std::string>> &phrases) {
      std::map<std::string, int> word_counts;
      for (std::vector<std::string> phrase : phrases) {
        for (std::string word : phrase) ++word_counts[word];
      }
      return word_counts;
    }

    std::map<std::string, std::pair<int, int>> compare_word_counts(const std::map<std::string, int> &a, 
                                                                   const std::map<std::string, int> &b,
                                                                   int min_count,
                                                                   double min_percent_increase) {
      std::map<std::string, std::pair<int, int>> top_words;
      for (std::pair<std::string, int> word : b) {
        if (word.second >= min_count) {
          if (a.count(word.first)) {
            double percent_increase = ((double) word.second)/a.at(word.first);
            if (percent_increase >= min_percent_increase) top_words[word.first] = std::make_pair(a.at(word.first), word.second);
          } else {
            top_words[word.first] = std::make_pair(0, word.second);
          }
        }
      }
      return top_words;
    }
  }
}
