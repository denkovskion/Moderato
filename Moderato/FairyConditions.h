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

#include "MoveFactory.h"

namespace moderato {

class CirceMoveFactory : public MoveFactory {
  void write(std::ostream& output) const override;

 public:
  bool generateCapture(
      const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
      int target, std::vector<std::unique_ptr<Move>>& moves) const override;
  bool generateEnPassant(
      const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
      int target, int stop,
      std::vector<std::unique_ptr<Move>>& moves) const override;
  bool generatePromotionCapture(
      const std::array<std::unique_ptr<Piece>, 128>& board,
      const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
          box,
      int origin, int target, bool black, int order,
      std::vector<std::unique_ptr<Move>>& moves) const override;
};

class NoCaptureMoveFactory : public MoveFactory {
  void write(std::ostream& output) const override;

 public:
  bool generateCapture(
      const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
      int target, std::vector<std::unique_ptr<Move>>& moves) const override;
  bool generateEnPassant(
      const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
      int target, int stop,
      std::vector<std::unique_ptr<Move>>& moves) const override;
  bool generatePromotionCapture(
      const std::array<std::unique_ptr<Piece>, 128>& board,
      const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
          box,
      int origin, int target, bool black, int order,
      std::vector<std::unique_ptr<Move>>& moves) const override;
};

class AntiCirceMoveFactory : public MoveFactory {
  void write(std::ostream& output) const override;

 protected:
  const bool calvet_;

 public:
  AntiCirceMoveFactory(bool calvet_);
  bool generateCapture(
      const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
      int target, std::vector<std::unique_ptr<Move>>& moves) const override;
  bool generateCapture(const std::array<std::unique_ptr<Piece>, 128>& board,
                       int origin, int target) const override;
  bool generateEnPassant(
      const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
      int target, int stop,
      std::vector<std::unique_ptr<Move>>& moves) const override;
  bool generateEnPassant(const std::array<std::unique_ptr<Piece>, 128>& board,
                         int origin, int target, int stop) const override;
  bool generatePromotionCapture(
      const std::array<std::unique_ptr<Piece>, 128>& board,
      const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
          box,
      int origin, int target, bool black, int order,
      std::vector<std::unique_ptr<Move>>& moves) const override;
  bool generatePromotionCapture(
      const std::array<std::unique_ptr<Piece>, 128>& board,
      const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
          box,
      int origin, int target, bool black, int order) const override;
};

}  // namespace moderato
