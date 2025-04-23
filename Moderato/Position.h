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

#pragma once

#include <stack>

#include "MoveFactory.h"

namespace moderato {

class Position {
  std::array<std::unique_ptr<Piece>, 128> board_;
  std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>> box_;
  std::stack<std::unique_ptr<Piece>> table_;
  bool blackToMove_ = false;
  std::pair<std::set<int>, std::shared_ptr<int>> state_;
  std::stack<std::pair<std::set<int>, std::shared_ptr<int>>> memory_;
  std::unique_ptr<MoveFactory> moveFactory_;

 public:
  Position(
      std::array<std::unique_ptr<Piece>, 128> board,
      std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>> box,
      std::stack<std::unique_ptr<Piece>> table, bool blackToMove,
      std::pair<std::set<int>, std::shared_ptr<int>> state,
      std::stack<std::pair<std::set<int>, std::shared_ptr<int>>> memory,
      std::unique_ptr<MoveFactory> moveFactory);

  std::array<std::unique_ptr<Piece>, 128>& getBoard();
  std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& getBox();
  std::stack<std::unique_ptr<Piece>>& getTable();
  bool& isBlackToMove();
  std::pair<std::set<int>, std::shared_ptr<int>>& getState();
  std::stack<std::pair<std::set<int>, std::shared_ptr<int>>>& getMemory();

  bool isLegal(std::vector<std::unique_ptr<Move>>& pseudoLegalMoves);
  bool isLegal();
  int isCheck();
  bool isTerminal(const std::vector<std::unique_ptr<Move>>& pseudoLegalMoves);

  friend std::ostream& operator<<(std::ostream& output,
                                  const Position& position);
};

}  // namespace moderato
