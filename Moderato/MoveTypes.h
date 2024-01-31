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

#include "Move.h"
#include "Piece.h"

namespace moderato {

class QuietMove : public NullMove {
  bool equals(const Move& other) const override;
  void write(std::ostream& output) const override;
  void updatePieces(Position& position) const override;
  virtual void updatePieces(
      std::array<std::unique_ptr<Piece>, 128>& board) const;
  void revertPieces(Position& position) const override;
  virtual void revertPieces(
      std::array<std::unique_ptr<Piece>, 128>& board) const;
  void updateState(Position& position) const override;
  void updateState(
      const std::array<std::unique_ptr<Piece>, 128>& board, bool& blackToMove,
      std::pair<std::set<int>, std::shared_ptr<int>>& state,
      std::stack<std::pair<std::set<int>, std::shared_ptr<int>>>& memory) const;
  virtual void addNoCastling(
      const std::array<std::unique_ptr<Piece>, 128>& board,
      std::set<int>& noCastling) const;
  virtual void setEnPassant(
      const std::array<std::unique_ptr<Piece>, 128>& board,
      std::shared_ptr<int>& enPassant) const;
  void preWrite(Position& position, std::ostream& lanBuilder,
                int translate) const override;
  virtual void preWrite(const std::array<std::unique_ptr<Piece>, 128>& board,
                        std::ostream& lanBuilder, int translate) const;
  void postWrite(Position& position, std::ostream& lanBuilder) const override;

 protected:
  const int origin_;
  const int target_;

 public:
  QuietMove(int origin, int target);
};

class Capture : public QuietMove {
  bool equals(const Move& other) const override;
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
  void addNoCastling(const std::array<std::unique_ptr<Piece>, 128>& board,
                     std::set<int>& noCastling) const override;

 protected:
  const int origin2_;
  const int target2_;
  Castling(int origin, int target, int origin2, int target2);
};

class LongCastling : public Castling {
  bool equals(const Move& other) const override;
  void write(std::ostream& output) const override;
  void preWrite(Position& position, std::ostream& lanBuilder,
                int translate) const override;

 public:
  LongCastling(int origin, int target, int origin2, int target2);
};

class ShortCastling : public Castling {
  bool equals(const Move& other) const override;
  void write(std::ostream& output) const override;
  void preWrite(Position& position, std::ostream& lanBuilder,
                int translate) const override;

 public:
  ShortCastling(int origin, int target, int origin2, int target2);
};

class DoubleStep : public QuietMove {
  bool equals(const Move& other) const override;
  void write(std::ostream& output) const override;
  void setEnPassant(const std::array<std::unique_ptr<Piece>, 128>& board,
                    std::shared_ptr<int>& enPassant) const override;
  const int stop_;

 public:
  DoubleStep(int origin, int target, int stop);
};

class EnPassant : public Capture {
  bool equals(const Move& other) const override;
  void write(std::ostream& output) const override;
  void updatePieces(std::array<std::unique_ptr<Piece>, 128>& board,
                    std::stack<std::unique_ptr<Piece>>& table) const override;
  void revertPieces(std::array<std::unique_ptr<Piece>, 128>& board,
                    std::stack<std::unique_ptr<Piece>>& table) const override;
  void preWrite(const std::array<std::unique_ptr<Piece>, 128>& board,
                std::ostream& lanBuilder, int translate) const override;
  const int stop_;

 public:
  EnPassant(int origin, int target, int stop);
};

class Promotion : public QuietMove {
  bool equals(const Move& other) const override;
  void write(std::ostream& output) const override;
  void updatePieces(Position& position) const override;
  void updatePieces(
      std::array<std::unique_ptr<Piece>, 128>& board,
      std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box)
      const;
  void revertPieces(Position& position) const override;
  void revertPieces(
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
  bool equals(const Move& other) const override;
  void write(std::ostream& output) const override;
  void updatePieces(Position& position) const override;
  void updatePieces(
      std::array<std::unique_ptr<Piece>, 128>& board,
      std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box,
      std::stack<std::unique_ptr<Piece>>& table) const;
  void revertPieces(Position& position) const override;
  void revertPieces(
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

}  // namespace moderato
