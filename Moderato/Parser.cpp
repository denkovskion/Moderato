/*
 * MIT License
 *
 * Copyright (c) 2024-2025 Ivan Denkovski
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
#include <iterator>
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
  Nightrider,
  Amazon
};
enum Colour { White, Black };
struct Piece {
  Square square;
  PieceType pieceType;
  Colour colour;
};
enum StipulationType { Direct, Help, Self };
enum Goal { Mate, Stalemate };
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

namespace model {

enum Piece {
  WhiteKing = 1,
  WhiteQueen,
  WhiteRook,
  WhiteBishop,
  WhiteKnight,
  WhitePawn,
  BlackKing,
  BlackQueen,
  BlackRook,
  BlackBishop,
  BlackKnight,
  BlackPawn
};
enum Colour { White, Black };
enum Castling { WhiteShort, WhiteLong, BlackShort, BlackLong };
struct Square {
  int index;
  bool present;
};
enum Opcode { ACD, DM };
struct Operation {
  Opcode opcode;
  int operand;
};
struct Position {
  std::array<Piece, 64> board;
  Colour sideToMove;
  std::set<Castling> castlings;
  Square enPassant;
  Operation operation;
};

}  // namespace model

void validatePosition(const model::Position& specification);
void convertPosition(const model::Position& specification,
                     std::vector<Task>& tasks);

static std::vector<std::vector<std::string>>& popeyeKeywords();
static std::vector<std::vector<std::string>>& pieceTypeCodes();
std::string translateTerm(
    const std::string& term, int inputLanguage, int outputLanguage,
    const std::vector<std::vector<std::string>>& translations);
std::string translatePattern(
    const std::string& pattern, int inputLanguage, int outputLanguage,
    const std::vector<std::vector<std::string>>& translations);

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
    kPopeyeWhite,
    kPopeyeBlack,
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
  int inputLanguage = 0;
  std::vector<popeye::Problem> problems;
  popeye::Problem problem = {};
  std::vector<model::Position> positions;
  model::Position position = {};
  bool hasNextLine = true;
  for (std::string line; hasNextLine && std::getline(input, line);) {
    std::istringstream tokenInput(line);
    if (transitions.empty()) {
      transitions = {kEpdForsyth};
    }
    bool hasNextToken = true;
    for (std::string token; hasNextToken && tokenInput >> token;) {
      if (transitions.count(kPopeyeLanguage)) {
        for (int language : {Piece::ENGLISH, Piece::FRENCH, Piece::GERMAN}) {
          if (translateTerm(token, language, Piece::ENGLISH,
                            popeyeKeywords()) == "BeginProblem") {
            inputLanguage = language;
            break;
          }
        }
        if (inputLanguage != 0) {
          transitions = {kPopeyeCommand, kPopeyeDirective};
          continue;
        }
      }
      if (transitions.count(kPopeyeCommand)) {
        std::string command = translateTerm(token, inputLanguage,
                                            Piece::ENGLISH, popeyeKeywords());
        if (command == "Remark") {
          hasNextToken = false;
          transitions = {kPopeyeCommand, kPopeyeDirective};
          continue;
        }
        if (command == "Condition") {
          transitions = {kPopeyeCondition};
          continue;
        }
        if (command == "Option") {
          transitions = {kPopeyeOption};
          continue;
        }
        if (command == "Stipulation") {
          transitions = {kPopeyeStipulation};
          continue;
        }
        if (command == "Pieces") {
          transitions = {kPopeyeColour};
          continue;
        }
      }
      if (transitions.count(kPopeyeCondition)) {
        std::string condition = translateTerm(token, inputLanguage,
                                              Piece::ENGLISH, popeyeKeywords());
        if (condition == "Circe") {
          problem.conditions.circe = true;
          transitions = {kPopeyeCondition, kPopeyeCommand, kPopeyeDirective};
          continue;
        }
      }
      if (transitions.count(kPopeyeOption)) {
        std::string option = translateTerm(token, inputLanguage, Piece::ENGLISH,
                                           popeyeKeywords());
        if (option == "Try") {
          problem.options.tri = true;
          transitions = {kPopeyeOption, kPopeyeCommand, kPopeyeDirective};
          continue;
        }
        if (option == "Defence") {
          transitions = {kPopeyeDefence};
          continue;
        }
        if (option == "SetPlay") {
          problem.options.setPlay = true;
          transitions = {kPopeyeOption, kPopeyeCommand, kPopeyeDirective};
          continue;
        }
        if (option == "NullMoves") {
          problem.options.nullMoves = true;
          transitions = {kPopeyeOption, kPopeyeCommand, kPopeyeDirective};
          continue;
        }
        if (option == "WhiteToPlay") {
          problem.options.whiteToPlay = true;
          transitions = {kPopeyeOption, kPopeyeCommand, kPopeyeDirective};
          continue;
        }
        if (option == "Variation") {
          problem.options.variation = true;
          transitions = {kPopeyeOption, kPopeyeCommand, kPopeyeDirective};
          continue;
        }
        if (option == "MoveNumbers") {
          problem.options.moveNumbers = true;
          transitions = {kPopeyeOption, kPopeyeCommand, kPopeyeDirective};
          continue;
        }
        if (option == "NoThreat") {
          problem.options.noThreat = true;
          transitions = {kPopeyeOption, kPopeyeCommand, kPopeyeDirective};
          continue;
        }
        if (option == "EnPassant") {
          transitions = {kPopeyeEnPassant};
          continue;
        }
        if (option == "NoBoard") {
          problem.options.noBoard = true;
          transitions = {kPopeyeOption, kPopeyeCommand, kPopeyeDirective};
          continue;
        }
        if (option == "NoShortVariations") {
          problem.options.noShortVariations = true;
          transitions = {kPopeyeOption, kPopeyeCommand, kPopeyeDirective};
          continue;
        }
        if (option == "HalfDuplex") {
          problem.options.halfDuplex = true;
          transitions = {kPopeyeOption, kPopeyeCommand, kPopeyeDirective};
          continue;
        }
        if (option == "NoCastling") {
          transitions = {kPopeyeNoCastling};
          continue;
        }
      }
      if (transitions.count(kPopeyeDefence)) {
        if (std::regex_match(token, std::regex("[1-9]\\d*"))) {
          int defence = std::stoi(token);
          problem.options.defence = defence;
          transitions = {kPopeyeOption, kPopeyeCommand, kPopeyeDirective};
          continue;
        }
      }
      if (transitions.count(kPopeyeEnPassant)) {
        if (std::regex_match(token, std::regex("([A-Ha-h][1-8]){1,2}"))) {
          std::string squaresSubsequence = token;
          for (std::smatch squareMatch;
               std::regex_search(squaresSubsequence, squareMatch,
                                 std::regex("[A-Ha-h][1-8]"));) {
            popeye::File file =
                static_cast<popeye::File>(std::islower(squareMatch.str()[0])
                                              ? squareMatch.str()[0] - 'a'
                                              : squareMatch.str()[0] - 'A');
            popeye::Rank rank =
                static_cast<popeye::Rank>(squareMatch.str()[1] - '1');
            problem.options.enPassant.push_back({file, rank});
            squaresSubsequence = squareMatch.suffix();
          };
          transitions = {kPopeyeOption, kPopeyeCommand, kPopeyeDirective};
          continue;
        }
      }
      if (transitions.count(kPopeyeNoCastling)) {
        if (std::regex_match(token, std::regex("([A-Ha-h][1-8])+"))) {
          std::string squaresSubsequence = token;
          for (std::smatch squareMatch;
               std::regex_search(squaresSubsequence, squareMatch,
                                 std::regex("[A-Ha-h][1-8]"));) {
            popeye::File file =
                static_cast<popeye::File>(std::islower(squareMatch.str()[0])
                                              ? squareMatch.str()[0] - 'a'
                                              : squareMatch.str()[0] - 'A');
            popeye::Rank rank =
                static_cast<popeye::Rank>(squareMatch.str()[1] - '1');
            problem.options.noCastling.push_back({file, rank});
            squaresSubsequence = squareMatch.suffix();
          };
          transitions = {kPopeyeOption, kPopeyeCommand, kPopeyeDirective};
          continue;
        }
      }
      if (transitions.count(kPopeyeStipulation)) {
        std::string stipulationPattern = "(|[Hh]|[Ss])(#|=)([1-9]\\d*)";
        if (std::regex_match(token, std::regex(stipulationPattern))) {
          std::smatch stipulationMatch;
          std::regex_match(token, stipulationMatch,
                           std::regex(stipulationPattern));
          popeye::StipulationType stipulationType =
              std::regex_match(stipulationMatch[1].str(), std::regex("[Hh]"))
                  ? popeye::Help
              : std::regex_match(stipulationMatch[1].str(), std::regex("[Ss]"))
                  ? popeye::Self
                  : popeye::Direct;
          popeye::Goal goal =
              stipulationMatch[2] == "=" ? popeye::Stalemate : popeye::Mate;
          int nMoves = std::stoi(stipulationMatch[3]);
          problem.stipulation = {stipulationType, goal, nMoves};
          transitions = {kPopeyeCommand, kPopeyeDirective};
          continue;
        }
      }
      if (transitions.count(kPopeyeColour)) {
        std::string colour = translateTerm(token, inputLanguage, Piece::ENGLISH,
                                           popeyeKeywords());
        if (colour == "White") {
          transitions = {kPopeyeWhite};
          continue;
        }
        if (colour == "Black") {
          transitions = {kPopeyeBlack};
          continue;
        }
      }
      if (transitions.count(kPopeyeWhite) || transitions.count(kPopeyeBlack)) {
        std::string piecePattern =
            "(" +
            translatePattern("[Kk]|[Qq]|[Rr]|[Bb]|[Ss]|[Pp]|[Gg]|[Nn]|[Aa][Mm]",
                             Piece::ENGLISH, inputLanguage, pieceTypeCodes()) +
            ")(([A-Ha-h][1-8])+)";
        if (std::regex_match(token, std::regex(piecePattern))) {
          popeye::Colour colour =
              transitions.count(kPopeyeBlack) ? popeye::Black : popeye::White;
          std::smatch pieceMatch;
          std::regex_match(token, pieceMatch, std::regex(piecePattern));
          std::string pieceTypeCode =
              translateTerm(pieceMatch[1].str(), inputLanguage, Piece::ENGLISH,
                            pieceTypeCodes());
          popeye::PieceType pieceType =
              pieceTypeCode == "Q"    ? popeye::Queen
              : pieceTypeCode == "R"  ? popeye::Rook
              : pieceTypeCode == "B"  ? popeye::Bishop
              : pieceTypeCode == "S"  ? popeye::Knight
              : pieceTypeCode == "P"  ? popeye::Pawn
              : pieceTypeCode == "G"  ? popeye::Grasshopper
              : pieceTypeCode == "N"  ? popeye::Nightrider
              : pieceTypeCode == "AM" ? popeye::Amazon
                                      : popeye::King;
          std::string squaresSubsequence = pieceMatch[2];
          for (std::smatch squareMatch;
               std::regex_search(squaresSubsequence, squareMatch,
                                 std::regex("[A-Ha-h][1-8]"));) {
            popeye::File file =
                static_cast<popeye::File>(std::islower(squareMatch.str()[0])
                                              ? squareMatch.str()[0] - 'a'
                                              : squareMatch.str()[0] - 'A');
            popeye::Rank rank =
                static_cast<popeye::Rank>(squareMatch.str()[1] - '1');
            popeye::Square square = {file, rank};
            problem.pieces.push_back({square, pieceType, colour});
            squaresSubsequence = squareMatch.suffix();
          };
          transitions = {
              transitions.count(kPopeyeBlack) ? kPopeyeBlack : kPopeyeWhite,
              kPopeyeColour, kPopeyeCommand, kPopeyeDirective};
          continue;
        }
      }
      if (transitions.count(kPopeyeDirective)) {
        std::string directive = translateTerm(token, inputLanguage,
                                              Piece::ENGLISH, popeyeKeywords());
        if (directive == "NextProblem") {
          problems.push_back(problem);
          problem = {};
          transitions = {kPopeyeCommand, kPopeyeDirective};
          continue;
        }
        if (directive == "EndProblem") {
          problems.push_back(problem);
          problem = {};
          hasNextToken = false;
          hasNextLine = false;
          transitions = {};
          continue;
        }
      }
      if (transitions.count(kEpdForsyth)) {
        if (!std::regex_search(token, std::regex("\\d{2,}")) &&
            std::regex_match(
                std::accumulate(
                    token.cbegin(), token.cend(), std::string(),
                    [](const std::string& sum, unsigned char symbol) {
                      return sum + (std::isdigit(symbol)
                                        ? std::string(symbol - '0', '1')
                                        : std::string(1, symbol));
                    }),
                std::regex("([KQRBNPkqrbnp1]{8}/){7}[KQRBNPkqrbnp1]{8}"))) {
          int index = 0;
          for (unsigned char symbol : token) {
            if (std::isalpha(symbol)) {
              model::Piece piece = symbol == 'q'   ? model::BlackQueen
                                   : symbol == 'Q' ? model::WhiteQueen
                                   : symbol == 'r' ? model::BlackRook
                                   : symbol == 'R' ? model::WhiteRook
                                   : symbol == 'b' ? model::BlackBishop
                                   : symbol == 'B' ? model::WhiteBishop
                                   : symbol == 'n' ? model::BlackKnight
                                   : symbol == 'N' ? model::WhiteKnight
                                   : symbol == 'p' ? model::BlackPawn
                                   : symbol == 'P' ? model::WhitePawn
                                   : symbol == 'k' ? model::BlackKing
                                                   : model::WhiteKing;
              position.board.at(index) = piece;
              index++;
            } else if (std::isdigit(symbol)) {
              index += symbol - '0';
            }
          }
          hasNextLine = false;
          transitions = {kEpdSideToMove};
          continue;
        }
      }
      if (transitions.count(kEpdSideToMove)) {
        if (token == "w") {
          model::Colour sideToMove = model::White;
          position.sideToMove = sideToMove;
          transitions = {kEpdCastling};
          continue;
        }
        if (token == "b") {
          model::Colour sideToMove = model::Black;
          position.sideToMove = sideToMove;
          transitions = {kEpdCastling};
          continue;
        }
      }
      if (transitions.count(kEpdCastling)) {
        if (std::regex_match(token, std::regex("\\bK?Q?k?q?"))) {
          for (unsigned char symbol : token) {
            model::Castling castling = symbol == 'q'   ? model::BlackLong
                                       : symbol == 'Q' ? model::WhiteLong
                                       : symbol == 'k' ? model::BlackShort
                                                       : model::WhiteShort;
            position.castlings.insert(castling);
          }
          transitions = {kEpdEnPassant};
          continue;
        }
        if (token == "-") {
          transitions = {kEpdEnPassant};
          continue;
        }
      }
      if (transitions.count(kEpdEnPassant)) {
        if (std::regex_match(token, std::regex("[a-h][36]"))) {
          int file = token.at(0) - 'a' + 1;
          int rank = token.at(1) - '1' + 1;
          position.enPassant = {(8 - rank) * 8 + file - 1, true};
          transitions = {kEpdOpcode};
          continue;
        }
        if (token == "-") {
          transitions = {kEpdOpcode};
          continue;
        }
      }
      if (transitions.count(kEpdOpcode)) {
        if (token == "acd") {
          transitions = {kEpdAcd};
          continue;
        }
        if (token == "dm") {
          transitions = {kEpdDm};
          continue;
        }
      }
      if (transitions.count(kEpdAcd) || transitions.count(kEpdDm)) {
        if (std::regex_match(token, std::regex(transitions.count(kEpdDm)
                                                   ? "[1-9]\\d*;"
                                                   : "(0|[1-9]\\d*);"))) {
          model::Opcode opcode =
              transitions.count(kEpdDm) ? model::DM : model::ACD;
          int operand;
          std::istringstream(token) >> operand;
          position.operation = {opcode, operand};
          positions.push_back(position);
          position = {};
          hasNextLine = true;
          transitions = {};
          continue;
        }
      }
      throw std::invalid_argument("Parse failure (invalid token: \"" + token +
                                  "\").");
    }
  }
  if (!transitions.empty() && !transitions.count(kEpdForsyth)) {
    throw std::invalid_argument("Parse failure (incomplete input).");
  }
  for (const popeye::Problem& specification : problems) {
    validateProblem(specification);
    verifyProblem(specification);
    convertProblem(specification, inputLanguage, tasks);
  }
  for (const model::Position& specification : positions) {
    validatePosition(specification);
    convertPosition(specification, tasks);
  }
  return input;
}

void validateProblem(const popeye::Problem& specification) {
  if (!specification.stipulation.nMoves) {
    throw std::invalid_argument(
        "Problem conversion failure (missing stipulation).");
  }
  for (const popeye::Colour& colour : {popeye::White, popeye::Black}) {
    int nKings = 0;
    std::vector<popeye::Piece>::const_iterator iPiece =
        specification.pieces.cbegin();
    std::vector<popeye::Piece>::const_iterator nPiece =
        specification.pieces.cend();
    while (true) {
      iPiece =
          std::find_if(iPiece, nPiece, [&colour](const popeye::Piece& piece) {
            return piece.colour == colour && piece.pieceType == popeye::King;
          });
      if (iPiece == nPiece) {
        break;
      }
      if (std::none_of(std::next(iPiece), nPiece,
                       [&iPiece](const popeye::Piece& piece) {
                         return piece.square.file == iPiece->square.file &&
                                piece.square.rank == iPiece->square.rank;
                       })) {
        nKings++;
      }
      iPiece++;
    }
    if (nKings > 1) {
      throw std::invalid_argument(
          "Problem conversion failure (too many " +
          std::string(colour == popeye::Black ? "black" : "white") +
          " kings).");
    }
    if (nKings == 0) {
      throw std::invalid_argument(
          "Problem conversion failure (missing " +
          std::string(colour == popeye::Black ? "black" : "white") + " king).");
    }
  }
}
void verifyProblem(const popeye::Problem& specification) {
  for (const popeye::Square& square : specification.options.noCastling) {
    if (!((square.file == popeye::_e || square.file == popeye::_a ||
           square.file == popeye::_h) &&
          (square.rank == popeye::_1 || square.rank == popeye::_8))) {
      throw std::domain_error(
          "Task creation failure (not accepted option: nocastling " +
          std::string()
              .append(1, 'a' + square.file)
              .append(1, '1' + square.rank) +
          ").");
    }
  }
  std::vector<popeye::Square> enPassant;
  std::vector<popeye::Square>::const_iterator iSquare =
      specification.options.enPassant.cbegin();
  std::vector<popeye::Square>::const_iterator nSquare =
      specification.options.enPassant.cend();
  for (; iSquare != nSquare; iSquare++) {
    if (std::none_of(std::next(iSquare), nSquare,
                     [&iSquare](const popeye::Square& square) {
                       return square.file == iSquare->file &&
                              square.rank == iSquare->rank;
                     })) {
      enPassant.push_back(*iSquare);
    }
  }
  if (!enPassant.empty()) {
    if (specification.options.setPlay) {
      throw std::domain_error(
          "Task creation failure (not accepted option: enpassant w/ setplay).");
    }
    if (enPassant.size() > 1) {
      throw std::domain_error(
          "Task creation failure (not accepted option: multiple enpassant).");
    }
    const popeye::Square& square = enPassant.back();
    std::vector<popeye::Piece> pieces;
    std::vector<popeye::Piece>::const_iterator iPiece =
        specification.pieces.cbegin();
    std::vector<popeye::Piece>::const_iterator nPiece =
        specification.pieces.cend();
    for (; iPiece != nPiece; iPiece++) {
      if (std::none_of(std::next(iPiece), nPiece,
                       [&iPiece](const popeye::Piece& piece) {
                         return piece.square.file == iPiece->square.file &&
                                piece.square.rank == iPiece->square.rank;
                       })) {
        pieces.push_back(*iPiece);
      }
    }
    popeye::Colour sideToMove =
        (specification.stipulation.stipulationType == popeye::Help
             ? specification.options.halfDuplex ==
                   specification.options.whiteToPlay
             : specification.options.halfDuplex)
            ? popeye::Black
            : popeye::White;
    if (!(square.rank ==
              (sideToMove == popeye::Black ? popeye::_3 : popeye::_6) &&
          std::none_of(pieces.cbegin(), pieces.cend(),
                       [&square, &sideToMove](const popeye::Piece& piece) {
                         return piece.square.file == square.file &&
                                (piece.square.rank == square.rank ||
                                 piece.square.rank ==
                                     (sideToMove == popeye::Black
                                          ? popeye::_2
                                          : popeye::_7));
                       }) &&
          std::any_of(pieces.cbegin(), pieces.cend(),
                      [&square, &sideToMove](const popeye::Piece& piece) {
                        return piece.square.file == square.file &&
                               piece.square.rank == (sideToMove == popeye::Black
                                                         ? popeye::_4
                                                         : popeye::_5) &&
                               piece.colour == (sideToMove == popeye::Black
                                                    ? popeye::White
                                                    : popeye::Black) &&
                               piece.pieceType == popeye::Pawn;
                      }))) {
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
      board.at(square) = std::make_unique<Queen>(black);
    } else if (piece.pieceType == popeye::Rook) {
      board.at(square) = std::make_unique<Rook>(black);
    } else if (piece.pieceType == popeye::Bishop) {
      board.at(square) = std::make_unique<Bishop>(black);
    } else if (piece.pieceType == popeye::Knight) {
      board.at(square) = std::make_unique<Knight>(black);
    } else if (piece.pieceType == popeye::Pawn) {
      board.at(square) = std::make_unique<Pawn>(black);
    } else if (piece.pieceType == popeye::Grasshopper) {
      board.at(square) = std::make_unique<Grasshopper>(black);
    } else if (piece.pieceType == popeye::Nightrider) {
      board.at(square) = std::make_unique<Nightrider>(black);
    } else if (piece.pieceType == popeye::Amazon) {
      board.at(square) = std::make_unique<Amazon>(black);
    } else {
      board.at(square) = std::make_unique<King>(black);
    }
  }
  std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>> box;
  std::set<popeye::PieceType> fairyPromotionTypes;
  for (const popeye::Piece& piece : specification.pieces) {
    if (piece.pieceType != popeye::King && piece.pieceType != popeye::Queen &&
        piece.pieceType != popeye::Rook && piece.pieceType != popeye::Bishop &&
        piece.pieceType != popeye::Knight && piece.pieceType != popeye::Pawn) {
      fairyPromotionTypes.insert(piece.pieceType);
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
    int nPawns = 0;
    std::vector<popeye::Piece>::const_iterator iPiece =
        specification.pieces.cbegin();
    std::vector<popeye::Piece>::const_iterator nPiece =
        specification.pieces.cend();
    while (true) {
      iPiece =
          std::find_if(iPiece, nPiece, [&colour](const popeye::Piece& piece) {
            return piece.colour == colour && piece.pieceType == popeye::Pawn;
          });
      if (iPiece == nPiece) {
        break;
      }
      if (std::none_of(std::next(iPiece), nPiece,
                       [&iPiece](const popeye::Piece& piece) {
                         return piece.square.file == iPiece->square.file &&
                                piece.square.rank == iPiece->square.rank;
                       })) {
        nPawns++;
      }
      iPiece++;
    }
    int maxPromotion = std::min(maxMove, nPawns);
    if (maxPromotion) {
      bool black = colour == popeye::Black;
      for (int promotionNo = maxPromotion; promotionNo--;) {
        int order = 0;
        box[black][++order].push_front(std::make_unique<Queen>(black));
        box[black][++order].push_front(std::make_unique<Rook>(black));
        box[black][++order].push_front(std::make_unique<Bishop>(black));
        box[black][++order].push_front(std::make_unique<Knight>(black));
        if (fairyPromotionTypes.count(popeye::Grasshopper)) {
          box[black][++order].push_front(std::make_unique<Grasshopper>(black));
        }
        if (fairyPromotionTypes.count(popeye::Nightrider)) {
          box[black][++order].push_front(std::make_unique<Nightrider>(black));
        }
        if (fairyPromotionTypes.count(popeye::Amazon)) {
          box[black][++order].push_front(std::make_unique<Amazon>(black));
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
  std::vector<popeye::Piece>::const_iterator iPiece =
      specification.pieces.cbegin();
  std::vector<popeye::Piece>::const_iterator nPiece =
      specification.pieces.cend();
  while (true) {
    iPiece = std::find_if(iPiece, nPiece, [](const popeye::Piece& piece) {
      return (piece.pieceType == popeye::King &&
                  piece.square.file == popeye::_e ||
              piece.pieceType == popeye::Rook &&
                  (piece.square.file == popeye::_a ||
                   piece.square.file == popeye::_h)) &&
             (piece.colour == popeye::White &&
                  piece.square.rank == popeye::_1 ||
              piece.colour == popeye::Black && piece.square.rank == popeye::_8);
    });
    if (iPiece == nPiece) {
      break;
    }
    if (std::none_of(std::next(iPiece), nPiece,
                     [&iPiece](const popeye::Piece& piece) {
                       return piece.square.file == iPiece->square.file &&
                              piece.square.rank == iPiece->square.rank;
                     })) {
      if (std::none_of(specification.options.noCastling.cbegin(),
                       specification.options.noCastling.cend(),
                       [&iPiece](const popeye::Square& square) {
                         return square.file == iPiece->square.file &&
                                square.rank == iPiece->square.rank;
                       })) {
        int square = 16 * iPiece->square.file + iPiece->square.rank;
        state.first.insert(square);
      }
    }
    iPiece++;
  }
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
    problem = std::make_unique<Helpmate>(std::move(position), stalemate, nMoves,
                                         halfMove);
  } else if (specification.stipulation.stipulationType == popeye::Self) {
    problem =
        std::make_unique<Selfmate>(std::move(position), stalemate, nMoves);
  } else {
    problem =
        std::make_unique<Directmate>(std::move(position), stalemate, nMoves);
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
  AnalysisOptions analysisOptions = {setPlay, nRefutations,    variations,
                                     threats, shortVariations, tempoTries};
  int outputLanguage = inputLanguage;
  bool internalModel = !specification.options.noBoard;
  bool internalProgress = specification.options.moveNumbers;
  DisplayOptions displayOptions = {outputLanguage, internalModel,
                                   internalProgress};
  tasks.push_back({std::move(problem), analysisOptions, displayOptions});
}

void validatePosition(const model::Position& specification) {
  for (const model::Piece& king : {model::WhiteKing, model::BlackKing}) {
    int nKings = static_cast<int>(std::count(specification.board.cbegin(),
                                             specification.board.cend(), king));
    if (nKings > 1) {
      throw std::invalid_argument(
          "Position conversion failure (too many " +
          std::string(king == model::BlackKing ? "black" : "white") +
          " kings).");
    }
    if (nKings == 0) {
      throw std::invalid_argument(
          "Position conversion failure (missing " +
          std::string(king == model::BlackKing ? "black" : "white") +
          " king).");
    }
  }
  for (const model::Castling& castling : specification.castlings) {
    if (!((castling == model::BlackShort || castling == model::BlackLong
               ? specification.board.at(4) == model::BlackKing
               : specification.board.at(60) == model::WhiteKing) &&
          (castling == model::BlackLong
               ? specification.board.at(0) == model::BlackRook
           : castling == model::WhiteLong
               ? specification.board.at(56) == model::WhiteRook
           : castling == model::BlackShort
               ? specification.board.at(7) == model::BlackRook
               : specification.board.at(63) == model::WhiteRook))) {
      throw std::invalid_argument(
          "Position conversion failure (not accepted castling rights).");
    }
  }
  if (specification.enPassant.present) {
    if (!(specification.sideToMove == model::Black
              ? specification.enPassant.index >= 40 &&
                    specification.enPassant.index <= 47 &&
                    specification.board.at(specification.enPassant.index) ==
                        0 &&
                    specification.board.at(specification.enPassant.index + 8) ==
                        0 &&
                    specification.board.at(specification.enPassant.index - 8) ==
                        model::WhitePawn
              : specification.enPassant.index >= 16 &&
                    specification.enPassant.index <= 23 &&
                    specification.board.at(specification.enPassant.index) ==
                        0 &&
                    specification.board.at(specification.enPassant.index - 8) ==
                        0 &&
                    specification.board.at(specification.enPassant.index + 8) ==
                        model::BlackPawn)) {
      throw std::invalid_argument(
          "Position conversion failure (not accepted en passant square).");
    }
  }
}
void convertPosition(const model::Position& specification,
                     std::vector<Task>& tasks) {
  std::array<std::unique_ptr<Piece>, 128> board;
  for (int index = 0; index < 64; index++) {
    const model::Piece& piece = specification.board.at(index);
    if (piece != 0) {
      int square = 16 * (index % 8) + 7 - index / 8;
      if (piece == model::BlackQueen) {
        board.at(square) = std::make_unique<Queen>(true);
      } else if (piece == model::WhiteQueen) {
        board.at(square) = std::make_unique<Queen>(false);
      } else if (piece == model::BlackRook) {
        board.at(square) = std::make_unique<Rook>(true);
      } else if (piece == model::WhiteRook) {
        board.at(square) = std::make_unique<Rook>(false);
      } else if (piece == model::BlackBishop) {
        board.at(square) = std::make_unique<Bishop>(true);
      } else if (piece == model::WhiteBishop) {
        board.at(square) = std::make_unique<Bishop>(false);
      } else if (piece == model::BlackKnight) {
        board.at(square) = std::make_unique<Knight>(true);
      } else if (piece == model::WhiteKnight) {
        board.at(square) = std::make_unique<Knight>(false);
      } else if (piece == model::BlackPawn) {
        board.at(square) = std::make_unique<Pawn>(true);
      } else if (piece == model::WhitePawn) {
        board.at(square) = std::make_unique<Pawn>(false);
      } else if (piece == model::BlackKing) {
        board.at(square) = std::make_unique<King>(true);
      } else {
        board.at(square) = std::make_unique<King>(false);
      }
    }
  }
  std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>> box;
  for (const model::Colour& colour : {model::White, model::Black}) {
    int maxMove = specification.operation.opcode == model::ACD
                      ? (specification.sideToMove == colour
                             ? (specification.operation.operand + 1) / 2 + 1
                             : specification.operation.operand / 2 + 1)
                      : (specification.sideToMove == colour
                             ? specification.operation.operand + 1
                             : specification.operation.operand);
    int nPawns = static_cast<int>(std::count(
        specification.board.cbegin(), specification.board.cend(),
        colour == model::Black ? model::BlackPawn : model::WhitePawn));
    int maxPromotion = std::min(maxMove, nPawns);
    if (maxPromotion) {
      bool black = colour == model::Black;
      for (int promotionNo = maxPromotion; promotionNo--;) {
        int order = 0;
        box[black][++order].push_front(std::make_unique<Queen>(black));
        box[black][++order].push_front(std::make_unique<Rook>(black));
        box[black][++order].push_front(std::make_unique<Bishop>(black));
        box[black][++order].push_front(std::make_unique<Knight>(black));
      }
    }
  }
  std::stack<std::unique_ptr<Piece>> table;
  bool blackToMove = specification.sideToMove == model::Black;
  std::pair<std::set<int>, std::shared_ptr<int>> state;
  for (const model::Castling& castling : specification.castlings) {
    for (int index :
         {castling == model::BlackShort || castling == model::BlackLong ? 4
                                                                        : 60,
          castling == model::BlackLong    ? 0
          : castling == model::WhiteLong  ? 56
          : castling == model::BlackShort ? 7
                                          : 63}) {
      int square = 16 * (index % 8) + 7 - index / 8;
      state.first.insert(square);
    }
  }
  if (specification.enPassant.present) {
    int square = 16 * (specification.enPassant.index % 8) + 7 -
                 specification.enPassant.index / 8;
    state.second = std::make_shared<int>(square);
  }
  std::stack<std::pair<std::set<int>, std::shared_ptr<int>>> memory;
  std::unique_ptr<MoveFactory> moveFactory = std::make_unique<MoveFactory>();
  Position position(std::move(board), std::move(box), std::move(table),
                    blackToMove, std::move(state), std::move(memory),
                    std::move(moveFactory));
  int nMoves = specification.operation.opcode == model::ACD
                   ? specification.operation.operand / 2
                   : specification.operation.operand;
  bool halfMove = specification.operation.opcode == model::ACD &&
                  specification.operation.operand % 2 == 1;
  std::unique_ptr<Problem> problem;
  if (specification.operation.opcode == model::ACD) {
    problem = std::make_unique<Perft>(std::move(position), nMoves, halfMove);
  } else {
    problem = std::make_unique<MateSearch>(std::move(position), nMoves);
  }
  AnalysisOptions analysisOptions;
  DisplayOptions displayOptions;
  tasks.push_back({std::move(problem), analysisOptions, displayOptions});
}

std::string translateTerm(
    const std::string& term, int inputLanguage, int outputLanguage,
    const std::vector<std::vector<std::string>>& translations) {
  int iInput = inputLanguage - 1;
  std::vector<std::vector<std::string>>::const_iterator iWords = std::find_if(
      translations.cbegin(), translations.cend(),
      [iInput, &term](const std::vector<std::string>& words) {
        const std::string& word = words.at(iInput);
        return word.size() == term.size() &&
               std::equal(word.cbegin(), word.cend(), term.cbegin(),
                          [](unsigned char symbol1, unsigned char symbol2) {
                            return std::tolower(symbol1) ==
                                   std::tolower(symbol2);
                          });
      });
  if (iWords != translations.cend()) {
    int iOutput = outputLanguage - 1;
    return iWords->at(iOutput);
  }
  return "";
}
std::string translatePattern(
    const std::string& pattern, int inputLanguage, int outputLanguage,
    const std::vector<std::vector<std::string>>& translations) {
  std::vector<std::string> values;
  std::vector<int> iLanguages = {inputLanguage - 1, outputLanguage - 1};
  std::transform(
      iLanguages.cbegin(), iLanguages.cend(), std::back_inserter(values),
      [&translations](int iLanguage) {
        return std::accumulate(
            translations.cbegin(), translations.cend(), std::string(),
            [iLanguage](const std::string& value,
                        const std::vector<std::string>& words) {
              const std::string& word = words.at(iLanguage);
              return value +
                     std::accumulate(
                         word.cbegin(), word.cend(),
                         value.empty() ? std::string() : std::string("|"),
                         [](const std::string& part, unsigned char symbol) {
                           return part + std::string("[")
                                             .append(1, std::toupper(symbol))
                                             .append(1, std::tolower(symbol))
                                             .append("]");
                         });
            });
      });
  if (pattern == values.front()) {
    return values.back();
  }
  return "";
}
std::vector<std::vector<std::string>>& popeyeKeywords() {
  static std::vector<std::vector<std::string>> translations = {
      {"White", "Blanc", "Weiss"},
      {"Black", "Noir", "Schwarz"},
      {"BeginProblem", "DebutProbleme", "Anfangproblem"},
      {"EndProblem", "FinProbleme", "Endeproblem"},
      {"NextProblem", "ASuivre", "WeiteresProblem"},
      {"Remark", "Remarque", "Bemerkung"},
      {"Condition", "Condition", "Bedingung"},
      {"Option", "Option", "Option"},
      {"Stipulation", "Enonce", "Forderung"},
      {"Pieces", "Pieces", "Steine"},
      {"Circe", "Circe", "Circe"},
      {"Try", "Essais", "Verfuehrung"},
      {"Defence", "Defense", "Widerlegung"},
      {"SetPlay", "Apparent", "Satzspiel"},
      {"NullMoves", "CoupsVides", "NullZuege"},
      {"WhiteToPlay", "ApparentSeul", "WeissBeginnt"},
      {"Variation", "Variantes", "Varianten"},
      {"MoveNumbers", "Trace", "Zugnummern"},
      {"NoThreat", "SansMenace", "OhneDrohung"},
      {"EnPassant", "EnPassant", "EnPassant"},
      {"NoBoard", "SansEchiquier", "OhneBrett"},
      {"NoShortVariations", "SansVariantesCourtes", "OhneKurzVarianten"},
      {"HalfDuplex", "DemiDuplex", "HalbDuplex"},
      {"NoCastling", "SansRoquer", "KeineRochade"}};
  return translations;
}
std::vector<std::vector<std::string>>& pieceTypeCodes() {
  static std::vector<std::vector<std::string>> translations = {
      {King::code(Piece::ENGLISH), King::code(Piece::FRENCH),
       King::code(Piece::GERMAN)},
      {Queen::code(Piece::ENGLISH), Queen::code(Piece::FRENCH),
       Queen::code(Piece::GERMAN)},
      {Rook::code(Piece::ENGLISH), Rook::code(Piece::FRENCH),
       Rook::code(Piece::GERMAN)},
      {Bishop::code(Piece::ENGLISH), Bishop::code(Piece::FRENCH),
       Bishop::code(Piece::GERMAN)},
      {Knight::code(Piece::ENGLISH), Knight::code(Piece::FRENCH),
       Knight::code(Piece::GERMAN)},
      {Pawn::code(Piece::ENGLISH), Pawn::code(Piece::FRENCH),
       Pawn::code(Piece::GERMAN)},
      {Grasshopper::code(Piece::ENGLISH), Grasshopper::code(Piece::FRENCH),
       Grasshopper::code(Piece::GERMAN)},
      {Nightrider::code(Piece::ENGLISH), Nightrider::code(Piece::FRENCH),
       Nightrider::code(Piece::GERMAN)},
      {Amazon::code(Piece::ENGLISH), Amazon::code(Piece::FRENCH),
       Amazon::code(Piece::GERMAN)}};
  return translations;
}

}  // namespace moderato
