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
  const bool castling_;
  CirceMove(int rebirth, bool castling);
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
  CirceCapture(int origin, int target, int rebirth, bool castling);
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
  CirceEnPassant(int origin, int target, int stop, int rebirth, bool castling);
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
                        int rebirth, bool castling);
};

class AntiCirceMove {
 protected:
  const int rebirth_;
  const bool castling_;
  AntiCirceMove(int rebirth, bool castling);
};

class AntiCirceCapture : public Capture, protected AntiCirceMove {
  void write(std::ostream& output) const override;
  void updatePieces(std::array<std::unique_ptr<Piece>, 128>& board,
                    std::stack<std::unique_ptr<Piece>>& table) const override;
  void revertPieces(std::array<std::unique_ptr<Piece>, 128>& board,
                    std::stack<std::unique_ptr<Piece>>& table) const override;
  void updateCastlings(std::set<int>& castlings) const override;
  void preWrite(const std::array<std::unique_ptr<Piece>, 128>& board,
                std::ostream& lanBuilder, int translate) const override;

 public:
  AntiCirceCapture(int origin, int target, int rebirth, bool castling);
};

class AntiCirceEnPassant : public EnPassant, protected AntiCirceMove {
  void write(std::ostream& output) const override;
  void updatePieces(std::array<std::unique_ptr<Piece>, 128>& board,
                    std::stack<std::unique_ptr<Piece>>& table) const override;
  void revertPieces(std::array<std::unique_ptr<Piece>, 128>& board,
                    std::stack<std::unique_ptr<Piece>>& table) const override;
  void updateCastlings(std::set<int>& castlings) const override;
  void preWrite(const std::array<std::unique_ptr<Piece>, 128>& board,
                std::ostream& lanBuilder, int translate) const override;

 public:
  AntiCirceEnPassant(int origin, int target, int stop, int rebirth,
                     bool castling);
};

class AntiCircePromotionCapture : public PromotionCapture,
                                  protected AntiCirceMove {
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
                            int rebirth, bool castling);
};

class AndernachMove {
 protected:
  const bool castling_;
  AndernachMove(bool castling);
};

class AndernachCapture : public Capture, protected AndernachMove {
  void write(std::ostream& output) const override;
  void updatePieces(std::array<std::unique_ptr<Piece>, 128>& board,
                    std::stack<std::unique_ptr<Piece>>& table) const override;
  void revertPieces(std::array<std::unique_ptr<Piece>, 128>& board,
                    std::stack<std::unique_ptr<Piece>>& table) const override;
  void updateCastlings(std::set<int>& castlings) const override;
  void preWrite(const std::array<std::unique_ptr<Piece>, 128>& board,
                std::ostream& lanBuilder, int translate) const override;

 public:
  AndernachCapture(int origin, int target, bool castling);
};

class AndernachEnPassant : public EnPassant, protected AndernachMove {
  void write(std::ostream& output) const override;
  void updatePieces(std::array<std::unique_ptr<Piece>, 128>& board,
                    std::stack<std::unique_ptr<Piece>>& table) const override;
  void revertPieces(std::array<std::unique_ptr<Piece>, 128>& board,
                    std::stack<std::unique_ptr<Piece>>& table) const override;
  void updateCastlings(std::set<int>& castlings) const override;
  void preWrite(const std::array<std::unique_ptr<Piece>, 128>& board,
                std::ostream& lanBuilder, int translate) const override;

 public:
  AndernachEnPassant(int origin, int target, int stop, bool castling);
};

class AndernachPromotionCapture : public PromotionCapture,
                                  protected AndernachMove {
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
  AndernachPromotionCapture(int origin, int target, bool black, int order,
                            bool castling);
};

class AntiAndernachMove {
 protected:
  const bool castling_;
  AntiAndernachMove(bool castling);
};

class AntiAndernachQuietMove : public QuietMove, protected AntiAndernachMove {
  void write(std::ostream& output) const override;
  void updatePieces(
      std::array<std::unique_ptr<Piece>, 128>& board) const override;
  void revertPieces(
      std::array<std::unique_ptr<Piece>, 128>& board) const override;
  void updateCastlings(std::set<int>& castlings) const override;
  void preWrite(const std::array<std::unique_ptr<Piece>, 128>& board,
                std::ostream& lanBuilder, int translate) const override;

 public:
  AntiAndernachQuietMove(int origin, int target, bool castling);
};

