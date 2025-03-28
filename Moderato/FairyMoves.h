/*
 * MIT License
 *
 * Copyright (c) 2025 Ivan Denkovski
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

#include "MoveTypes.h"

namespace moderato {

class CirceMove {
 protected:
  const int rebirth_;
  CirceMove(int rebirth);
};

class CirceCapture : public Capture, protected CirceMove {
  void write(std::ostream& output) const override;
  void updatePieces(std::array<std::unique_ptr<Piece>, 128>& board,
                    std::stack<std::unique_ptr<Piece>>& table) const override;
  void revertPieces(std::array<std::unique_ptr<Piece>, 128>& board,
                    std::stack<std::unique_ptr<Piece>>& table) const override;
  void updateCastlings(std::set<int>& castlings) const override;
  void preWrite(const std::array<std::unique_ptr<Piece>, 128>& board,
                std::ostream& lanBuilder, int translate) const override;

 public:
  CirceCapture(int origin, int target, int rebirth);
};

class CirceCaptureCastling : public CirceCapture {
  void write(std::ostream& output) const override;
  void updateCastlings(std::set<int>& castlings) const override;

 public:
  CirceCaptureCastling(int origin, int target, int rebirth);
};

class CirceEnPassant : public EnPassant, protected CirceMove {
  void write(std::ostream& output) const override;
  void updatePieces(std::array<std::unique_ptr<Piece>, 128>& board,
                    std::stack<std::unique_ptr<Piece>>& table) const override;
  void revertPieces(std::array<std::unique_ptr<Piece>, 128>& board,
                    std::stack<std::unique_ptr<Piece>>& table) const override;
  void updateCastlings(std::set<int>& castlings) const override;
  void preWrite(const std::array<std::unique_ptr<Piece>, 128>& board,
                std::ostream& lanBuilder, int translate) const override;

 public:
  CirceEnPassant(int origin, int target, int stop, int rebirth);
};

class CirceEnPassantCastling : public CirceEnPassant {
  void write(std::ostream& output) const override;
  void updateCastlings(std::set<int>& castlings) const override;

 public:
  CirceEnPassantCastling(int origin, int target, int stop, int rebirth);
};

class CircePromotionCapture : public PromotionCapture, protected CirceMove {
  void write(std::ostream& output) const override;
  void updatePieces(
      std::array<std::unique_ptr<Piece>, 128>& board,
      std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box,
      std::stack<std::unique_ptr<Piece>>& table) const override;
  void revertPieces(
      std::array<std::unique_ptr<Piece>, 128>& board,
      std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box,
      std::stack<std::unique_ptr<Piece>>& table) const override;
  void updateCastlings(std::set<int>& castlings) const override;
  void preWrite(
      const std::array<std::unique_ptr<Piece>, 128>& board,
      const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
          box,
      std::ostream& lanBuilder, int translate) const override;

 public:
  CircePromotionCapture(int origin, int target, bool black, int order,
                        int rebirth);
};

class CircePromotionCaptureCastling : public CircePromotionCapture {
  void write(std::ostream& output) const override;
  void updateCastlings(std::set<int>& castlings) const override;

 public:
  CircePromotionCaptureCastling(int origin, int target, bool black, int order,
                                int rebirth);
};

class AntiCirceCapture : public Capture, protected CirceMove {
  void write(std::ostream& output) const override;
  void updatePieces(std::array<std::unique_ptr<Piece>, 128>& board,
                    std::stack<std::unique_ptr<Piece>>& table) const override;
  void revertPieces(std::array<std::unique_ptr<Piece>, 128>& board,
                    std::stack<std::unique_ptr<Piece>>& table) const override;
  void updateCastlings(std::set<int>& castlings) const override;
  void preWrite(const std::array<std::unique_ptr<Piece>, 128>& board,
                std::ostream& lanBuilder, int translate) const override;

 public:
  AntiCirceCapture(int origin, int target, int rebirth);
};

class AntiCirceCaptureCastling : public AntiCirceCapture {
  void write(std::ostream& output) const override;
  void updateCastlings(std::set<int>& castlings) const override;

 public:
  AntiCirceCaptureCastling(int origin, int target, int rebirth);
};

class AntiCirceEnPassant : public EnPassant, protected CirceMove {
  void write(std::ostream& output) const override;
  void updatePieces(std::array<std::unique_ptr<Piece>, 128>& board,
                    std::stack<std::unique_ptr<Piece>>& table) const override;
  void revertPieces(std::array<std::unique_ptr<Piece>, 128>& board,
                    std::stack<std::unique_ptr<Piece>>& table) const override;
  void updateCastlings(std::set<int>& castlings) const override;
  void preWrite(const std::array<std::unique_ptr<Piece>, 128>& board,
                std::ostream& lanBuilder, int translate) const override;

 public:
  AntiCirceEnPassant(int origin, int target, int stop, int rebirth);
};

class AntiCirceEnPassantCastling : public AntiCirceEnPassant {
  void write(std::ostream& output) const override;
  void updateCastlings(std::set<int>& castlings) const override;

 public:
  AntiCirceEnPassantCastling(int origin, int target, int stop, int rebirth);
};

class AntiCircePromotionCapture : public PromotionCapture, protected CirceMove {
  void write(std::ostream& output) const override;
  void updatePieces(
      std::array<std::unique_ptr<Piece>, 128>& board,
      std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box,
      std::stack<std::unique_ptr<Piece>>& table) const override;
  void revertPieces(
      std::array<std::unique_ptr<Piece>, 128>& board,
      std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box,
      std::stack<std::unique_ptr<Piece>>& table) const override;
  void updateCastlings(std::set<int>& castlings) const override;
  void preWrite(
      const std::array<std::unique_ptr<Piece>, 128>& board,
      const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
          box,
      std::ostream& lanBuilder, int translate) const override;

 public:
  AntiCircePromotionCapture(int origin, int target, bool black, int order,
                            int rebirth);
};

class AntiCircePromotionCaptureCastling : public AntiCircePromotionCapture {
  void write(std::ostream& output) const override;
  void updateCastlings(std::set<int>& castlings) const override;

 public:
  AntiCircePromotionCaptureCastling(int origin, int target, bool black,
                                    int order, int rebirth);
};

}  // namespace moderato
