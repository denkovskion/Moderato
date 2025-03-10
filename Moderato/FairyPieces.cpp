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

#include "FairyPieces.h"

namespace moderato {

FairyPiece::FairyPiece(bool black) : Piece(black) {}
int FairyPiece::findRebirthSquare(
    const std::array<std::unique_ptr<Piece>, 128>& board, int square) const {
  return (square / 16) * 16 + (black_ ? 0 : 7);
}

Grasshopper::Grasshopper(bool black) : FairyPiece(black) {}
Nightrider::Nightrider(bool black) : FairyPiece(black) {}
Amazon::Amazon(bool black) : FairyPiece(black) {}

bool Grasshopper::isBlack() const { return Piece::isBlack(); }
bool Nightrider::isBlack() const { return Piece::isBlack(); }
bool Amazon::isBlack() const { return Piece::isBlack(); }

std::vector<int>& Grasshopper::hops(
    const std::array<std::unique_ptr<Piece>, 128>& board) {
  static std::vector<int> hops = {-17, -16, -15, -1, 1, 15, 16, 17};
  return hops;
}
const std::vector<int>& Grasshopper::getHops(
    const std::array<std::unique_ptr<Piece>, 128>& board) const {
  return hops(board);
}
bool Grasshopper::generateMoves(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    const std::pair<std::set<int>, std::shared_ptr<int>>& state, int origin,
    const MoveFactory& moveFactory,
    std::vector<std::shared_ptr<Move>>& moves) const {
  return Hopper::generateMoves(board, origin, moveFactory, moves);
}
bool Grasshopper::generateMoves(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    const std::pair<std::set<int>, std::shared_ptr<int>>& state, int origin,
    const MoveFactory& moveFactory) const {
  return Hopper::generateMoves(board, origin, moveFactory);
}

std::vector<int>& Nightrider::rides(
    const std::array<std::unique_ptr<Piece>, 128>& board) {
  static std::vector<int> rides = {-33, -31, -18, -14, 14, 18, 31, 33};
  return rides;
}
const std::vector<int>& Nightrider::getRides(
    const std::array<std::unique_ptr<Piece>, 128>& board) const {
  return rides(board);
}
bool Nightrider::generateMoves(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    const std::pair<std::set<int>, std::shared_ptr<int>>& state, int origin,
    const MoveFactory& moveFactory,
    std::vector<std::shared_ptr<Move>>& moves) const {
  return Rider::generateMoves(board, origin, moveFactory, moves);
}
bool Nightrider::generateMoves(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    const std::pair<std::set<int>, std::shared_ptr<int>>& state, int origin,
    const MoveFactory& moveFactory) const {
  return Rider::generateMoves(board, origin, moveFactory);
}

std::vector<int>& Amazon::rides(
    const std::array<std::unique_ptr<Piece>, 128>& board) {
  static std::vector<int> rides = {-17, -16, -15, -1, 1, 15, 16, 17};
  return rides;
}
const std::vector<int>& Amazon::getRides(
    const std::array<std::unique_ptr<Piece>, 128>& board) const {
  return rides(board);
}
std::vector<int>& Amazon::leaps(
    const std::array<std::unique_ptr<Piece>, 128>& board) {
  static std::vector<int> leaps = {-33, -31, -18, -14, 14, 18, 31, 33};
  return leaps;
}
const std::vector<int>& Amazon::getLeaps(
    const std::array<std::unique_ptr<Piece>, 128>& board) const {
  return leaps(board);
}
bool Amazon::generateMoves(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    const std::pair<std::set<int>, std::shared_ptr<int>>& state, int origin,
    const MoveFactory& moveFactory,
    std::vector<std::shared_ptr<Move>>& moves) const {
  return Rider::generateMoves(board, origin, moveFactory, moves) &&
         Leaper::generateMoves(board, origin, moveFactory, moves);
}
bool Amazon::generateMoves(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    const std::pair<std::set<int>, std::shared_ptr<int>>& state, int origin,
    const MoveFactory& moveFactory) const {
  return Rider::generateMoves(board, origin, moveFactory) &&
         Leaper::generateMoves(board, origin, moveFactory);
}

std::string Grasshopper::code(int translate) {
  return translate == GERMAN    ? "G"
         : translate == FRENCH  ? "S"
         : translate == ENGLISH ? "G"
                                : "Grasshopper";
}
std::string Grasshopper::getCode(int translate) const {
  return code(translate);
}
std::string Nightrider::code(int translate) {
  return translate == GERMAN    ? "N"
         : translate == FRENCH  ? "N"
         : translate == ENGLISH ? "N"
                                : "Nightrider";
}
std::string Nightrider::getCode(int translate) const { return code(translate); }
std::string Amazon::code(int translate) {
  return translate == GERMAN    ? "AM"
         : translate == FRENCH  ? "AM"
         : translate == ENGLISH ? "AM"
                                : "Amazon";
}
std::string Amazon::getCode(int translate) const { return code(translate); }

std::string Grasshopper::getName() const { return "Grasshopper"; }
std::string Nightrider::getName() const { return "Nightrider"; }
std::string Amazon::getName() const { return "Amazon"; }

}  // namespace moderato
