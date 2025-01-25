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

#include "OrthodoxPieces.h"

#include "MoveFactory.h"

namespace moderato {

King::King(bool black) : Piece(black) {}
Queen::Queen(bool black) : Piece(black) {}
Rook::Rook(bool black) : Piece(black) {}
Bishop::Bishop(bool black) : Piece(black) {}
Knight::Knight(bool black) : Piece(black) {}
Pawn::Pawn(bool black) : Piece(black) {}

bool King::isBlack() const { return Piece::isBlack(); }
bool Queen::isBlack() const { return Piece::isBlack(); }
bool Rook::isBlack() const { return Piece::isBlack(); }
bool Bishop::isBlack() const { return Piece::isBlack(); }
bool Knight::isBlack() const { return Piece::isBlack(); }

bool King::isRoyal() const { return true; }

bool King::isCastling() const { return true; }
bool Rook::isCastling() const { return true; }

int King::findRebirthSquare(
    const std::array<std::unique_ptr<Piece>, 128>& board, int square) const {
  return black_ ? 71 : 64;
}
int Queen::findRebirthSquare(
    const std::array<std::unique_ptr<Piece>, 128>& board, int square) const {
  return black_ ? 55 : 48;
}
int Rook::findRebirthSquare(
    const std::array<std::unique_ptr<Piece>, 128>& board, int square) const {
  return (square / 16 + square % 16) % 2 ? (black_ ? 7 : 112)
                                         : (black_ ? 119 : 0);
}
int Bishop::findRebirthSquare(
    const std::array<std::unique_ptr<Piece>, 128>& board, int square) const {
  return (square / 16 + square % 16) % 2 ? (black_ ? 39 : 80)
                                         : (black_ ? 87 : 32);
}
int Knight::findRebirthSquare(
    const std::array<std::unique_ptr<Piece>, 128>& board, int square) const {
  return (square / 16 + square % 16) % 2 ? (black_ ? 103 : 16)
                                         : (black_ ? 23 : 96);
}
int Pawn::findRebirthSquare(
    const std::array<std::unique_ptr<Piece>, 128>& board, int square) const {
  return (square / 16) * 16 + (black_ ? 6 : 1);
}

std::vector<int>& King::leaps(
    const std::array<std::unique_ptr<Piece>, 128>& board) {
  static std::vector<int> leaps = {-17, -16, -15, -1, 1, 15, 16, 17};
  return leaps;
}
const std::vector<int>& King::getLeaps(
    const std::array<std::unique_ptr<Piece>, 128>& board) const {
  return leaps(board);
}
bool King::generateMoves(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    const std::pair<std::set<int>, std::shared_ptr<int>>& state, int origin,
    const MoveFactory& moveFactory,
    std::vector<std::shared_ptr<Move>>& moves) const {
  if (!Leaper::generateMoves(board, origin, moveFactory, moves)) {
    return false;
  }
  const std::set<int>& castlings = state.first;
  if (origin == (black_ ? 71 : 64) && castlings.count(origin)) {
    for (int direction : {-16, 16}) {
      int distance = 1;
      while (true) {
        int origin2 = origin + distance * direction;
        if (!(origin2 & 136)) {
          if (castlings.count(origin2)) {
            int target = origin + 2 * direction;
            int target2 = origin + direction;
            if (direction > 0) {
              moveFactory.generateShortCastling(board, origin, target, origin2,
                                                target2, moves);
            } else {
              moveFactory.generateLongCastling(board, origin, target, origin2,
                                               target2, moves);
            }
            break;
          } else if (board.at(origin2)) {
            break;
          } else {
            distance++;
          }
        } else {
          break;
        }
      }
    }
  }
  return true;
}
bool King::generateMoves(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    const std::pair<std::set<int>, std::shared_ptr<int>>& state,
    int origin) const {
  return Leaper::generateMoves(board, origin);
}

std::vector<int>& Queen::rides(
    const std::array<std::unique_ptr<Piece>, 128>& board) {
  static std::vector<int> rides = {-17, -16, -15, -1, 1, 15, 16, 17};
  return rides;
}
const std::vector<int>& Queen::getRides(
    const std::array<std::unique_ptr<Piece>, 128>& board) const {
  return rides(board);
}
bool Queen::generateMoves(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    const std::pair<std::set<int>, std::shared_ptr<int>>& state, int origin,
    const MoveFactory& moveFactory,
    std::vector<std::shared_ptr<Move>>& moves) const {
  return Rider::generateMoves(board, origin, moveFactory, moves);
}
bool Queen::generateMoves(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    const std::pair<std::set<int>, std::shared_ptr<int>>& state,
    int origin) const {
  return Rider::generateMoves(board, origin);
}

std::vector<int>& Rook::rides(
    const std::array<std::unique_ptr<Piece>, 128>& board) {
  static std::vector<int> rides = {-16, -1, 1, 16};
  return rides;
}
const std::vector<int>& Rook::getRides(
    const std::array<std::unique_ptr<Piece>, 128>& board) const {
  return rides(board);
}
bool Rook::generateMoves(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    const std::pair<std::set<int>, std::shared_ptr<int>>& state, int origin,
    const MoveFactory& moveFactory,
    std::vector<std::shared_ptr<Move>>& moves) const {
  return Rider::generateMoves(board, origin, moveFactory, moves);
}
bool Rook::generateMoves(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    const std::pair<std::set<int>, std::shared_ptr<int>>& state,
    int origin) const {
  return Rider::generateMoves(board, origin);
}

std::vector<int>& Bishop::rides(
    const std::array<std::unique_ptr<Piece>, 128>& board) {
  static std::vector<int> rides = {-17, -15, 15, 17};
  return rides;
}
const std::vector<int>& Bishop::getRides(
    const std::array<std::unique_ptr<Piece>, 128>& board) const {
  return rides(board);
}
bool Bishop::generateMoves(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    const std::pair<std::set<int>, std::shared_ptr<int>>& state, int origin,
    const MoveFactory& moveFactory,
    std::vector<std::shared_ptr<Move>>& moves) const {
  return Rider::generateMoves(board, origin, moveFactory, moves);
}
bool Bishop::generateMoves(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    const std::pair<std::set<int>, std::shared_ptr<int>>& state,
    int origin) const {
  return Rider::generateMoves(board, origin);
}

std::vector<int>& Knight::leaps(
    const std::array<std::unique_ptr<Piece>, 128>& board) {
  static std::vector<int> leaps = {-33, -31, -18, -14, 14, 18, 31, 33};
  return leaps;
}
const std::vector<int>& Knight::getLeaps(
    const std::array<std::unique_ptr<Piece>, 128>& board) const {
  return leaps(board);
}
bool Knight::generateMoves(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    const std::pair<std::set<int>, std::shared_ptr<int>>& state, int origin,
    const MoveFactory& moveFactory,
    std::vector<std::shared_ptr<Move>>& moves) const {
  return Leaper::generateMoves(board, origin, moveFactory, moves);
}
bool Knight::generateMoves(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    const std::pair<std::set<int>, std::shared_ptr<int>>& state,
    int origin) const {
  return Leaper::generateMoves(board, origin);
}

bool Pawn::generateMoves(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    const std::pair<std::set<int>, std::shared_ptr<int>>& state, int origin,
    const MoveFactory& moveFactory,
    std::vector<std::shared_ptr<Move>>& moves) const {
  int directions[] = {black_ ? -17 : -15, black_ ? 15 : 17};
  for (int direction : directions) {
    int target = origin + direction;
    if (!(target & 136)) {
      const std::unique_ptr<Piece>& piece = board.at(target);
      if (piece) {
        if (piece->isBlack() != black_) {
          if (piece->isRoyal()) {
            return false;
          }
          if (origin % 16 == (black_ ? 1 : 6)) {
            const std::map<int, std::deque<std::unique_ptr<Piece>>>&
                promotions = box.at(black_);
            for (const std::pair<const int, std::deque<std::unique_ptr<Piece>>>&
                     promotion : promotions) {
              int order = promotion.first;
              moveFactory.generatePromotionCapture(board, box, origin, target,
                                                   black_, order, moves);
            }
          } else {
            moveFactory.generateCapture(board, origin, target, moves);
          }
        }
      } else {
        const std::shared_ptr<int>& enPassant = state.second;
        if (enPassant && target == *enPassant) {
          int stop = target + (black_ ? 1 : -1);
          if (board.at(stop)->isRoyal()) {
            return false;
          }
          moveFactory.generateEnPassant(board, origin, target, stop, moves);
        }
      }
    }
  }
  int direction = black_ ? -1 : 1;
  int target = origin + direction;
  if (!(target & 136)) {
    if (!board.at(target)) {
      if (origin % 16 == (black_ ? 1 : 6)) {
        const std::map<int, std::deque<std::unique_ptr<Piece>>>& promotions =
            box.at(black_);
        for (const std::pair<const int, std::deque<std::unique_ptr<Piece>>>&
                 promotion : promotions) {
          int order = promotion.first;
          moveFactory.generatePromotion(board, box, origin, target, black_,
                                        order, moves);
        }
      } else {
        moveFactory.generateQuietMove(board, origin, target, moves);
        if (origin % 16 == (black_ ? 6 : 1)) {
          target = origin + 2 * direction;
          if (!board.at(target)) {
            int stop = origin + direction;
            moveFactory.generateDoubleStep(board, origin, target, stop, moves);
          }
        }
      }
    }
  }
  return true;
}
bool Pawn::generateMoves(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    const std::pair<std::set<int>, std::shared_ptr<int>>& state,
    int origin) const {
  int directions[] = {black_ ? -17 : -15, black_ ? 15 : 17};
  for (int direction : directions) {
    int target = origin + direction;
    if (!(target & 136)) {
      const std::unique_ptr<Piece>& piece = board.at(target);
      if (piece) {
        if (piece->isBlack() != black_) {
          if (piece->isRoyal()) {
            return false;
          }
        }
      } else {
        const std::shared_ptr<int>& enPassant = state.second;
        if (enPassant && target == *enPassant) {
          int stop = target + (black_ ? 1 : -1);
          if (board.at(stop)->isRoyal()) {
            return false;
          }
        }
      }
    }
  }
  return true;
}

std::string King::getCode(int translate) const {
  return translate == GERMAN    ? "K"
         : translate == FRENCH  ? "R"
         : translate == ENGLISH ? "K"
                                : "K";
}
std::string Queen::getCode(int translate) const {
  return translate == GERMAN    ? "D"
         : translate == FRENCH  ? "D"
         : translate == ENGLISH ? "Q"
                                : "Q";
}
std::string Rook::getCode(int translate) const {
  return translate == GERMAN    ? "T"
         : translate == FRENCH  ? "T"
         : translate == ENGLISH ? "R"
                                : "R";
}
std::string Bishop::getCode(int translate) const {
  return translate == GERMAN    ? "L"
         : translate == FRENCH  ? "F"
         : translate == ENGLISH ? "B"
                                : "B";
}
std::string Knight::getCode(int translate) const {
  return translate == GERMAN    ? "S"
         : translate == FRENCH  ? "C"
         : translate == ENGLISH ? "S"
                                : "N";
}
std::string Pawn::getCode(int translate) const { return ""; }

std::string King::getName() const { return "King"; }
std::string Queen::getName() const { return "Queen"; }
std::string Rook::getName() const { return "Rook"; }
std::string Bishop::getName() const { return "Bishop"; }
std::string Knight::getName() const { return "Knight"; }
std::string Pawn::getName() const { return "Pawn"; }

}  // namespace moderato
