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

#include <memory>
#include <ostream>
#include <set>
#include <stack>
#include <utility>
#include <vector>

namespace moderato {

class Position;

class Move {
  virtual void write(std::ostream& output) const = 0;
  virtual bool preMake(Position& position) const = 0;
  virtual void updatePieces(Position& position) const = 0;
  virtual void revertPieces(Position& position) const = 0;
  virtual void updateState(Position& position) const = 0;
  virtual void revertState(Position& position) const = 0;
  virtual void preWrite(Position& position, std::ostream& lanBuilder,
                        int translate) const = 0;

 public:
  friend std::ostream& operator<<(std::ostream& output, const Move& move);
  virtual ~Move();
  bool make(Position& position,
            std::vector<std::unique_ptr<Move>>& pseudoLegalMoves,
            std::ostream& lanBuilder, int translate) const;
  bool make(Position& position,
            std::vector<std::unique_ptr<Move>>& pseudoLegalMoves) const;
  bool make(Position& position) const;
  void unmake(Position& position) const;
};

class NullMove : public Move {
  void write(std::ostream& output) const override;
  bool preMake(Position& position) const override;
  void updatePieces(Position& position) const override;
  void revertPieces(Position& position) const override;
  void updateState(Position& position) const override;
  void updateState(
      bool& blackToMove, std::pair<std::set<int>, std::shared_ptr<int>>& state,
      std::stack<std::pair<std::set<int>, std::shared_ptr<int>>>& memory) const;
  virtual void updateCastlings(std::set<int>& castlings) const;
  virtual void updateEnPassant(std::shared_ptr<int>& enPassant) const;
  void revertState(Position& position) const override;
  void revertState(
      bool& blackToMove, std::pair<std::set<int>, std::shared_ptr<int>>& state,
      std::stack<std::pair<std::set<int>, std::shared_ptr<int>>>& memory) const;
  void preWrite(Position& position, std::ostream& lanBuilder,
                int translate) const override;
};

void postWrite(Position& position,
               const std::vector<std::unique_ptr<Move>>& pseudoLegalMoves,
               std::ostream& lanBuilder);

}  // namespace moderato
