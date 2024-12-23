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

#pragma once

#include <array>
#include <deque>
#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace moderato {

class Move;
class MoveFactory;

class Piece {
  virtual std::string getName() const = 0;

 protected:
  const bool black_;
  Piece(bool black);

 public:
  virtual ~Piece();
  bool isBlack() const;
  virtual bool isRoyal() const;
  virtual bool isCastling() const;
  virtual int findRebirthSquare(
      const std::array<std::unique_ptr<Piece>, 128>& board,
      int origin) const = 0;
  virtual bool generateMoves(
      const std::array<std::unique_ptr<Piece>, 128>& board,
      const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
          box,
      const std::pair<std::set<int>, std::shared_ptr<int>>& state, int origin,
      const MoveFactory& moveFactory,
      std::vector<std::shared_ptr<Move>>& moves) const = 0;
  virtual bool generateMoves(
      const std::array<std::unique_ptr<Piece>, 128>& board,
      const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
          box,
      const std::pair<std::set<int>, std::shared_ptr<int>>& state,
      int origin) const = 0;
  enum { ENGLISH = 1, FRENCH, GERMAN };
  virtual std::string getCode(int translate) const = 0;
  friend std::ostream& operator<<(std::ostream& output, const Piece& piece);
};

}  // namespace moderato