class AntiAndernachPromotion : public Promotion, protected AntiAndernachMove {
  void write(std::ostream& output) const override;
  void updatePieces(
      std::array<std::unique_ptr<Piece>, 128>& board,
      std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box)
      const override;
  void revertPieces(
      std::array<std::unique_ptr<Piece>, 128>& board,
      std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box)
      const override;
  void updateCastlings(std::set<int>& castlings) const override;
  void preWrite(
      const std::array<std::unique_ptr<Piece>, 128>& board,
      const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
          box,
      std::ostream& lanBuilder, int translate) const override;

 public:
  AntiAndernachPromotion(int origin, int target, bool black, int order,
                         bool castling);
};

class CirceAndernachMove {
 protected:
  const int rebirth_;
  const bool castling_;
  const bool castling2_;
  CirceAndernachMove(int rebirth, bool castling, bool castling2);
};

class CirceAndernachCapture : public Capture, protected CirceAndernachMove {
  void write(std::ostream& output) const override;
  void updatePieces(std::array<std::unique_ptr<Piece>, 128>& board,
                    std::stack<std::unique_ptr<Piece>>& table) const override;
  void revertPieces(std::array<std::unique_ptr<Piece>, 128>& board,
                    std::stack<std::unique_ptr<Piece>>& table) const override;
  void updateCastlings(std::set<int>& castlings) const override;
  void preWrite(const std::array<std::unique_ptr<Piece>, 128>& board,
                std::ostream& lanBuilder, int translate) const override;

 public:
  CirceAndernachCapture(int origin, int target, int rebirth, bool castling,
                        bool castling2);
};

class CirceAndernachEnPassant : public EnPassant, protected CirceAndernachMove {
  void write(std::ostream& output) const override;
  void updatePieces(std::array<std::unique_ptr<Piece>, 128>& board,
                    std::stack<std::unique_ptr<Piece>>& table) const override;
  void revertPieces(std::array<std::unique_ptr<Piece>, 128>& board,
                    std::stack<std::unique_ptr<Piece>>& table) const override;
  void updateCastlings(std::set<int>& castlings) const override;
  void preWrite(const std::array<std::unique_ptr<Piece>, 128>& board,
                std::ostream& lanBuilder, int translate) const override;

 public:
  CirceAndernachEnPassant(int origin, int target, int stop, int rebirth,
                          bool castling, bool castling2);
};

class CirceAndernachPromotionCapture : public PromotionCapture,
                                       protected CirceAndernachMove {
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
  CirceAndernachPromotionCapture(int origin, int target, bool black, int order,
                                 int rebirth, bool castling, bool castling2);
};

class AntiCirceAndernachMove {
 protected:
  const int rebirth_;
  const bool castling_;
  AntiCirceAndernachMove(int rebirth, bool castling);
};

class AntiCirceAndernachCapture : public Capture,
                                  protected AntiCirceAndernachMove {
  void write(std::ostream& output) const override;
  void updatePieces(std::array<std::unique_ptr<Piece>, 128>& board,
                    std::stack<std::unique_ptr<Piece>>& table) const override;
  void revertPieces(std::array<std::unique_ptr<Piece>, 128>& board,
                    std::stack<std::unique_ptr<Piece>>& table) const override;
  void updateCastlings(std::set<int>& castlings) const override;
  void preWrite(const std::array<std::unique_ptr<Piece>, 128>& board,
                std::ostream& lanBuilder, int translate) const override;

 public:
  AntiCirceAndernachCapture(int origin, int target, int rebirth, bool castling);
};

class AntiCirceAndernachEnPassant : public EnPassant,
                                    protected AntiCirceAndernachMove {
  void write(std::ostream& output) const override;
  void updatePieces(std::array<std::unique_ptr<Piece>, 128>& board,
                    std::stack<std::unique_ptr<Piece>>& table) const override;
  void revertPieces(std::array<std::unique_ptr<Piece>, 128>& board,
                    std::stack<std::unique_ptr<Piece>>& table) const override;
  void updateCastlings(std::set<int>& castlings) const override;
  void preWrite(const std::array<std::unique_ptr<Piece>, 128>& board,
                std::ostream& lanBuilder, int translate) const override;

 public:
  AntiCirceAndernachEnPassant(int origin, int target, int stop, int rebirth,
                              bool castling);
};

class AntiCirceAndernachPromotionCapture : public PromotionCapture,
                                           protected AntiCirceAndernachMove {
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
  AntiCirceAndernachPromotionCapture(int origin, int target, bool black,
                                     int order, int rebirth, bool castling);
};

}  // namespace moderato
