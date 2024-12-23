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

#include "MoveFactory.h"

#include "MoveTypes.h"

namespace moderato {

void MoveFactory::write(std::ostream& output) const {
  output << "MoveFactory[]";
}
void MoveFactory::generateQuietMove(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, std::vector<std::shared_ptr<Move>>& moves) const {
  moves.push_back(std::make_shared<QuietMove>(origin, target));
}
void MoveFactory::generateCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, std::vector<std::shared_ptr<Move>>& moves) const {
  moves.push_back(std::make_shared<Capture>(origin, target));
}
void MoveFactory::generateLongCastling(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int origin2, int target2,
    std::vector<std::shared_ptr<Move>>& moves) const {
  moves.push_back(
      std::make_shared<LongCastling>(origin, target, origin2, target2));
}
void MoveFactory::generateShortCastling(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int origin2, int target2,
    std::vector<std::shared_ptr<Move>>& moves) const {
  moves.push_back(
      std::make_shared<ShortCastling>(origin, target, origin2, target2));
}
void MoveFactory::generateDoubleStep(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int stop, std::vector<std::shared_ptr<Move>>& moves) const {
  moves.push_back(std::make_shared<DoubleStep>(origin, target, stop));
}
void MoveFactory::generateEnPassant(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int stop, std::vector<std::shared_ptr<Move>>& moves) const {
  moves.push_back(std::make_shared<EnPassant>(origin, target, stop));
}
void MoveFactory::generatePromotion(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    int origin, int target, bool black, int order,
    std::vector<std::shared_ptr<Move>>& moves) const {
  moves.push_back(std::make_shared<Promotion>(origin, target, black, order));
}
void MoveFactory::generatePromotionCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    int origin, int target, bool black, int order,
    std::vector<std::shared_ptr<Move>>& moves) const {
  moves.push_back(
      std::make_shared<PromotionCapture>(origin, target, black, order));
}
std::ostream& operator<<(std::ostream& output, const MoveFactory& moveFactory) {
  moveFactory.write(output);
  return output;
}

}  // namespace moderato
