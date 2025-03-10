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

#include "PieceCategories.h"

namespace moderato {

class King : public Piece, Leaper {
  static std::vector<int>& leaps(
      const std::array<std::unique_ptr<Piece>, 128>& board);
  const std::vector<int>& getLeaps(
      const std::array<std::unique_ptr<Piece>, 128>& board) const override;
  std::string getName() const override;

 public:
  King(bool black);
  bool isBlack() const override;
  bool isRoyal() const override;
  bool isCastling() const override;
  int findRebirthSquare(const std::array<std::unique_ptr<Piece>, 128>& board,
                        int square) const override;
  bool generateMoves(
      const std::array<std::unique_ptr<Piece>, 128>& board,
      const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
          box,
      const std::pair<std::set<int>, std::shared_ptr<int>>& state, int origin,
      const MoveFactory& moveFactory,
      std::vector<std::shared_ptr<Move>>& moves) const override;
  bool generateMoves(
      const std::array<std::unique_ptr<Piece>, 128>& board,
      const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
          box,
      const std::pair<std::set<int>, std::shared_ptr<int>>& state, int origin,
      const MoveFactory& moveFactory) const override;
  static std::string code(int translate);
  std::string getCode(int translate) const override;
};

class Queen : public Piece, Rider {
  static std::vector<int>& rides(
      const std::array<std::unique_ptr<Piece>, 128>& board);
  const std::vector<int>& getRides(
      const std::array<std::unique_ptr<Piece>, 128>& board) const override;
  std::string getName() const override;

 public:
  Queen(bool black);
  bool isBlack() const override;
  int findRebirthSquare(const std::array<std::unique_ptr<Piece>, 128>& board,
                        int square) const override;
  bool generateMoves(
      const std::array<std::unique_ptr<Piece>, 128>& board,
      const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
          box,
      const std::pair<std::set<int>, std::shared_ptr<int>>& state, int origin,
      const MoveFactory& moveFactory,
      std::vector<std::shared_ptr<Move>>& moves) const override;
  bool generateMoves(
      const std::array<std::unique_ptr<Piece>, 128>& board,
      const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
          box,
      const std::pair<std::set<int>, std::shared_ptr<int>>& state, int origin,
      const MoveFactory& moveFactory) const override;
  static std::string code(int translate);
  std::string getCode(int translate) const override;
};

class Rook : public Piece, Rider {
  static std::vector<int>& rides(
      const std::array<std::unique_ptr<Piece>, 128>& board);
  const std::vector<int>& getRides(
      const std::array<std::unique_ptr<Piece>, 128>& board) const override;
  std::string getName() const override;

 public:
  Rook(bool black);
  bool isBlack() const override;
  bool isCastling() const override;
  int findRebirthSquare(const std::array<std::unique_ptr<Piece>, 128>& board,
                        int square) const override;
  bool generateMoves(
      const std::array<std::unique_ptr<Piece>, 128>& board,
      const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
          box,
      const std::pair<std::set<int>, std::shared_ptr<int>>& state, int origin,
      const MoveFactory& moveFactory,
      std::vector<std::shared_ptr<Move>>& moves) const override;
  bool generateMoves(
      const std::array<std::unique_ptr<Piece>, 128>& board,
      const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
          box,
      const std::pair<std::set<int>, std::shared_ptr<int>>& state, int origin,
      const MoveFactory& moveFactory) const override;
  static std::string code(int translate);
  std::string getCode(int translate) const override;
};

class Bishop : public Piece, Rider {
  static std::vector<int>& rides(
      const std::array<std::unique_ptr<Piece>, 128>& board);
  const std::vector<int>& getRides(
      const std::array<std::unique_ptr<Piece>, 128>& board) const override;
  std::string getName() const override;

 public:
  Bishop(bool black);
  bool isBlack() const override;
  int findRebirthSquare(const std::array<std::unique_ptr<Piece>, 128>& board,
                        int square) const override;
  bool generateMoves(
      const std::array<std::unique_ptr<Piece>, 128>& board,
      const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
          box,
      const std::pair<std::set<int>, std::shared_ptr<int>>& state, int origin,
      const MoveFactory& moveFactory,
      std::vector<std::shared_ptr<Move>>& moves) const override;
  bool generateMoves(
      const std::array<std::unique_ptr<Piece>, 128>& board,
      const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
          box,
      const std::pair<std::set<int>, std::shared_ptr<int>>& state, int origin,
      const MoveFactory& moveFactory) const override;
  static std::string code(int translate);
  std::string getCode(int translate) const override;
};

class Knight : public Piece, Leaper {
  static std::vector<int>& leaps(
      const std::array<std::unique_ptr<Piece>, 128>& board);
  const std::vector<int>& getLeaps(
      const std::array<std::unique_ptr<Piece>, 128>& board) const override;
  std::string getName() const override;

 public:
  Knight(bool black);
  bool isBlack() const override;
  int findRebirthSquare(const std::array<std::unique_ptr<Piece>, 128>& board,
                        int square) const override;
  bool generateMoves(
      const std::array<std::unique_ptr<Piece>, 128>& board,
      const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
          box,
      const std::pair<std::set<int>, std::shared_ptr<int>>& state, int origin,
      const MoveFactory& moveFactory,
      std::vector<std::shared_ptr<Move>>& moves) const override;
  bool generateMoves(
      const std::array<std::unique_ptr<Piece>, 128>& board,
      const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
          box,
      const std::pair<std::set<int>, std::shared_ptr<int>>& state, int origin,
      const MoveFactory& moveFactory) const override;
  static std::string code(int translate);
  std::string getCode(int translate) const override;
};

class Pawn : public Piece {
  std::string getName() const override;

 public:
  Pawn(bool black);
  int findRebirthSquare(const std::array<std::unique_ptr<Piece>, 128>& board,
                        int square) const override;
  bool generateMoves(
      const std::array<std::unique_ptr<Piece>, 128>& board,
      const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
          box,
      const std::pair<std::set<int>, std::shared_ptr<int>>& state, int origin,
      const MoveFactory& moveFactory,
      std::vector<std::shared_ptr<Move>>& moves) const override;
  bool generateMoves(
      const std::array<std::unique_ptr<Piece>, 128>& board,
      const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
          box,
      const std::pair<std::set<int>, std::shared_ptr<int>>& state, int origin,
      const MoveFactory& moveFactory) const override;
  static std::string code(int translate);
  std::string getCode(int translate) const override;
};

}  // namespace moderato
