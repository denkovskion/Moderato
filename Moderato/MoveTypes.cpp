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

#include "MoveTypes.h"

#include "Position.h"

namespace moderato {

std::string toCode(const std::array<std::unique_ptr<Piece>, 128>& board,
                   int square);

QuietMove::QuietMove(int origin, int target)
    : origin_(origin), target_(target) {}
bool QuietMove::equals(const Move& other) const {
  return origin_ == static_cast<const QuietMove&>(other).origin_ &&
         target_ == static_cast<const QuietMove&>(other).target_;
}
void QuietMove::write(std::ostream& output) const {
  output << "QuietMove[origin=" << origin_ << ", target=" << target_ << "]";
}
void QuietMove::updatePieces(Position& position) const {
  updatePieces(position.getBoard());
}
void QuietMove::updatePieces(
    std::array<std::unique_ptr<Piece>, 128>& board) const {
  board[target_] = std::move(board[origin_]);
}
void QuietMove::revertPieces(Position& position) const {
  revertPieces(position.getBoard());
}
void QuietMove::revertPieces(
    std::array<std::unique_ptr<Piece>, 128>& board) const {
  board[origin_] = std::move(board[target_]);
}
void QuietMove::removeCastlings(std::set<int>& castlings) const {
  castlings.erase(origin_);
  castlings.erase(target_);
}
void QuietMove::preWrite(Position& position, std::ostream& lanBuilder,
                         int translate) const {
  preWrite(position.getBoard(), lanBuilder, translate);
}
void QuietMove::preWrite(const std::array<std::unique_ptr<Piece>, 128>& board,
                         std::ostream& lanBuilder, int translate) const {
  lanBuilder << board[origin_]->getCode(translate) << toCode(board, origin_)
             << "-" << toCode(board, target_);
}
void QuietMove::postWrite(Position& position, std::ostream& lanBuilder) const {
  int nChecks = position.isCheck();
  bool terminal = position.isTerminal();
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

Capture::Capture(int origin, int target) : QuietMove(origin, target) {}
bool Capture::equals(const Move& other) const {
  return origin_ == static_cast<const Capture&>(other).origin_ &&
         target_ == static_cast<const Capture&>(other).target_;
}
void Capture::write(std::ostream& output) const {
  output << "Capture[origin=" << origin_ << ", target=" << target_ << "]";
}
void Capture::updatePieces(Position& position) const {
  updatePieces(position.getBoard(), position.getTable());
}
void Capture::updatePieces(std::array<std::unique_ptr<Piece>, 128>& board,
                           std::stack<std::unique_ptr<Piece>>& table) const {
  table.push(std::move(board[target_]));
  board[target_] = std::move(board[origin_]);
}
void Capture::revertPieces(Position& position) const {
  revertPieces(position.getBoard(), position.getTable());
}
void Capture::revertPieces(std::array<std::unique_ptr<Piece>, 128>& board,
                           std::stack<std::unique_ptr<Piece>>& table) const {
  board[origin_] = std::move(board[target_]);
  board[target_] = std::move(table.top());
  table.pop();
}
void Capture::preWrite(const std::array<std::unique_ptr<Piece>, 128>& board,
                       std::ostream& lanBuilder, int translate) const {
  lanBuilder << board[origin_]->getCode(translate) << toCode(board, origin_)
             << "x" << toCode(board, target_);
}

Castling::Castling(int origin, int target, int origin2, int target2)
    : QuietMove(origin, target), origin2_(origin2), target2_(target2) {}
bool Castling::preMake(Position& position) const {
  NullMove nullMove;
  bool result = nullMove.make(position);
  nullMove.unmake(position);
  QuietMove stopMove(origin_, target2_);
  result = stopMove.make(position) && result;
  stopMove.unmake(position);
  return result;
}
void Castling::updatePieces(
    std::array<std::unique_ptr<Piece>, 128>& board) const {
  board[target_] = std::move(board[origin_]);
  board[target2_] = std::move(board[origin2_]);
}
void Castling::revertPieces(
    std::array<std::unique_ptr<Piece>, 128>& board) const {
  board[origin2_] = std::move(board[target2_]);
  board[origin_] = std::move(board[target_]);
}
void Castling::removeCastlings(std::set<int>& castlings) const {
  castlings.erase(origin_);
  castlings.erase(origin2_);
}

LongCastling::LongCastling(int origin, int target, int origin2, int target2)
    : Castling(origin, target, origin2, target2) {}
bool LongCastling::equals(const Move& other) const {
  return origin_ == static_cast<const LongCastling&>(other).origin_ &&
         target_ == static_cast<const LongCastling&>(other).target_ &&
         origin2_ == static_cast<const LongCastling&>(other).origin2_ &&
         target2_ == static_cast<const LongCastling&>(other).target2_;
}
void LongCastling::write(std::ostream& output) const {
  output << "LongCastling[origin=" << origin_ << ", target=" << target_
         << ", origin2=" << origin2_ << ", target2=" << target2_ << "]";
}
void LongCastling::preWrite(Position& position, std::ostream& lanBuilder,
                            int translate) const {
  lanBuilder << "0-0-0";
}

ShortCastling::ShortCastling(int origin, int target, int origin2, int target2)
    : Castling(origin, target, origin2, target2) {}
bool ShortCastling::equals(const Move& other) const {
  return origin_ == static_cast<const ShortCastling&>(other).origin_ &&
         target_ == static_cast<const ShortCastling&>(other).target_ &&
         origin2_ == static_cast<const ShortCastling&>(other).origin2_ &&
         target2_ == static_cast<const ShortCastling&>(other).target2_;
}
void ShortCastling::write(std::ostream& output) const {
  output << "ShortCastling[origin=" << origin_ << ", target=" << target_
         << ", origin2=" << origin2_ << ", target2=" << target2_ << "]";
}
void ShortCastling::preWrite(Position& position, std::ostream& lanBuilder,
                             int translate) const {
  lanBuilder << "0-0";
}

DoubleStep::DoubleStep(int origin, int target, int stop)
    : QuietMove(origin, target), stop_(stop) {}
bool DoubleStep::equals(const Move& other) const {
  return origin_ == static_cast<const DoubleStep&>(other).origin_ &&
         target_ == static_cast<const DoubleStep&>(other).target_ &&
         stop_ == static_cast<const DoubleStep&>(other).stop_;
}
void DoubleStep::write(std::ostream& output) const {
  output << "DoubleStep[origin=" << origin_ << ", target=" << target_
         << ", stop=" << stop_ << "]";
}
void DoubleStep::setEnPassant(std::shared_ptr<int>& enPassant) const {
  enPassant = std::make_shared<int>(stop_);
}

EnPassant::EnPassant(int origin, int target, int stop)
    : Capture(origin, target), stop_(stop) {}
bool EnPassant::equals(const Move& other) const {
  return origin_ == static_cast<const EnPassant&>(other).origin_ &&
         target_ == static_cast<const EnPassant&>(other).target_ &&
         stop_ == static_cast<const EnPassant&>(other).stop_;
}
void EnPassant::write(std::ostream& output) const {
  output << "EnPassant[origin=" << origin_ << ", target=" << target_
         << ", stop=" << stop_ << "]";
}
void EnPassant::updatePieces(std::array<std::unique_ptr<Piece>, 128>& board,
                             std::stack<std::unique_ptr<Piece>>& table) const {
  table.push(std::move(board[stop_]));
  board[target_] = std::move(board[origin_]);
}
void EnPassant::revertPieces(std::array<std::unique_ptr<Piece>, 128>& board,
                             std::stack<std::unique_ptr<Piece>>& table) const {
  board[origin_] = std::move(board[target_]);
  board[stop_] = std::move(table.top());
  table.pop();
}
void EnPassant::preWrite(const std::array<std::unique_ptr<Piece>, 128>& board,
                         std::ostream& lanBuilder, int translate) const {
  lanBuilder << board[origin_]->getCode(translate) << toCode(board, origin_)
             << "x" << toCode(board, target_) << " e.p.";
}

Promotion::Promotion(int origin, int target, bool black, int order)
    : QuietMove(origin, target), black_(black), order_(order) {}
bool Promotion::equals(const Move& other) const {
  return origin_ == static_cast<const Promotion&>(other).origin_ &&
         target_ == static_cast<const Promotion&>(other).target_ &&
         black_ == static_cast<const Promotion&>(other).black_ &&
         order_ == static_cast<const Promotion&>(other).order_;
}
void Promotion::write(std::ostream& output) const {
  output << "Promotion[origin=" << origin_ << ", target=" << target_
         << ", black=" << black_ << ", order=" << order_ << "]";
}
void Promotion::updatePieces(Position& position) const {
  updatePieces(position.getBoard(), position.getBox());
}
void Promotion::updatePieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box)
    const {
  box.at(black_).at(order_).push_back(std::move(board[origin_]));
  board[target_] = std::move(box.at(black_).at(order_).front());
  box.at(black_).at(order_).pop_front();
}
void Promotion::revertPieces(Position& position) const {
  revertPieces(position.getBoard(), position.getBox());
}
void Promotion::revertPieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box)
    const {
  box.at(black_).at(order_).push_front(std::move(board[target_]));
  board[origin_] = std::move(box.at(black_).at(order_).back());
  box.at(black_).at(order_).pop_back();
}
void Promotion::preWrite(Position& position, std::ostream& lanBuilder,
                         int translate) const {
  preWrite(position.getBoard(), position.getBox(), lanBuilder, translate);
}
void Promotion::preWrite(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    std::ostream& lanBuilder, int translate) const {
  lanBuilder << board[origin_]->getCode(translate) << toCode(board, origin_)
             << "-" << toCode(board, target_) << "="
             << box.at(black_).at(order_).front()->getCode(translate);
}

