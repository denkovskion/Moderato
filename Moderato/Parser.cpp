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
enum AntiCirce { Calvet = 1, Cheylan };
struct Conditions {
  bool circe;
  bool noCapture;
  AntiCirce antiCirce;
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
Task convertProblem(const popeye::Problem& specification, int inputLanguage);
std::unique_ptr<Piece> convertPieceTypeAndColour(
    const popeye::PieceType& pieceType, const popeye::Colour& colour);
bool convertColour(const popeye::Colour& colour);

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
Task convertPosition(const model::Position& specification);
std::unique_ptr<Piece> convertPiece(const model::Piece& piece);
bool convertColour(const model::Colour& colour);

std::vector<std::vector<std::string>> pieceTypeCodes();

bool translateTerm(const std::vector<std::vector<std::string>>& translations,
                   int inputLanguage, int outputLanguage,
                   const std::string& input, std::string& output);
bool translatePattern(const std::vector<std::vector<std::string>>& translations,
                      int inputLanguage, int outputLanguage,
                      const std::string& input, std::string& output);

std::istream& operator>>(std::istream& input, std::vector<Task>& tasks) {
  std::vector<std::string> transitions = {"Popeye", "1"};
  int inputLanguage = 0;
  std::vector<popeye::Problem> problems;
  popeye::Problem problem = {};
  std::vector<model::Position> positions;
  model::Position position = {};
  bool hasNextLine = true;
  for (std::string line; hasNextLine && std::getline(input, line);) {
    std::istringstream tokenInput(line);
    if (transitions.empty()) {
      transitions = {"1"};
    }
    bool hasNextToken = true;
    for (std::string token; hasNextToken && tokenInput >> token;) {
      std::vector<std::string> steps(transitions.cbegin(), transitions.cend());
      if (std::none_of(
              steps.cbegin(), steps.cend(), [&](const std::string& transition) {
                if (transition == "Popeye") {
                  int languages[] = {Piece::ENGLISH, Piece::FRENCH,
                                     Piece::GERMAN};
                  const int* iLanguage = std::find_if(
                      std::cbegin(languages), std::cend(languages),
                      [&token](int language) {
                        std::string directive;
                        if (translateTerm({{"BeginProblem", "DebutProbleme",
                                            "Anfangproblem"}},
                                          language, Piece::ENGLISH, token,
                                          directive)) {
                          if (!(directive == "BeginProblem")) {
                            throw directive;
                          }
                          return true;
                        }
                        return false;
                      });
                  if (iLanguage != std::cend(languages)) {
                    inputLanguage = *iLanguage;
                    transitions = {"Problem"};
                    return true;
                  }
                } else if (transition == "Problem") {
                  std::string command;
                  if (translateTerm({{"Remark", "Remarque", "Bemerkung"},
                                     {"Condition", "Condition", "Bedingung"},
                                     {"Option", "Option", "Option"},
                                     {"Stipulation", "Enonce", "Forderung"},
                                     {"Pieces", "Pieces", "Steine"}},
                                    inputLanguage, Piece::ENGLISH, token,
                                    command)) {
                    if (command == "Condition" || command == "Option" ||
                        command == "Stipulation" || command == "Pieces") {
                      transitions = {command};
                    } else if (command == "Remark") {
                      hasNextToken = false;
                      transitions = {"Problem"};
                    } else {
                      throw command;
                    }
                    return true;
                  }
                  std::string directive;
                  if (translateTerm(
                          {{"EndProblem", "FinProbleme", "Endeproblem"},
                           {"NextProblem", "ASuivre", "WeiteresProblem"}},
                          inputLanguage, Piece::ENGLISH, token, directive)) {
                    if (directive == "NextProblem") {
                      transitions = {"Problem"};
                    } else if (directive == "EndProblem") {
                      hasNextToken = false;
                      hasNextLine = false;
                      transitions = {};
                    } else {
                      throw directive;
                    }
                    problems.push_back(problem);
                    problem = {};
                    return true;
                  }
                } else if (transition == "Condition") {
                  std::string condition;
                  if (translateTerm({{"Circe", "Circe", "Circe"},
                                     {"NoCapture", "SansPrises", "Ohneschlag"},
                                     {"AntiCirce", "AntiCirce", "AntiCirce"}},
                                    inputLanguage, Piece::ENGLISH, token,
                                    condition)) {
                    if (condition == "AntiCirce") {
                      problem.conditions.antiCirce = popeye::Calvet;
                      transitions = {"AntiCirce", "Condition", "Problem"};
                    } else {
                      (condition == "Circe"       ? problem.conditions.circe
                       : condition == "NoCapture" ? problem.conditions.noCapture
                                                  : throw condition) = true;
                      transitions = {"Condition", "Problem"};
                    }
                    return true;
                  }
                } else if (transition == "AntiCirce") {
                  std::string antiCirce;
                  if (translateTerm({{"Calvet", "Calvet", "Calvet"},
                                     {"Cheylan", "Cheylan", "Cheylan"}},
                                    inputLanguage, Piece::ENGLISH, token,
                                    antiCirce)) {
                    problem.conditions.antiCirce =
                        (antiCirce == "Calvet"    ? popeye::Calvet
                         : antiCirce == "Cheylan" ? popeye::Cheylan
                                                  : throw antiCirce);
                    transitions = {"Condition", "Problem"};
                    return true;
                  }
                } else if (transition == "Option") {
                  std::string option;
                  if (translateTerm(
                          {{"Try", "Essais", "Verfuehrung"},
                           {"Defence", "Defense", "Widerlegung"},
                           {"SetPlay", "Apparent", "Satzspiel"},
                           {"NullMoves", "CoupsVides", "NullZuege"},
                           {"WhiteToPlay", "ApparentSeul", "WeissBeginnt"},
                           {"Variation", "Variantes", "Varianten"},
                           {"MoveNumbers", "Trace", "Zugnummern"},
                           {"NoThreat", "SansMenace", "OhneDrohung"},
                           {"EnPassant", "EnPassant", "EnPassant"},
                           {"NoBoard", "SansEchiquier", "OhneBrett"},
                           {"NoShortVariations", "SansVariantesCourtes",
                            "OhneKurzVarianten"},
                           {"HalfDuplex", "DemiDuplex", "HalbDuplex"},
                           {"NoCastling", "SansRoquer", "KeineRochade"}},
                          inputLanguage, Piece::ENGLISH, token, option)) {
                    if (option == "Defence" || option == "EnPassant" ||
                        option == "NoCastling") {
                      transitions = {option};
                    } else {
                      (option == "Try"           ? problem.options.tri
                       : option == "SetPlay"     ? problem.options.setPlay
                       : option == "NullMoves"   ? problem.options.nullMoves
                       : option == "WhiteToPlay" ? problem.options.whiteToPlay
                       : option == "Variation"   ? problem.options.variation
                       : option == "MoveNumbers" ? problem.options.moveNumbers
                       : option == "NoThreat"    ? problem.options.noThreat
                       : option == "NoBoard"     ? problem.options.noBoard
                       : option == "NoShortVariations"
                           ? problem.options.noShortVariations
                       : option == "HalfDuplex" ? problem.options.halfDuplex
                                                : throw option) = true;
                      transitions = {"Option", "Problem"};
                    }
                    return true;
                  }
                } else if (transition == "Defence") {
                  if (std::regex_match(token, std::regex("[1-9]\\d*"))) {
                    int defence = std::stoi(token);
                    problem.options.defence = defence;
                    transitions = {"Option", "Problem"};
                    return true;
                  }
                } else if (transition == "EnPassant" ||
                           transition == "NoCastling") {
                  if (std::regex_match(token,
                                       std::regex(transition == "EnPassant"
                                                      ? "([A-Ha-h][1-8]){1,2}"
                                                  : transition == "NoCastling"
                                                      ? "([A-Ha-h][1-8])+"
                                                      : throw transition))) {
                    std::string squaresSubsequence = token;
                    for (std::smatch squareMatch;
                         std::regex_search(squaresSubsequence, squareMatch,
                                           std::regex("[A-Ha-h][1-8]"));) {
                      popeye::File file = static_cast<popeye::File>(
                          std::islower(squareMatch.str()[0])
                              ? squareMatch.str()[0] - 'a'
                              : squareMatch.str()[0] - 'A');
                      popeye::Rank rank =
                          static_cast<popeye::Rank>(squareMatch.str()[1] - '1');
                      (transition == "EnPassant"    ? problem.options.enPassant
                       : transition == "NoCastling" ? problem.options.noCastling
                                                    : throw transition)
                          .push_back({file, rank});
                      squaresSubsequence = squareMatch.suffix();
                    };
                    transitions = {"Option", "Problem"};
                    return true;
                  }
                } else if (transition == "Stipulation") {
                  std::string stipulationPattern =
                      "(|[Hh]|[Ss])(#|=)([1-9]\\d*)";
                  if (std::regex_match(token, std::regex(stipulationPattern))) {
                    std::smatch stipulationMatch;
                    std::regex_match(token, stipulationMatch,
                                     std::regex(stipulationPattern));
                    popeye::StipulationType stipulationType =
                        std::regex_match(stipulationMatch[1].str(),
                                         std::regex("[Hh]"))
                            ? popeye::Help
                        : std::regex_match(stipulationMatch[1].str(),
                                           std::regex("[Ss]"))
                            ? popeye::Self
                            : popeye::Direct;
                    popeye::Goal goal = stipulationMatch[2] == "="
                                            ? popeye::Stalemate
                                            : popeye::Mate;
                    int nMoves = std::stoi(stipulationMatch[3]);
                    problem.stipulation = {stipulationType, goal, nMoves};
                    transitions = {"Problem"};
                    return true;
                  }
                } else if (transition == "Pieces") {
                  std::string colour;
                  if (translateTerm({{"White", "Blanc", "Weiss"},
                                     {"Black", "Noir", "Schwarz"}},
                                    inputLanguage, Piece::ENGLISH, token,
                                    colour)) {
                    if (colour == "White" || colour == "Black") {
                      transitions = {colour};
                    } else {
                      throw colour;
                    }
                    return true;
                  }
                } else if (transition == "White" || transition == "Black") {
                  std::string pieceTypePattern;
                  if (translatePattern(
                          pieceTypeCodes(), Piece::ENGLISH, inputLanguage,
                          "[Kk]|[Qq]|[Rr]|[Bb]|[Ss]|[Pp]|[Gg]|[Nn]|[Aa][Mm]",
                          pieceTypePattern)) {
                    std::string piecePattern =
                        "(" + pieceTypePattern + ")(([A-Ha-h][1-8])+)";
                    if (std::regex_match(token, std::regex(piecePattern))) {
                      popeye::Colour colour =
                          transition == "White"   ? popeye::White
                          : transition == "Black" ? popeye::Black
                                                  : throw transition;
                      std::smatch pieceMatch;
                      std::regex_match(token, pieceMatch,
                                       std::regex(piecePattern));
                      std::string pieceTypeCode;
                      if (translateTerm(pieceTypeCodes(), inputLanguage,
                                        Piece::ENGLISH, pieceMatch[1].str(),
                                        pieceTypeCode)) {
                        popeye::PieceType pieceType =
                            pieceTypeCode == "K"    ? popeye::King
                            : pieceTypeCode == "Q"  ? popeye::Queen
                            : pieceTypeCode == "R"  ? popeye::Rook
                            : pieceTypeCode == "B"  ? popeye::Bishop
                            : pieceTypeCode == "S"  ? popeye::Knight
                            : pieceTypeCode == "P"  ? popeye::Pawn
                            : pieceTypeCode == "G"  ? popeye::Grasshopper
                            : pieceTypeCode == "N"  ? popeye::Nightrider
                            : pieceTypeCode == "AM" ? popeye::Amazon
                                                    : throw pieceTypeCode;
                        std::string squaresSubsequence = pieceMatch[2];
                        for (std::smatch squareMatch;
                             std::regex_search(squaresSubsequence, squareMatch,
                                               std::regex("[A-Ha-h][1-8]"));) {
                          popeye::File file = static_cast<popeye::File>(
                              std::islower(squareMatch.str()[0])
                                  ? squareMatch.str()[0] - 'a'
                                  : squareMatch.str()[0] - 'A');
                          popeye::Rank rank = static_cast<popeye::Rank>(
                              squareMatch.str()[1] - '1');
                          popeye::Square square = {file, rank};
                          problem.pieces.push_back({square, pieceType, colour});
                          squaresSubsequence = squareMatch.suffix();
                        };
                        transitions = {transition, "Pieces", "Problem"};
                        return true;
                      } else {
                        throw pieceMatch[1].str();
                      }
                    }
                  } else {
                    throw "[Kk]|[Qq]|[Rr]|[Bb]|[Ss]|[Pp]|[Gg]|[Nn]|[Aa][Mm]";
                  }
                } else if (transition == "1") {
                  if (std::regex_match(
                          std::accumulate(
                              token.cbegin(), token.cend(), std::string(),
                              [](const std::string& placement,
                                 unsigned char symbol) {
                                return placement +
                                       (std::isdigit(symbol)
                                            ? std::string(symbol - '0', '1')
                                            : std::string(1, symbol));
                              }),
                          std::regex(
                              "([KQRBNPkqrbnp1]{8}/){7}[KQRBNPkqrbnp1]{8}")) &&
                      !std::regex_search(token, std::regex("\\d{2,}"))) {
                    int index = 0;
                    for (unsigned char symbol : token) {
                      if (std::isalpha(symbol)) {
                        model::Piece piece =
                            symbol == 'K'   ? model::WhiteKing
                            : symbol == 'Q' ? model::WhiteQueen
                            : symbol == 'R' ? model::WhiteRook
                            : symbol == 'B' ? model::WhiteBishop
                            : symbol == 'N' ? model::WhiteKnight
                            : symbol == 'P' ? model::WhitePawn
                            : symbol == 'q' ? model::BlackQueen
                            : symbol == 'r' ? model::BlackRook
                            : symbol == 'b' ? model::BlackBishop
                            : symbol == 'n' ? model::BlackKnight
                            : symbol == 'p' ? model::BlackPawn
                            : symbol == 'k' ? model::BlackKing
                                            : throw symbol;
                        position.board.at(index) = piece;
                        index++;
                      } else if (std::isdigit(symbol)) {
                        index += symbol - '0';
                      }
                    }
                    hasNextLine = false;
                    transitions = {"2"};
                    return true;
                  }
                } else if (transition == "2") {
                  if (std::regex_match(token, std::regex("[wb]"))) {
                    model::Colour sideToMove = token == "w"   ? model::White
                                               : token == "b" ? model::Black
                                                              : throw token;
                    position.sideToMove = sideToMove;
                    transitions = {"3"};
                    return true;
                  }
                } else if (transition == "3") {
                  if (std::regex_match(token, std::regex("\\bK?Q?k?q?|-"))) {
                    if (!(token == "-")) {
                      for (unsigned char symbol : token) {
                        model::Castling castling =
                            symbol == 'K'   ? model::WhiteShort
                            : symbol == 'Q' ? model::WhiteLong
                            : symbol == 'k' ? model::BlackShort
                            : symbol == 'q' ? model::BlackLong
                                            : throw symbol;
                        position.castlings.insert(castling);
                      }
                    }
                    transitions = {"4"};
                    return true;
                  }
                } else if (transition == "4") {
                  if (std::regex_match(token, std::regex("[a-h][36]|-"))) {
                    if (!(token == "-")) {
                      int file = token.at(0) - 'a' + 1;
                      int rank = token.at(1) - '1' + 1;
                      position.enPassant = {(8 - rank) * 8 + file - 1, true};
                    }
                    transitions = {"5"};
                    return true;
                  }
                } else if (transition == "5") {
                  if (std::regex_match(token, std::regex("acd|dm"))) {
                    transitions = {token};
                    return true;
                  }
                } else if (transition == "acd" || transition == "dm") {
                  if (std::regex_match(
                          token,
                          std::regex(transition == "acd" ? "(0|[1-9]\\d*);"
                                     : transition == "dm"
                                         ? "[1-9]\\d*;"
                                         : throw transition))) {
                    model::Opcode opcode = transition == "acd" ? model::ACD
                                           : transition == "dm"
                                               ? model::DM
                                               : throw transition;
                    int operand = std::stoi(token);
                    position.operation = {opcode, operand};
                    hasNextLine = true;
                    transitions = {};
                    positions.push_back(position);
                    position = {};
                    return true;
                  }
                } else {
                  throw transition;
                }
                return false;
              })) {
        throw std::invalid_argument("Parse failure (invalid token: \"" + token +
                                    "\").");
      }
    }
  }
  if (!(transitions.empty() ||
        std::find(transitions.cbegin(), transitions.cend(), "1") !=
            transitions.cend())) {
    throw std::invalid_argument("Parse failure (incomplete input).");
  }
  std::transform(problems.cbegin(), problems.cend(), std::back_inserter(tasks),
                 [inputLanguage](const popeye::Problem& specification) {
                   validateProblem(specification);
                   verifyProblem(specification);
                   return convertProblem(specification, inputLanguage);
                 });
  std::transform(positions.cbegin(), positions.cend(),
                 std::back_inserter(tasks),
                 [](const model::Position& specification) {
                   validatePosition(specification);
                   return convertPosition(specification);
                 });
  return input;
}

void validateProblem(const popeye::Problem& specification) {
  if (!specification.stipulation.nMoves) {
    throw std::invalid_argument(
        "Problem conversion failure (missing stipulation).");
  }
  for (const popeye::Colour& colour : {popeye::White, popeye::Black}) {
    bool present = false;
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
        if (present) {
          throw std::invalid_argument(
              "Problem conversion failure (too many " +
              std::string(colour == popeye::White   ? "white"
                          : colour == popeye::Black ? "black"
                                                    : throw colour) +
              " kings).");
        }
        present = true;
      }
      iPiece++;
    }
    if (!present) {
      throw std::invalid_argument("Problem conversion failure (missing " +
                                  std::string(colour == popeye::White ? "white"
                                              : colour == popeye::Black
                                                  ? "black"
                                                  : throw colour) +
                                  " king).");
    }
  }
}
void verifyProblem(const popeye::Problem& specification) {
  if (specification.conditions.circe) {
    if (specification.conditions.noCapture) {
      throw std::domain_error(
          "Task creation failure (not accepted condition: Circe w/ "
          "NoCapture).");
    } else if (specification.conditions.antiCirce) {
      throw std::domain_error(
          "Task creation failure (not accepted condition: Circe w/ "
          "AntiCirce).");
    }
  } else if (specification.conditions.noCapture &&
             specification.conditions.antiCirce) {
    throw std::domain_error(
        "Task creation failure (not accepted condition: NoCapture w/ "
        "AntiCirce).");
  }
  std::vector<popeye::Square>::const_iterator jSquare = std::find_if_not(
      specification.options.noCastling.cbegin(),
      specification.options.noCastling.cend(),
      [](const popeye::Square& square) {
        return (square.file == popeye::_e || square.file == popeye::_a ||
                square.file == popeye::_h) &&
               (square.rank == popeye::_1 || square.rank == popeye::_8);
      });
  if (jSquare != specification.options.noCastling.cend()) {
    throw std::domain_error(
        "Task creation failure (not accepted option: nocastling " +
        std::string()
            .append(1, 'a' + jSquare->file)
            .append(1, '1' + jSquare->rank) +
        ").");
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
        (specification.stipulation.stipulationType == popeye::Direct ||
                 specification.stipulation.stipulationType == popeye::Self
             ? specification.options.halfDuplex
         : specification.stipulation.stipulationType == popeye::Help
             ? specification.options.halfDuplex ==
                   specification.options.whiteToPlay
             : throw specification.stipulation.stipulationType)
            ? popeye::Black
            : popeye::White;
    if (!(square.rank == (sideToMove == popeye::White   ? popeye::_6
                          : sideToMove == popeye::Black ? popeye::_3
                                                        : throw sideToMove) &&
          std::none_of(pieces.cbegin(), pieces.cend(),
                       [&square, &sideToMove](const popeye::Piece& piece) {
                         return piece.square.file == square.file &&
                                (piece.square.rank == square.rank ||
                                 piece.square.rank ==
                                     (sideToMove == popeye::White ? popeye::_7
                                      : sideToMove == popeye::Black
                                          ? popeye::_2
                                          : throw sideToMove));
                       }) &&
          std::any_of(pieces.cbegin(), pieces.cend(),
                      [&square, &sideToMove](const popeye::Piece& piece) {
                        return piece.square.file == square.file &&
                               piece.square.rank ==
                                   (sideToMove == popeye::White ? popeye::_5
                                    : sideToMove == popeye::Black
                                        ? popeye::_4
                                        : throw sideToMove) &&
                               piece.colour == (sideToMove == popeye::White
                                                    ? popeye::Black
                                                : sideToMove == popeye::Black
                                                    ? popeye::White
                                                    : throw sideToMove) &&
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
Task convertProblem(const popeye::Problem& specification, int inputLanguage) {
  std::array<std::unique_ptr<Piece>, 128> board;
  for (const popeye::Piece& piece : specification.pieces) {
    int square = 16 * piece.square.file + piece.square.rank;
    board.at(square) = convertPieceTypeAndColour(piece.pieceType, piece.colour);
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
        (specification.stipulation.stipulationType == popeye::Direct
             ? (colour == popeye::White   ? specification.options.halfDuplex
                : colour == popeye::Black ? !specification.options.halfDuplex
                                          : throw colour)
         : specification.stipulation.stipulationType == popeye::Help
             ? specification.options.whiteToPlay &&
                   (colour == popeye::White ? specification.options.halfDuplex
                    : colour == popeye::Black
                        ? !specification.options.halfDuplex
                        : throw colour)
         : specification.stipulation.stipulationType == popeye::Self
             ? false
             : throw specification.stipulation.stipulationType)
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
      bool black = convertColour(colour);
      for (int promotionNo = maxPromotion; promotionNo--;) {
        int order = 0;
        for (const popeye::PieceType& promotionType : promotionTypes) {
          box[black][++order].push_front(
              convertPieceTypeAndColour(promotionType, colour));
        }
      }
    }
  }
  std::stack<std::unique_ptr<Piece>> table;
  bool blackToMove =
      specification.stipulation.stipulationType == popeye::Direct ||
              specification.stipulation.stipulationType == popeye::Self
          ? specification.options.halfDuplex
      : specification.stipulation.stipulationType == popeye::Help
          ? specification.options.halfDuplex ==
                specification.options.whiteToPlay
          : throw specification.stipulation.stipulationType;
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
  } else if (specification.conditions.noCapture) {
    moveFactory = std::make_unique<NoCaptureMoveFactory>();
  } else if (specification.conditions.antiCirce) {
    if (specification.conditions.antiCirce == popeye::Calvet) {
      moveFactory = std::make_unique<AntiCirceMoveFactory>(true);
    } else if (specification.conditions.antiCirce == popeye::Cheylan) {
      moveFactory = std::make_unique<AntiCirceMoveFactory>(false);
    } else {
      throw specification.conditions.antiCirce;
    }
  } else {
    moveFactory = std::make_unique<MoveFactory>();
  }
  Position position(std::move(board), std::move(box), std::move(table),
                    blackToMove, std::move(state), std::move(memory),
                    std::move(moveFactory));
  bool stalemate = specification.stipulation.goal == popeye::Mate ? false
                   : specification.stipulation.goal == popeye::Stalemate
                       ? true
                       : throw specification.stipulation.goal;
  int nMoves =
      (specification.stipulation.stipulationType == popeye::Direct ||
               specification.stipulation.stipulationType == popeye::Self
           ? false
       : specification.stipulation.stipulationType == popeye::Help
           ? specification.options.whiteToPlay
           : throw specification.stipulation.stipulationType)
          ? specification.stipulation.nMoves - 1
          : specification.stipulation.nMoves;
  bool halfMove =
      specification.stipulation.stipulationType == popeye::Direct ||
              specification.stipulation.stipulationType == popeye::Self
          ? false
      : specification.stipulation.stipulationType == popeye::Help
          ? specification.options.whiteToPlay
          : throw specification.stipulation.stipulationType;
  std::unique_ptr<Problem> problem;
  if (specification.stipulation.stipulationType == popeye::Direct) {
    problem =
        std::make_unique<Directmate>(std::move(position), stalemate, nMoves);
  } else if (specification.stipulation.stipulationType == popeye::Self) {
    problem =
        std::make_unique<Selfmate>(std::move(position), stalemate, nMoves);
  } else if (specification.stipulation.stipulationType == popeye::Help) {
    problem = std::make_unique<Helpmate>(std::move(position), stalemate, nMoves,
                                         halfMove);
  } else {
    throw specification.stipulation.stipulationType;
  }
  bool setPlay = specification.options.setPlay;
  int nRefutations =
      specification.stipulation.stipulationType == popeye::Direct ||
              specification.stipulation.stipulationType == popeye::Self
          ? std::max(specification.options.defence,
                     specification.options.tri ? 1 : 0)
      : specification.stipulation.stipulationType == popeye::Help
          ? 0
          : throw specification.stipulation.stipulationType;
  bool variations =
      specification.stipulation.stipulationType == popeye::Direct ||
              specification.stipulation.stipulationType == popeye::Self
          ? specification.options.variation
      : specification.stipulation.stipulationType == popeye::Help
          ? false
          : throw specification.stipulation.stipulationType;
  bool threats =
      specification.stipulation.stipulationType == popeye::Direct ||
              specification.stipulation.stipulationType == popeye::Self
          ? specification.options.variation && !specification.options.noThreat
      : specification.stipulation.stipulationType == popeye::Help
          ? false
          : throw specification.stipulation.stipulationType;
  bool shortVariations =
      specification.stipulation.stipulationType == popeye::Direct ||
              specification.stipulation.stipulationType == popeye::Self
          ? specification.options.variation &&
                !specification.options.noShortVariations
      : specification.stipulation.stipulationType == popeye::Help
          ? false
          : throw specification.stipulation.stipulationType;
  bool tempoTries =
      specification.stipulation.stipulationType == popeye::Direct ||
              specification.stipulation.stipulationType == popeye::Self
          ? false
      : specification.stipulation.stipulationType == popeye::Help
          ? specification.options.nullMoves || specification.options.tri
          : throw specification.stipulation.stipulationType;
  AnalysisOptions analysisOptions = {setPlay, nRefutations,    variations,
                                     threats, shortVariations, tempoTries};
  int outputLanguage = inputLanguage;
  bool internalModel = !specification.options.noBoard;
  bool internalProgress = specification.options.moveNumbers;
  DisplayOptions displayOptions = {outputLanguage, internalModel,
                                   internalProgress};
  return {std::move(problem), analysisOptions, displayOptions};
}
std::unique_ptr<Piece> convertPieceTypeAndColour(
    const popeye::PieceType& pieceType, const popeye::Colour& colour) {
  bool black = convertColour(colour);
  switch (pieceType) {
    case popeye::King:
      return std::make_unique<King>(black);
    case popeye::Queen:
      return std::make_unique<Queen>(black);
    case popeye::Rook:
      return std::make_unique<Rook>(black);
    case popeye::Bishop:
      return std::make_unique<Bishop>(black);
    case popeye::Knight:
      return std::make_unique<Knight>(black);
    case popeye::Pawn:
      return std::make_unique<Pawn>(black);
    case popeye::Grasshopper:
      return std::make_unique<Grasshopper>(black);
    case popeye::Nightrider:
      return std::make_unique<Nightrider>(black);
    case popeye::Amazon:
      return std::make_unique<Amazon>(black);
  }
  throw pieceType;
}
bool convertColour(const popeye::Colour& colour) {
  switch (colour) {
    case popeye::White:
      return false;
    case popeye::Black:
      return true;
  }
  throw colour;
}

void validatePosition(const model::Position& specification) {
  model::Piece kings[] = {model::WhiteKing, model::BlackKing};
  if (!std::all_of(std::cbegin(kings), std::cend(kings),
                   [&specification](const model::Piece& king) {
                     return std::count(specification.board.cbegin(),
                                       specification.board.cend(), king) == 1;
                   })) {
    throw std::invalid_argument(
        "Position conversion failure (not accepted number of kings).");
  }
  if (!std::all_of(
          specification.castlings.cbegin(), specification.castlings.cend(),
          [&specification](const model::Castling& castling) {
            return (castling == model::WhiteShort ||
                            castling == model::WhiteLong
                        ? specification.board.at(60) == model::WhiteKing
                    : castling == model::BlackShort ||
                            castling == model::BlackLong
                        ? specification.board.at(4) == model::BlackKing
                        : throw castling) &&
                   (castling == model::WhiteShort
                        ? specification.board.at(63) == model::WhiteRook
                    : castling == model::WhiteLong
                        ? specification.board.at(56) == model::WhiteRook
                    : castling == model::BlackShort
                        ? specification.board.at(7) == model::BlackRook
                    : castling == model::BlackLong
                        ? specification.board.at(0) == model::BlackRook
                        : throw castling);
          })) {
    throw std::invalid_argument(
        "Position conversion failure (not accepted castling rights).");
  }
  if (specification.enPassant.present) {
    if (!(specification.sideToMove == model::White
              ? specification.enPassant.index >= 16 &&
                    specification.enPassant.index <= 23 &&
                    specification.board.at(specification.enPassant.index) ==
                        0 &&
                    specification.board.at(specification.enPassant.index - 8) ==
                        0 &&
                    specification.board.at(specification.enPassant.index + 8) ==
                        model::BlackPawn
          : specification.sideToMove == model::Black
              ? specification.enPassant.index >= 40 &&
                    specification.enPassant.index <= 47 &&
                    specification.board.at(specification.enPassant.index) ==
                        0 &&
                    specification.board.at(specification.enPassant.index + 8) ==
                        0 &&
                    specification.board.at(specification.enPassant.index - 8) ==
                        model::WhitePawn
              : throw specification.sideToMove)) {
      throw std::invalid_argument(
          "Position conversion failure (not accepted en passant square).");
    }
  }
}
Task convertPosition(const model::Position& specification) {
  std::array<std::unique_ptr<Piece>, 128> board;
  for (int index = 0; index < 64; index++) {
    const model::Piece& piece = specification.board.at(index);
    if (piece != 0) {
      int square = 16 * (index % 8) + 7 - index / 8;
      board.at(square) = convertPiece(piece);
    }
  }
  std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>> box;
  for (const model::Colour& colour : {model::White, model::Black}) {
    int maxMove = specification.operation.opcode == model::ACD
                      ? (specification.sideToMove == colour
                             ? (specification.operation.operand + 1) / 2 + 1
                             : specification.operation.operand / 2 + 1)
                  : specification.operation.opcode == model::DM
                      ? (specification.sideToMove == colour
                             ? specification.operation.operand + 1
                             : specification.operation.operand)
                      : throw specification.operation.opcode;
    int nPawns = static_cast<int>(
        std::count(specification.board.cbegin(), specification.board.cend(),
                   colour == model::White   ? model::WhitePawn
                   : colour == model::Black ? model::BlackPawn
                                            : throw colour));
    model::Piece promotions[] = {colour == model::White   ? model::WhiteQueen
                                 : colour == model::Black ? model::BlackQueen
                                                          : throw colour,
                                 colour == model::White   ? model::WhiteRook
                                 : colour == model::Black ? model::BlackRook
                                                          : throw colour,
                                 colour == model::White   ? model::WhiteBishop
                                 : colour == model::Black ? model::BlackBishop
                                                          : throw colour,
                                 colour == model::White   ? model::WhiteKnight
                                 : colour == model::Black ? model::BlackKnight
                                                          : throw colour};
    int maxPromotion = std::min(maxMove, nPawns);
    if (maxPromotion) {
      bool black = convertColour(colour);
      for (int promotionNo = maxPromotion; promotionNo--;) {
        int order = 0;
        for (const model::Piece& promotion : promotions) {
          box[black][++order].push_front(convertPiece(promotion));
        }
      }
    }
  }
  std::stack<std::unique_ptr<Piece>> table;
  bool blackToMove = convertColour(specification.sideToMove);
  std::pair<std::set<int>, std::shared_ptr<int>> state;
  for (const model::Castling& castling : specification.castlings) {
    for (int index :
         {castling == model::WhiteShort || castling == model::WhiteLong ? 60
          : castling == model::BlackShort || castling == model::BlackLong
              ? 4
              : throw castling,
          castling == model::WhiteShort   ? 63
          : castling == model::WhiteLong  ? 56
          : castling == model::BlackShort ? 7
          : castling == model::BlackLong  ? 0
                                          : throw castling}) {
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
               : specification.operation.opcode == model::DM
                   ? specification.operation.operand
                   : throw specification.operation.opcode;
  bool halfMove = specification.operation.opcode == model::ACD
                      ? specification.operation.operand % 2 == 1
                  : specification.operation.opcode == model::DM
                      ? false
                      : throw specification.operation.opcode;
  std::unique_ptr<Problem> problem;
  if (specification.operation.opcode == model::ACD) {
    problem = std::make_unique<Perft>(std::move(position), nMoves, halfMove);
  } else if (specification.operation.opcode == model::DM) {
    problem = std::make_unique<MateSearch>(std::move(position), nMoves);
  } else {
    throw specification.operation.opcode;
  }
  AnalysisOptions analysisOptions;
  DisplayOptions displayOptions;
  return {std::move(problem), analysisOptions, displayOptions};
}
std::unique_ptr<Piece> convertPiece(const model::Piece& piece) {
  switch (piece) {
    case 0:
      return nullptr;
    case model::WhiteKing:
      return std::make_unique<King>(false);
    case model::WhiteQueen:
      return std::make_unique<Queen>(false);
    case model::WhiteRook:
      return std::make_unique<Rook>(false);
    case model::WhiteBishop:
      return std::make_unique<Bishop>(false);
    case model::WhiteKnight:
      return std::make_unique<Knight>(false);
    case model::WhitePawn:
      return std::make_unique<Pawn>(false);
    case model::BlackKing:
      return std::make_unique<King>(true);
    case model::BlackQueen:
      return std::make_unique<Queen>(true);
    case model::BlackRook:
      return std::make_unique<Rook>(true);
    case model::BlackBishop:
      return std::make_unique<Bishop>(true);
    case model::BlackKnight:
      return std::make_unique<Knight>(true);
    case model::BlackPawn:
      return std::make_unique<Pawn>(true);
  }
  throw piece;
}
bool convertColour(const model::Colour& colour) {
  switch (colour) {
    case model::White:
      return false;
    case model::Black:
      return true;
  }
  throw colour;
}

bool translateTerm(const std::vector<std::vector<std::string>>& translations,
                   int inputLanguage, int outputLanguage,
                   const std::string& input, std::string& output) {
  int iInput = inputLanguage - 1;
  std::vector<std::vector<std::string>>::const_iterator iEntry = std::find_if(
      translations.cbegin(), translations.cend(),
      [iInput, &input](const std::vector<std::string>& entry) {
        const std::string& term = entry.at(iInput);
        return term.size() == input.size() &&
               std::equal(term.cbegin(), term.cend(), input.cbegin(),
                          [](unsigned char symbol1, unsigned char symbol2) {
                            return std::tolower(symbol1) ==
                                   std::tolower(symbol2);
                          });
      });
  if (iEntry != translations.cend()) {
    int iOutput = outputLanguage - 1;
    output = iEntry->at(iOutput);
    return true;
  }
  return false;
}
bool translatePattern(const std::vector<std::vector<std::string>>& translations,
                      int inputLanguage, int outputLanguage,
                      const std::string& input, std::string& output) {
  std::vector<std::string> patterns;
  std::vector<int> iLanguages = {inputLanguage - 1};
  if (outputLanguage != inputLanguage) {
    iLanguages.push_back(outputLanguage - 1);
  }
  std::transform(
      iLanguages.cbegin(), iLanguages.cend(), std::back_inserter(patterns),
      [&translations](int iLanguage) {
        return std::accumulate(
            translations.cbegin(), translations.cend(), std::string(),
            [iLanguage](const std::string& pattern,
                        const std::vector<std::string>& entry) {
              const std::string& term = entry.at(iLanguage);
              return pattern +
                     std::accumulate(
                         term.cbegin(), term.cend(),
                         pattern.empty() ? std::string() : std::string("|"),
                         [](const std::string& part, unsigned char symbol) {
                           return part + std::string("[")
                                             .append(1, std::toupper(symbol))
                                             .append(1, std::tolower(symbol))
                                             .append("]");
                         });
            });
      });
  if (input == patterns.front()) {
    output = patterns.back();
    return true;
  }
  return false;
}

std::vector<std::vector<std::string>> pieceTypeCodes() {
  return {{King::code(Piece::ENGLISH), King::code(Piece::FRENCH),
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
}

}  // namespace moderato
