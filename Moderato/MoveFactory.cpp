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

#include "MoveFactory.h"

#include "MoveTypes.h"

namespace moderato {

MoveFactory::~MoveFactory() {}
void MoveFactory::write(std::ostream& output) const {
  output << "MoveFactory[]";
}
void MoveFactory::generateQuietMove(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, std::vector<std::unique_ptr<Move>>& moves) const {
  moves.push_back(std::make_unique<QuietMove>(origin, target));
}
bool MoveFactory::generateCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, std::vector<std::unique_ptr<Move>>& moves) const {
  if (board.at(target)->isRoyal()) {
    return false;
  }
  moves.push_back(std::make_unique<Capture>(origin, target));
  return true;
}
bool MoveFactory::generateCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target) const {
  return !board.at(target)->isRoyal();
}
void MoveFactory::generateLongCastling(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int origin2, int target2,
    std::vector<std::unique_ptr<Move>>& moves) const {
  moves.push_back(
      std::make_unique<LongCastling>(origin, target, origin2, target2));
}
void MoveFactory::generateShortCastling(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int origin2, int target2,
    std::vector<std::unique_ptr<Move>>& moves) const {
  moves.push_back(
      std::make_unique<ShortCastling>(origin, target, origin2, target2));
}
void MoveFactory::generateDoubleStep(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int stop, std::vector<std::unique_ptr<Move>>& moves) const {
  moves.push_back(std::make_unique<DoubleStep>(origin, target, stop));
}
bool MoveFactory::generateEnPassant(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int stop, std::vector<std::unique_ptr<Move>>& moves) const {
  if (board.at(stop)->isRoyal()) {
    return false;
  }
  moves.push_back(std::make_unique<EnPassant>(origin, target, stop));
  return true;
}
bool MoveFactory::generateEnPassant(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int stop) const {
  return !board.at(stop)->isRoyal();
}
void MoveFactory::generatePromotion(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    int origin, int target, bool black, int order,
    std::vector<std::unique_ptr<Move>>& moves) const {
  moves.push_back(std::make_unique<Promotion>(origin, target, black, order));
}
bool MoveFactory::generatePromotionCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    int origin, int target, bool black, int order,
    std::vector<std::unique_ptr<Move>>& moves) const {
  if (board.at(target)->isRoyal()) {
    return false;
  }
  moves.push_back(
      std::make_unique<PromotionCapture>(origin, target, black, order));
  return true;
}
bool MoveFactory::generatePromotionCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    int origin, int target, bool black, int order) const {
  return !board.at(target)->isRoyal();
}
std::ostream& operator<<(std::ostream& output, const MoveFactory& moveFactory) {
  moveFactory.write(output);
  return output;
}

}  // namespace moderato