PromotionCapture::PromotionCapture(int origin, int target, bool black,
                                   int order)
    : Promotion(origin, target, black, order) {}
bool PromotionCapture::equals(const Move& other) const {
  return origin_ == static_cast<const PromotionCapture&>(other).origin_ &&
         target_ == static_cast<const PromotionCapture&>(other).target_ &&
         black_ == static_cast<const PromotionCapture&>(other).black_ &&
         order_ == static_cast<const PromotionCapture&>(other).order_;
}
void PromotionCapture::write(std::ostream& output) const {
  output << "PromotionCapture[origin=" << origin_ << ", target=" << target_
         << ", black=" << black_ << ", order=" << order_ << "]";
}
void PromotionCapture::updatePieces(Position& position) const {
  updatePieces(position.getBoard(), position.getBox(), position.getTable());
}
void PromotionCapture::updatePieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box,
    std::stack<std::unique_ptr<Piece>>& table) const {
  table.push(std::move(board[target_]));
  box.at(black_).at(order_).push_back(std::move(board[origin_]));
  board[target_] = std::move(box.at(black_).at(order_).front());
  box.at(black_).at(order_).pop_front();
}
void PromotionCapture::revertPieces(Position& position) const {
  revertPieces(position.getBoard(), position.getBox(), position.getTable());
}
void PromotionCapture::revertPieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box,
    std::stack<std::unique_ptr<Piece>>& table) const {
  box.at(black_).at(order_).push_front(std::move(board[target_]));
  board[origin_] = std::move(box.at(black_).at(order_).back());
  box.at(black_).at(order_).pop_back();
  board[target_] = std::move(table.top());
  table.pop();
}
void PromotionCapture::preWrite(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    std::ostream& lanBuilder, int translate) const {
  lanBuilder << board[origin_]->getCode(translate) << toCode(board, origin_)
             << "x" << toCode(board, target_) << "="
             << box.at(black_).at(order_).front()->getCode(translate);
}

std::string toCode(const std::array<std::unique_ptr<Piece>, 128>& board,
                   int square) {
  return std::string()
      .append(1, 'a' + square / 16)
      .append(1, '1' + square % 16);
}

}  // namespace moderato
