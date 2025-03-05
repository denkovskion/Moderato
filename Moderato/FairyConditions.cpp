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

#include "FairyConditions.h"

#include "FairyMoves.h"

namespace moderato {

void CirceMoveFactory::write(std::ostream& output) const {
  output << "CirceMoveFactory[]";
}
void CirceMoveFactory::generateCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, std::vector<std::shared_ptr<Move>>& moves) const {
  const std::unique_ptr<Piece>& piece = board.at(target);
  int rebirth = piece->findRebirthSquare(board, target);
  if (!board.at(rebirth) || rebirth == origin) {
    if (piece->isCastling()) {
      moves.push_back(
          std::make_shared<CirceCaptureCastling>(origin, target, rebirth));
    } else {
      moves.push_back(std::make_shared<CirceCapture>(origin, target, rebirth));
    }
  } else {
    moves.push_back(std::make_shared<Capture>(origin, target));
  }
}
void CirceMoveFactory::generateEnPassant(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int stop, std::vector<std::shared_ptr<Move>>& moves) const {
  const std::unique_ptr<Piece>& piece = board.at(stop);
  int rebirth = piece->findRebirthSquare(board, stop);
  if (!board.at(rebirth) || rebirth == origin) {
    if (piece->isCastling()) {
      moves.push_back(std::make_shared<CirceEnPassantCastling>(origin, target,
                                                               stop, rebirth));
    } else {
      moves.push_back(
          std::make_shared<CirceEnPassant>(origin, target, stop, rebirth));
    }
  } else {
    moves.push_back(std::make_shared<EnPassant>(origin, target, stop));
  }
}
void CirceMoveFactory::generatePromotionCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    int origin, int target, bool black, int order,
    std::vector<std::shared_ptr<Move>>& moves) const {
  const std::unique_ptr<Piece>& piece = board.at(target);
  int rebirth = piece->findRebirthSquare(board, target);
  if (!board.at(rebirth) || rebirth == origin) {
    if (piece->isCastling()) {
      moves.push_back(std::make_shared<CircePromotionCaptureCastling>(
          origin, target, black, order, rebirth));
    } else {
      moves.push_back(std::make_shared<CircePromotionCapture>(
          origin, target, black, order, rebirth));
    }
  } else {
    moves.push_back(
        std::make_shared<PromotionCapture>(origin, target, black, order));
  }
}

void NoCaptureMoveFactory::write(std::ostream& output) const {
  output << "NoCaptureMoveFactory[]";
}
void NoCaptureMoveFactory::generateCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, std::vector<std::shared_ptr<Move>>& moves) const {}
void NoCaptureMoveFactory::generateEnPassant(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int stop, std::vector<std::shared_ptr<Move>>& moves) const {}
void NoCaptureMoveFactory::generatePromotionCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    int origin, int target, bool black, int order,
    std::vector<std::shared_ptr<Move>>& moves) const {}

}  // namespace moderato
