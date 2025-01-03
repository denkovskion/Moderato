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

#include "Move.h"
#include "Piece.h"

namespace moderato {

class QuietMove : public NullMove {
  void write(std::ostream& output) const override;
  void updatePieces(Position& position) const override;
  virtual void updatePieces(
      std::array<std::unique_ptr<Piece>, 128>& board) const;
  void revertPieces(Position& position) const override;
  virtual void revertPieces(
      std::array<std::unique_ptr<Piece>, 128>& board) const;
  void updateCastlings(std::set<int>& castlings) const override;
  void preWrite(Position& position, std::ostream& lanBuilder,
                int translate) const override;
  virtual void preWrite(const std::array<std::unique_ptr<Piece>, 128>& board,
                        std::ostream& lanBuilder, int translate) const;

 protected:
  const int origin_;
  const int target_;

 public:
  QuietMove(int origin, int target);
  void postWrite(Position& position,
                 const std::vector<std::shared_ptr<Move>>& pseudoLegalMoves,
                 std::ostream& lanBuilder) const override;
};

class Capture : public QuietMove {
  void write(std::ostream& output) const override;
  void updatePieces(Position& position) const override;
  virtual void updatePieces(std::array<std::unique_ptr<Piece>, 128>& board,
                            std::stack<std::unique_ptr<Piece>>& table) const;
  void revertPieces(Position& position) const override;
  virtual void revertPieces(std::array<std::unique_ptr<Piece>, 128>& board,
                            std::stack<std::unique_ptr<Piece>>& table) const;
  void preWrite(const std::array<std::unique_ptr<Piece>, 128>& board,
                std::ostream& lanBuilder, int translate) const override;

 public:
  Capture(int origin, int target);
};

class Castling : public QuietMove {
  bool preMake(Position& position) const override;
  void updatePieces(
      std::array<std::unique_ptr<Piece>, 128>& board) const override;
  void revertPieces(
      std::array<std::unique_ptr<Piece>, 128>& board) const override;
  void updateCastlings(std::set<int>& castlings) const override;

 protected:
  const int origin2_;
  const int target2_;
  Castling(int origin, int target, int origin2, int target2);
};

class LongCastling : public Castling {
  void write(std::ostream& output) const override;
  void preWrite(Position& position, std::ostream& lanBuilder,
                int translate) const override;

 public:
  LongCastling(int origin, int target, int origin2, int target2);
};

class ShortCastling : public Castling {
  void write(std::ostream& output) const override;
  void preWrite(Position& position, std::ostream& lanBuilder,
                int translate) const override;

 public:
  ShortCastling(int origin, int target, int origin2, int target2);
};

class DoubleStep : public QuietMove {
  void write(std::ostream& output) const override;
  void updateCastlings(std::set<int>& castlings) const override;
  void updateEnPassant(std::shared_ptr<int>& enPassant) const override;

 protected:
  const int stop_;

 public:
  DoubleStep(int origin, int target, int stop);
};

class EnPassant : public Capture {
  void write(std::ostream& output) const override;
  void updatePieces(std::array<std::unique_ptr<Piece>, 128>& board,
                    std::stack<std::unique_ptr<Piece>>& table) const override;
  void revertPieces(std::array<std::unique_ptr<Piece>, 128>& board,
                    std::stack<std::unique_ptr<Piece>>& table) const override;
  void updateCastlings(std::set<int>& castlings) const override;
  void preWrite(const std::array<std::unique_ptr<Piece>, 128>& board,
                std::ostream& lanBuilder, int translate) const override;

 protected:
  const int stop_;

 public:
  EnPassant(int origin, int target, int stop);
};

class Promotion : public QuietMove {
  void write(std::ostream& output) const override;
  void updatePieces(Position& position) const override;
  virtual void updatePieces(
      std::array<std::unique_ptr<Piece>, 128>& board,
      std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box)
      const;
  void revertPieces(Position& position) const override;
  virtual void revertPieces(
      std::array<std::unique_ptr<Piece>, 128>& board,
      std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box)
      const;
  void preWrite(Position& position, std::ostream& lanBuilder,
                int translate) const override;
  virtual void preWrite(
      const std::array<std::unique_ptr<Piece>, 128>& board,
      const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
          box,
      std::ostream& lanBuilder, int translate) const;

 protected:
  const bool black_;
  const int order_;

 public:
  Promotion(int origin, int target, bool black, int order);
};

class PromotionCapture : public Promotion {
  void write(std::ostream& output) const override;
  void updatePieces(Position& position) const override;
  virtual void updatePieces(
      std::array<std::unique_ptr<Piece>, 128>& board,
      std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box,
      std::stack<std::unique_ptr<Piece>>& table) const;
  void revertPieces(Position& position) const override;
  virtual void revertPieces(
      std::array<std::unique_ptr<Piece>, 128>& board,
      std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box,
      std::stack<std::unique_ptr<Piece>>& table) const;
  void preWrite(
      const std::array<std::unique_ptr<Piece>, 128>& board,
      const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
          box,
      std::ostream& lanBuilder, int translate) const override;

 public:
  PromotionCapture(int origin, int target, bool black, int order);
};

std::string toCode(const std::array<std::unique_ptr<Piece>, 128>& board,
                   int square);

}  // namespace moderato
