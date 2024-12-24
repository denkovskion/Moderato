/*
 * MIT License
 *
 * Copyright (c) 2024 Ivan Denkovski
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <algorithm>
#include <cctype>
#include <numeric>
#include <regex>
#include <sstream>
#include <stdexcept>

#include "FairyConditions.h"
#include "FairyPieces.h"
#include "OrthodoxPieces.h"
#include "ProblemTypes.h"

namespace moderato {

namespace popeye {

enum File { _a, _b, _c, _d, _e, _f, _g, _h };
enum Rank { _1, _2, _3, _4, _5, _6, _7, _8 };
struct Square {
  File file;
  Rank rank;
};
enum PieceType {
  King,
  Queen,
  Rook,
  Bishop,
  Knight,
  Pawn,
  Grasshopper,
  Nightrider
};
enum Colour { White, Black };
struct Piece {
  Square square;
  PieceType pieceType;
  Colour colour;
};
enum StipulationType { Direct = 1, Help, Self };
enum Goal { Mate = 1, Stalemate };
struct Stipulation {
  StipulationType stipulationType;
  Goal goal;
  int nMoves;
};
struct Options {
  bool tri;
  int defence;
  bool setPlay;
  bool nullMoves;
  bool whiteToPlay;
  bool variation;
  bool moveNumbers;
  bool noThreat;
  std::vector<Square> enPassant;
  bool noBoard;
  bool noShortVariations;
  bool halfDuplex;
  std::vector<Square> noCastling;
};
struct Conditions {
  bool circe;
};
struct Problem {
  Conditions conditions;
  Options options;
  Stipulation stipulation;
  std::vector<Piece> pieces;
};

}  // namespace popeye

void validateProblem(const popeye::Problem& specification);
void verifyProblem(const popeye::Problem& specification);
void convertProblem(const popeye::Problem& specification, int inputLanguage,
                    std::vector<Task>& tasks);

std::istream& operator>>(std::istream& input, std::vector<Task>& tasks) {
  enum Transition {
    kPopeyeLanguage,
    kPopeyeCommand,
    kPopeyeCondition,
    kPopeyeOption,
    kPopeyeDefence,
    kPopeyeEnPassant,
    kPopeyeNoCastling,
    kPopeyeStipulation,
    kPopeyeColour,
    kPopeyePiece,
    kPopeyeDirective,
    kEpdForsyth,
    kEpdSideToMove,
    kEpdCastling,
    kEpdEnPassant,
    kEpdOpcode,
    kEpdAcd,
    kEpdDm
  };
  std::set<Transition> transitions = {kPopeyeLanguage, kEpdForsyth};
  bool english = false;
  bool french = false;
  bool german = false;
  std::vector<popeye::Problem> problems;
  popeye::Problem problem = {};
  popeye::Colour colour = {};
  bool hasNextLine = true;
  for (std::string line; hasNextLine && std::getline(input, line);) {
    bool epdLine = false;
    std::istringstream tokenInput(line);
    bool hasNextToken = true;
    for (std::string token; hasNextToken && tokenInput >> token;) {
      std::string popeyeToken;
      for (unsigned char symbol : token) {
        popeyeToken.push_back(std::tolower(symbol));
      }
      if (transitions.count(kPopeyeLanguage) && popeyeToken == "beginproblem") {
        english = true;
        transitions = {kPopeyeCommand, kPopeyeDirective};
      } else if (transitions.count(kPopeyeLanguage) &&
                 popeyeToken == "debutprobleme") {
        french = true;
        transitions = {kPopeyeCommand, kPopeyeDirective};
      } else if (transitions.count(kPopeyeLanguage) &&
                 popeyeToken == "anfangproblem") {
        german = true;
        transitions = {kPopeyeCommand, kPopeyeDirective};
      } else if (transitions.count(kPopeyeCommand) &&
                 popeyeToken == (!german ? !french ? "remark" : "remarque"
                                         : "bemerkung")) {
        hasNextToken = false;
        transitions = {kPopeyeCommand, kPopeyeDirective};
      } else if (transitions.count(kPopeyeCommand) &&
                 popeyeToken == (!german ? !french ? "condition" : "condition"
                                         : "bedingung")) {
        transitions = {kPopeyeCondition};
      } else if (transitions.count(kPopeyeCondition) &&
                 popeyeToken ==
                     (!german ? !french ? "circe" : "circe" : "circe")) {
        problem.conditions.circe = true;
        transitions = {kPopeyeCondition, kPopeyeCommand, kPopeyeDirective};
      } else if (transitions.count(kPopeyeCommand) &&
                 popeyeToken ==
                     (!german ? !french ? "option" : "option" : "option")) {
        transitions = {kPopeyeOption};
      } else if (transitions.count(kPopeyeOption) &&
                 popeyeToken ==
                     (!german ? !french ? "try" : "essais" : "verfuehrung")) {
        problem.options.tri = true;
        transitions = {kPopeyeOption, kPopeyeCommand, kPopeyeDirective};
      } else if (transitions.count(kPopeyeOption) &&
                 popeyeToken == (!german ? !french ? "defence" : "defense"
                                         : "widerlegung")) {
        transitions = {kPopeyeDefence};
      } else if (transitions.count(kPopeyeDefence) &&
                 std::regex_match(popeyeToken, std::regex("[1-9]\\d*"))) {
        int defence = std::stoi(popeyeToken);
        problem.options.defence = defence;
        transitions = {kPopeyeOption, kPopeyeCommand, kPopeyeDirective};
      } else if (transitions.count(kPopeyeOption) &&
                 popeyeToken == (!german ? !french ? "setplay" : "apparent"
                                         : "satzspiel")) {
        problem.options.setPlay = true;
        transitions = {kPopeyeOption, kPopeyeCommand, kPopeyeDirective};
      } else if (transitions.count(kPopeyeOption) &&
                 popeyeToken == (!german ? !french ? "nullmoves" : "coupsvides"
                                         : "nullzuege")) {
        problem.options.nullMoves = true;
        transitions = {kPopeyeOption, kPopeyeCommand, kPopeyeDirective};
      } else if (transitions.count(kPopeyeOption) &&
                 popeyeToken == (!german
                                     ? !french ? "whitetoplay" : "apparentseul"
                                     : "weissbeginnt")) {
        problem.options.whiteToPlay = true;
        transitions = {kPopeyeOption, kPopeyeCommand, kPopeyeDirective};
      } else if (transitions.count(kPopeyeOption) &&
                 popeyeToken == (!german ? !french ? "variation" : "variantes"
                                         : "varianten")) {
        problem.options.variation = true;
        transitions = {kPopeyeOption, kPopeyeCommand, kPopeyeDirective};
      } else if (transitions.count(kPopeyeOption) &&
                 popeyeToken == (!german ? !french ? "movenumbers" : "trace"
                                         : "zugnummern")) {
        problem.options.moveNumbers = true;
        transitions = {kPopeyeOption, kPopeyeCommand, kPopeyeDirective};
      } else if (transitions.count(kPopeyeOption) &&
                 popeyeToken == (!german ? !french ? "nothreat" : "sansmenace"
                                         : "ohnedrohung")) {
        problem.options.noThreat = true;
        transitions = {kPopeyeOption, kPopeyeCommand, kPopeyeDirective};
      } else if (transitions.count(kPopeyeOption) &&
                 popeyeToken == (!german ? !french ? "enpassant" : "enpassant"
                                         : "enpassant")) {
        transitions = {kPopeyeEnPassant};
      } else if (transitions.count(kPopeyeEnPassant) &&
                 std::regex_match(popeyeToken,
                                  std::regex("([a-h][1-8]){1,2}"))) {
        std::string squaresSubsequence = popeyeToken;
        for (std::smatch squareMatch; std::regex_search(
                 squaresSubsequence, squareMatch, std::regex("[a-h][1-8]"));) {
          popeye::File file =
              static_cast<popeye::File>(squareMatch.str()[0] - 'a');
          popeye::Rank rank =
              static_cast<popeye::Rank>(squareMatch.str()[1] - '1');
          problem.options.enPassant.push_back({file, rank});
          squaresSubsequence = squareMatch.suffix();
        };
        transitions = {kPopeyeOption, kPopeyeCommand, kPopeyeDirective};
      } else if (transitions.count(kPopeyeOption) &&
                 popeyeToken == (!german ? !french ? "noboard" : "sansechiquier"
                                         : "ohnebrett")) {
        problem.options.noBoard = true;
        transitions = {kPopeyeOption, kPopeyeCommand, kPopeyeDirective};
      } else if (transitions.count(kPopeyeOption) &&
                 popeyeToken == (!german ? !french ? "noshortvariations"
                                                   : "sansvariantescourtes"
                                         : "ohnekurzvarianten")) {
        problem.options.noShortVariations = true;
        transitions = {kPopeyeOption, kPopeyeCommand, kPopeyeDirective};
      } else if (transitions.count(kPopeyeOption) &&
                 popeyeToken == (!german ? !french ? "halfduplex" : "demiduplex"
                                         : "halbduplex")) {
        problem.options.halfDuplex = true;
        transitions = {kPopeyeOption, kPopeyeCommand, kPopeyeDirective};
      } else if (transitions.count(kPopeyeOption) &&
                 popeyeToken == (!german ? !french ? "nocastling" : "sansroquer"
                                         : "keinerochade")) {
        transitions = {kPopeyeNoCastling};
      } else if (transitions.count(kPopeyeNoCastling) &&
                 std::regex_match(popeyeToken, std::regex("([a-h][1-8])+"))) {
        std::string squaresSubsequence = popeyeToken;
        for (std::smatch squareMatch; std::regex_search(
                 squaresSubsequence, squareMatch, std::regex("[a-h][1-8]"));) {
          popeye::File file =
              static_cast<popeye::File>(squareMatch.str()[0] - 'a');
          popeye::Rank rank =
              static_cast<popeye::Rank>(squareMatch.str()[1] - '1');
          problem.options.noCastling.push_back({file, rank});
          squaresSubsequence = squareMatch.suffix();
        };
        transitions = {kPopeyeOption, kPopeyeCommand, kPopeyeDirective};
      } else if (transitions.count(kPopeyeCommand) &&
                 popeyeToken == (!german ? !french ? "stipulation" : "enonce"
                                         : "forderung")) {
        transitions = {kPopeyeStipulation};
      } else if (transitions.count(kPopeyeStipulation) &&
                 std::regex_match(popeyeToken,
                                  std::regex("(|h|s)(#|=)([1-9]\\d*)"))) {
        std::smatch stipulationMatch;
        std::regex_match(popeyeToken, stipulationMatch,
                         std::regex("(|h|s)(#|=)([1-9]\\d*)"));
        popeye::StipulationType stipulationType =
            stipulationMatch[1] == "h"   ? popeye::Help
            : stipulationMatch[1] == "s" ? popeye::Self
                                         : popeye::Direct;
        popeye::Goal goal =
            stipulationMatch[2] == "=" ? popeye::Stalemate : popeye::Mate;
        int nMoves = std::stoi(stipulationMatch[3]);
        problem.stipulation = {stipulationType, goal, nMoves};
        transitions = {kPopeyeCommand, kPopeyeDirective};
      } else if (transitions.count(kPopeyeCommand) &&
                 popeyeToken ==
                     (!german ? !french ? "pieces" : "pieces" : "steine")) {
        transitions = {kPopeyeColour};
      } else if (transitions.count(kPopeyeColour) &&
                 popeyeToken ==
                     (!german ? !french ? "white" : "blanc" : "weiss")) {
        colour = popeye::White;
        transitions = {kPopeyePiece};
      } else if (transitions.count(kPopeyeColour) &&
                 popeyeToken ==
                     (!german ? !french ? "black" : "noir" : "schwarz")) {
        colour = popeye::Black;
        transitions = {kPopeyePiece};
      } else if (transitions.count(kPopeyePiece) &&
                 std::regex_match(
                     popeyeToken,
                     std::regex(
                         !german ? !french ? "(k|q|r|b|s|p|g|n)(([a-h][1-8])+)"
                                           : "(r|d|t|f|c|p|s|n)(([a-h][1-8])+)"
                                 : "(k|d|t|l|s|b|g|n)(([a-h][1-8])+)"))) {
        std::smatch pieceMatch;
        std::regex_match(
            popeyeToken, pieceMatch,
            std::regex(!german ? !french ? "(k|q|r|b|s|p|g|n)(([a-h][1-8])+)"
                                         : "(r|d|t|f|c|p|s|n)(([a-h][1-8])+)"
                               : "(k|d|t|l|s|b|g|n)(([a-h][1-8])+)"));
        popeye::PieceType pieceType =
            pieceMatch[1] == (!german ? !french ? "q" : "d" : "d")
                ? popeye::Queen
            : pieceMatch[1] == (!german ? !french ? "r" : "t" : "t")
                ? popeye::Rook
            : pieceMatch[1] == (!german ? !french ? "b" : "f" : "l")
                ? popeye::Bishop
            : pieceMatch[1] == (!german ? !french ? "s" : "c" : "s")
                ? popeye::Knight
            : pieceMatch[1] == (!german ? !french ? "p" : "p" : "b")
                ? popeye::Pawn
            : pieceMatch[1] == (!german ? !french ? "g" : "s" : "g")
                ? popeye::Grasshopper
            : pieceMatch[1] == (!german ? !french ? "n" : "n" : "n")
                ? popeye::Nightrider
                : popeye::King;
        std::string squaresSubsequence = pieceMatch[2];
        for (std::smatch squareMatch; std::regex_search(
                 squaresSubsequence, squareMatch, std::regex("[a-h][1-8]"));) {
          popeye::File file =
              static_cast<popeye::File>(squareMatch.str()[0] - 'a');
          popeye::Rank rank =
              static_cast<popeye::Rank>(squareMatch.str()[1] - '1');
          popeye::Square square = {file, rank};
          problem.pieces.push_back({square, pieceType, colour});
          squaresSubsequence = squareMatch.suffix();
        };
        transitions = {kPopeyePiece, kPopeyeColour, kPopeyeCommand,
                       kPopeyeDirective};
      } else if (transitions.count(kPopeyeDirective) &&
                 popeyeToken == (!german ? !french ? "nextproblem" : "asuivre"
                                         : "weiteresproblem")) {
        problems.push_back(problem);
        problem = {};
        transitions = {kPopeyeCommand, kPopeyeDirective};
      } else if (transitions.count(kPopeyeDirective) &&
                 popeyeToken == (!german
                                     ? !french ? "endproblem" : "finprobleme"
                                     : "endeproblem")) {
        problems.push_back(problem);
        hasNextToken = false;
        hasNextLine = false;
        transitions = {};
      } else if (!epdLine && transitions.count(kEpdForsyth) &&
                 !std::regex_search(token, std::regex("\\d{2,}")) &&
                 std::regex_match(
                     std::accumulate(
                         token.cbegin(), token.cend(), std::string(),
                         [](const std::string& sum, unsigned char symbol) {
                           return sum + (std::isdigit(symbol)
                                             ? std::string(symbol - '0', '1')
                                             : std::string(1, symbol));
                         }),
                     std::regex(
                         "([KQRBNPkqrbnp1]{8}/){7}[KQRBNPkqrbnp1]{8}"))) {
        int iRank = popeye::_8;
        int iFile = popeye::_a;
        for (unsigned char symbol : token) {
          if (std::isalpha(symbol)) {
            popeye::Colour colour =
                std::islower(symbol) ? popeye::Black : popeye::White;
            popeye::PieceType pieceType =
                std::tolower(symbol) == 'q'   ? popeye::Queen
                : std::tolower(symbol) == 'r' ? popeye::Rook
                : std::tolower(symbol) == 'b' ? popeye::Bishop
                : std::tolower(symbol) == 'n' ? popeye::Knight
                : std::tolower(symbol) == 'p' ? popeye::Pawn
                                              : popeye::King;
            popeye::Square square = {static_cast<popeye::File>(iFile),
                                     static_cast<popeye::Rank>(iRank)};
            problem.pieces.push_back({square, pieceType, colour});
            iFile++;
          } else if (std::isdigit(symbol)) {
            iFile += symbol - '0';
          } else {
            iRank--;
            iFile = popeye::_a;
          }
        }
        epdLine = true;
        hasNextLine = false;
        transitions = {kEpdSideToMove};
      } else if (epdLine && transitions.count(kEpdSideToMove) && token == "w") {
        colour = popeye::White;
        transitions = {kEpdCastling};
      } else if (epdLine && transitions.count(kEpdSideToMove) && token == "b") {
        colour = popeye::Black;
        transitions = {kEpdCastling};
      } else if (epdLine && transitions.count(kEpdCastling) &&
                 std::regex_match(token, std::regex("\\bK?Q?k?q?"))) {
        if (token.find('K') == std::string::npos) {
          problem.options.noCastling.push_back({popeye::_h, popeye::_1});
        }
        if (token.find('Q') == std::string::npos) {
          problem.options.noCastling.push_back({popeye::_a, popeye::_1});
        }
        if (token.find('K') == std::string::npos &&
            token.find('Q') == std::string::npos) {
          problem.options.noCastling.push_back({popeye::_e, popeye::_1});
        }
        if (token.find('k') == std::string::npos) {
          problem.options.noCastling.push_back({popeye::_h, popeye::_8});
        }
        if (token.find('q') == std::string::npos) {
          problem.options.noCastling.push_back({popeye::_a, popeye::_8});
        }
        if (token.find('k') == std::string::npos &&
            token.find('q') == std::string::npos) {
          problem.options.noCastling.push_back({popeye::_e, popeye::_8});
        }
        transitions = {kEpdEnPassant};
      } else if (epdLine && transitions.count(kEpdCastling) && token == "-") {
        for (const popeye::File& file : {popeye::_h, popeye::_a, popeye::_e}) {
          for (const popeye::Rank& rank : {popeye::_1, popeye::_8}) {
            problem.options.noCastling.push_back({file, rank});
          }
        }
        transitions = {kEpdEnPassant};
      } else if (epdLine && transitions.count(kEpdEnPassant) &&
                 std::regex_match(token, std::regex("[a-h][36]"))) {
        popeye::File file = static_cast<popeye::File>(token[0] - 'a');
        popeye::Rank rank = static_cast<popeye::Rank>(token[1] - '1');
        problem.options.enPassant.push_back({file, rank});
        transitions = {kEpdOpcode};
      } else if (epdLine && transitions.count(kEpdEnPassant) && token == "-") {
        transitions = {kEpdOpcode};
      } else if (epdLine && transitions.count(kEpdOpcode) && token == "acd") {
        transitions = {kEpdAcd};
      } else if (epdLine && transitions.count(kEpdAcd) &&
                 std::regex_match(token, std::regex("(0|[1-9]\\d*);"))) {
        int nPlies;
        std::istringstream(token) >> nPlies;
        int nMoves = (nPlies + 1) / 2;
        problem.stipulation = {popeye::Help, {}, nMoves};
        if (nPlies % 2 == 1) {
          problem.options.whiteToPlay = true;
          if (colour == popeye::Black) {
            problem.options.halfDuplex = true;
          }
        } else {
          if (colour == popeye::White) {
            problem.options.halfDuplex = true;
          }
        }
        problems.push_back(problem);
        problem = {};
        hasNextLine = true;
        transitions = {kEpdForsyth};
      } else if (epdLine && transitions.count(kEpdOpcode) && token == "dm") {
        transitions = {kEpdDm};
      } else if (epdLine && transitions.count(kEpdDm) &&
                 std::regex_match(token, std::regex("[1-9]\\d*;"))) {
        int nMoves;
        std::istringstream(token) >> nMoves;
        problem.stipulation = {popeye::Direct, {}, nMoves};
        if (colour == popeye::Black) {
          problem.options.halfDuplex = true;
        }
        problems.push_back(problem);
        problem = {};
        hasNextLine = true;
        transitions = {kEpdForsyth};
      } else {
        throw std::invalid_argument("Parse failure (invalid token: \"" + token +
                                    "\").");
      }
    }
  }
  if (!transitions.empty() && !transitions.count(kEpdForsyth)) {
    throw std::invalid_argument("Parse failure (incomplete input).");
  }
  int inputLanguage = german ? 3 : french ? 2 : english ? 1 : 0;
  for (const popeye::Problem& specification : problems) {
    validateProblem(specification);
    verifyProblem(specification);
    convertProblem(specification, inputLanguage, tasks);
  }
  return input;
}

void validateProblem(const popeye::Problem& specification) {
  if (!specification.stipulation.stipulationType) {
    throw std::invalid_argument(
        "Task conversion failure (missing stipulation).");
  }
  for (const popeye::Colour& colour : {popeye::White, popeye::Black}) {
    std::set<std::pair<popeye::File, popeye::Rank>> origins;
    int nKings = std::accumulate(
        specification.pieces.crbegin(), specification.pieces.crend(), 0,
        [&colour, &origins](int sum, const popeye::Piece& piece) {
          if (origins.insert({piece.square.file, piece.square.rank}).second) {
            if (piece.colour == colour && piece.pieceType == popeye::King) {
              return sum + 1;
            }
          }
          return sum;
        });
    if (nKings > 1) {
      throw std::invalid_argument(
          "Task conversion failure (too many " +
          std::string(colour == popeye::Black ? "black" : "white") +
          " kings).");
    }
    if (nKings == 0) {
      throw std::invalid_argument(
          "Task conversion failure (missing " +
          std::string(colour == popeye::Black ? "black" : "white") + " king).");
    }
  }
}

void verifyProblem(const popeye::Problem& specification) {
  for (const popeye::Square& square : specification.options.noCastling) {
    if (!(square.file == popeye::_e || square.file == popeye::_a ||
          square.file == popeye::_h) ||
        !(square.rank == popeye::_1 || square.rank == popeye::_8)) {
      throw std::domain_error(
          "Task creation failure (not accepted option: nocastling " +
          std::string()
              .append(1, 'a' + square.file)
              .append(1, '1' + square.rank) +
          ").");
    }
  }
  std::set<std::pair<popeye::File, popeye::Rank>> targets;
  std::vector<popeye::Square> enPassant;
  std::for_each(specification.options.enPassant.crbegin(),
                specification.options.enPassant.crend(),
                [&targets, &enPassant](const popeye::Square& square) {
                  if (targets.insert({square.file, square.rank}).second) {
                    enPassant.insert(enPassant.cbegin(), square);
                  }
                });
  if (!enPassant.empty()) {
    if (specification.options.setPlay) {
      throw std::domain_error(
          "Task creation failure (not accepted option: enpassant w/ setplay).");
    }
    if (enPassant.size() > 1) {
      throw std::domain_error(
          "Task creation failure (not accepted option: multiple enpassant).");
    }
    const popeye::Square& square = enPassant.front();
    std::set<std::pair<popeye::File, popeye::Rank>> origins;
    std::vector<popeye::Piece> pieces;
    std::for_each(
        specification.pieces.crbegin(), specification.pieces.crend(),
        [&origins, &pieces](const popeye::Piece& piece) {
          if (origins.insert({piece.square.file, piece.square.rank}).second) {
            pieces.insert(pieces.cbegin(), piece);
          }
        });
    popeye::Colour sideToMove =
        (specification.stipulation.stipulationType == popeye::Help
             ? specification.options.halfDuplex ==
                   specification.options.whiteToPlay
             : specification.options.halfDuplex)
            ? popeye::Black
            : popeye::White;
    if (!(square.rank == popeye::_3 &&
              std::none_of(pieces.cbegin(), pieces.cend(),
                           [&square](const popeye::Piece& piece) {
                             return piece.square.file == square.file &&
                                    (piece.square.rank == square.rank ||
                                     piece.square.rank == popeye::_2);
                           }) &&
              std::any_of(pieces.cbegin(), pieces.cend(),
                          [&square](const popeye::Piece& piece) {
                            return piece.square.file == square.file &&
                                   piece.square.rank == popeye::_4 &&
                                   piece.colour == popeye::White &&
                                   piece.pieceType == popeye::Pawn;
                          }) &&
              sideToMove == popeye::Black ||
          square.rank == popeye::_6 &&
              std::none_of(pieces.cbegin(), pieces.cend(),
                           [&square](const popeye::Piece& piece) {
                             return piece.square.file == square.file &&
                                    (piece.square.rank == square.rank ||
                                     piece.square.rank == popeye::_7);
                           }) &&
              std::any_of(pieces.cbegin(), pieces.cend(),
                          [&square](const popeye::Piece& piece) {
                            return piece.square.file == square.file &&
                                   piece.square.rank == popeye::_5 &&
                                   piece.colour == popeye::Black &&
                                   piece.pieceType == popeye::Pawn;
                          }) &&
              sideToMove == popeye::White)) {
      throw std::domain_error(
          "Task creation failure (not accepted option: enpassant " +
          std::string()
              .append(1, 'a' + square.file)
              .append(1, '1' + square.rank) +
          ").");
    }
  }
}

void convertProblem(const popeye::Problem& specification, int inputLanguage,
                    std::vector<Task>& tasks) {
  std::array<std::unique_ptr<Piece>, 128> board;
  for (const popeye::Piece& piece : specification.pieces) {
    int square = 16 * piece.square.file + piece.square.rank;
    bool black = piece.colour == popeye::Black;
    if (piece.pieceType == popeye::Queen) {
      board[square] = std::make_unique<Queen>(black);
    } else if (piece.pieceType == popeye::Rook) {
      board[square] = std::make_unique<Rook>(black);
    } else if (piece.pieceType == popeye::Bishop) {
      board[square] = std::make_unique<Bishop>(black);
    } else if (piece.pieceType == popeye::Knight) {
      board[square] = std::make_unique<Knight>(black);
    } else if (piece.pieceType == popeye::Pawn) {
      board[square] = std::make_unique<Pawn>(black);
    } else if (piece.pieceType == popeye::Grasshopper) {
      board[square] = std::make_unique<Grasshopper>(black);
    } else if (piece.pieceType == popeye::Nightrider) {
      board[square] = std::make_unique<Nightrider>(black);
    } else {
      board[square] = std::make_unique<King>(black);
    }
  }
  std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>> box;
  std::set<popeye::PieceType> promotionTypes = {popeye::Queen, popeye::Rook,
                                                popeye::Bishop, popeye::Knight};
  for (const popeye::Piece& piece : specification.pieces) {
    if (piece.pieceType != popeye::King && piece.pieceType != popeye::Pawn) {
      promotionTypes.insert(piece.pieceType);
    }
  }
  for (const popeye::Colour& colour : {popeye::White, popeye::Black}) {
    int maxMove =
        (specification.stipulation.stipulationType == popeye::Direct ||
         specification.stipulation.stipulationType == popeye::Help &&
             specification.options.whiteToPlay) &&
                colour == popeye::White == specification.options.halfDuplex
            ? specification.stipulation.nMoves
            : specification.stipulation.nMoves + 1;
    std::set<std::pair<popeye::File, popeye::Rank>> origins;
    int nPawns = std::accumulate(
        specification.pieces.crbegin(), specification.pieces.crend(), 0,
        [&colour, &origins](int sum, const popeye::Piece& piece) {
          if (origins.insert({piece.square.file, piece.square.rank}).second) {
            if (piece.colour == colour && piece.pieceType == popeye::Pawn) {
              return sum + 1;
            }
          }
          return sum;
        });
    int maxPromotion = std::min(maxMove, nPawns);
    if (maxPromotion) {
      bool black = colour == popeye::Black;
      for (int promotionNo = maxPromotion; promotionNo--;) {
        int order = 0;
        for (const popeye::PieceType& promotionType : promotionTypes) {
          if (promotionType == popeye::Rook) {
            box[black][++order].push_front(std::make_unique<Rook>(black));
          } else if (promotionType == popeye::Bishop) {
            box[black][++order].push_front(std::make_unique<Bishop>(black));
          } else if (promotionType == popeye::Knight) {
            box[black][++order].push_front(std::make_unique<Knight>(black));
          } else if (promotionType == popeye::Grasshopper) {
            box[black][++order].push_front(
                std::make_unique<Grasshopper>(black));
          } else if (promotionType == popeye::Nightrider) {
            box[black][++order].push_front(std::make_unique<Nightrider>(black));
          } else {
            box[black][++order].push_front(std::make_unique<Queen>(black));
          }
        }
      }
    }
  }
  std::stack<std::unique_ptr<Piece>> table;
  bool blackToMove = specification.stipulation.stipulationType == popeye::Help
                         ? specification.options.halfDuplex ==
                               specification.options.whiteToPlay
                         : specification.options.halfDuplex;
  std::pair<std::set<int>, std::shared_ptr<int>> state;
  std::set<std::pair<popeye::File, popeye::Rank>> origins;
  std::for_each(
      specification.pieces.crbegin(), specification.pieces.crend(),
      [&specification, &origins, &state](const popeye::Piece& piece) {
        if (origins.insert({piece.square.file, piece.square.rank}).second) {
          if ((piece.pieceType == popeye::King &&
                   piece.square.file == popeye::_e ||
               piece.pieceType == popeye::Rook &&
                   (piece.square.file == popeye::_a ||
                    piece.square.file == popeye::_h)) &&
              (piece.colour == popeye::White &&
                   piece.square.rank == popeye::_1 ||
               piece.colour == popeye::Black &&
                   piece.square.rank == popeye::_8)) {
            if (std::find_if(specification.options.noCastling.cbegin(),
                             specification.options.noCastling.cend(),
                             [&piece](const popeye::Square& origin) {
                               return origin.file == piece.square.file &&
                                      origin.rank == piece.square.rank;
                             }) == specification.options.noCastling.cend()) {
              int square = 16 * piece.square.file + piece.square.rank;
              state.first.insert(square);
            }
          }
        }
      });
  for (const popeye::Square& target : specification.options.enPassant) {
    int square = 16 * target.file + target.rank;
    state.second = std::make_shared<int>(square);
  }
  std::stack<std::pair<std::set<int>, std::shared_ptr<int>>> memory;
  std::unique_ptr<MoveFactory> moveFactory;
  if (specification.conditions.circe) {
    moveFactory = std::make_unique<CirceMoveFactory>();
  } else {
    moveFactory = std::make_unique<MoveFactory>();
  }
  Position position(std::move(board), std::move(box), std::move(table),
                    blackToMove, std::move(state), std::move(memory),
                    std::move(moveFactory));
  int nMoves = specification.stipulation.stipulationType == popeye::Help &&
                       specification.options.whiteToPlay
                   ? specification.stipulation.nMoves - 1
                   : specification.stipulation.nMoves;
  bool halfMove = specification.stipulation.stipulationType == popeye::Help &&
                  specification.options.whiteToPlay;
  bool stalemate = specification.stipulation.goal == popeye::Stalemate;
  std::unique_ptr<Problem> problem;
  if (specification.stipulation.stipulationType == popeye::Help) {
    if (!specification.stipulation.goal) {
      problem = std::make_unique<Perft>(std::move(position), nMoves, halfMove);
    } else {
      problem = std::make_unique<Helpmate>(std::move(position), stalemate,
                                           nMoves, halfMove);
    }
  } else if (specification.stipulation.stipulationType == popeye::Self) {
    problem =
        std::make_unique<Selfmate>(std::move(position), stalemate, nMoves);
  } else {
    if (!specification.stipulation.goal) {
      problem = std::make_unique<MateSearch>(std::move(position), nMoves);
    } else {
      problem =
          std::make_unique<Directmate>(std::move(position), stalemate, nMoves);
    }
  }
  bool setPlay = specification.options.setPlay;
  int nRefutations =
      !(specification.stipulation.stipulationType == popeye::Help)
          ? std::max(specification.options.defence,
                     specification.options.tri ? 1 : 0)
          : 0;
  bool variations =
      !(specification.stipulation.stipulationType == popeye::Help) &&
      specification.options.variation;
  bool threats = !(specification.stipulation.stipulationType == popeye::Help) &&
                 specification.options.variation &&
                 !specification.options.noThreat;
  bool shortVariations =
      !(specification.stipulation.stipulationType == popeye::Help) &&
      specification.options.variation &&
      !specification.options.noShortVariations;
  bool tempoTries =
      specification.stipulation.stipulationType == popeye::Help &&
      (specification.options.nullMoves || specification.options.tri);
  AnalysisOptions analysisOptions;
  if (specification.stipulation.goal) {
    analysisOptions = {setPlay, nRefutations,    variations,
                       threats, shortVariations, tempoTries};
  }
  int outputLanguage = inputLanguage;
  bool internalModel = !specification.options.noBoard;
  bool internalProgress = specification.options.moveNumbers;
  DisplayOptions displayOptions;
  if (specification.stipulation.goal) {
    displayOptions = {outputLanguage, internalModel, internalProgress};
  }
  tasks.push_back({std::move(problem), analysisOptions, displayOptions});
}

}  // namespace moderato
