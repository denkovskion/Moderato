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

#include "Move.h"

#include "Position.h"

namespace moderato {

std::ostream& operator<<(std::ostream& output, const Move& move) {
  move.write(output);
  return output;
}
bool Move::make(Position& position,
                std::vector<std::shared_ptr<Move>>& pseudoLegalMoves,
                std::ostream& lanBuilder, int translate) const {
  preWrite(position, lanBuilder, translate);
  bool result = preMake(position);
  updatePieces(position);
  updateState(position);
  return result && position.isLegal(pseudoLegalMoves);
}
bool Move::make(Position& position,
                std::vector<std::shared_ptr<Move>>& pseudoLegalMoves) const {
  bool result = preMake(position);
  updatePieces(position);
  updateState(position);
  return result && position.isLegal(pseudoLegalMoves);
}
bool Move::make(Position& position) const {
  bool result = preMake(position);
  updatePieces(position);
  updateState(position);
  return result && position.isLegal();
}
void Move::unmake(Position& position) const {
  revertState(position);
  revertPieces(position);
}

void NullMove::write(std::ostream& output) const { output << "NullMove[]"; }
bool NullMove::preMake(Position& position) const { return true; }
void NullMove::updatePieces(Position& position) const {}
void NullMove::revertPieces(Position& position) const {}
void NullMove::updateState(Position& position) const {
  updateState(position.isBlackToMove(), position.getState(),
              position.getMemory());
}
void NullMove::updateState(
    bool& blackToMove, std::pair<std::set<int>, std::shared_ptr<int>>& state,
    std::stack<std::pair<std::set<int>, std::shared_ptr<int>>>& memory) const {
  memory.push(state);
  updateCastlings(state.first);
  updateEnPassant(state.second);
  blackToMove = !blackToMove;
}
void NullMove::updateCastlings(std::set<int>& castlings) const {}
void NullMove::updateEnPassant(std::shared_ptr<int>& enPassant) const {
  enPassant.reset();
}
void NullMove::revertState(Position& position) const {
  revertState(position.isBlackToMove(), position.getState(),
              position.getMemory());
}
void NullMove::revertState(
    bool& blackToMove, std::pair<std::set<int>, std::shared_ptr<int>>& state,
    std::stack<std::pair<std::set<int>, std::shared_ptr<int>>>& memory) const {
  blackToMove = !blackToMove;
  state = std::move(memory.top());
  memory.pop();
}
void NullMove::preWrite(Position& position, std::ostream& lanBuilder,
                        int translate) const {
  lanBuilder << "null";
}

void postWrite(Position& position,
               const std::vector<std::shared_ptr<Move>>& pseudoLegalMoves,
               std::ostream& lanBuilder) {
  int nChecks = position.isCheck();
  bool terminal = position.isTerminal(pseudoLegalMoves);
  if (terminal) {
    if (nChecks > 0) {
      if (nChecks > 1) {
        for (int checkNo = 0; checkNo < nChecks; checkNo++) {
          lanBuilder << "+";
        }
      }
      lanBuilder << "#";
    } else {
      lanBuilder << "=";
    }
  } else {
    if (nChecks > 0) {
      for (int checkNo = 0; checkNo < nChecks; checkNo++) {
        lanBuilder << "+";
      }
    }
  }
}

}  // namespace moderato
