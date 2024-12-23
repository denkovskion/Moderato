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

#include "FairyConditions.h"

namespace moderato {

CaptureRebirth::CaptureRebirth(int origin, int target, int rebirth)
    : Capture(origin, target), rebirth_(rebirth) {}
void CaptureRebirth::write(std::ostream& output) const {
  output << "CaptureRebirth[origin=" << origin_ << ", target=" << target_
         << ", rebirth=" << rebirth_ << "]";
}
void CaptureRebirth::updatePieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::stack<std::unique_ptr<Piece>>& table) const {
  table.push(std::move(board[target_]));
  board[target_] = std::move(board[origin_]);
  board[rebirth_] = std::move(table.top());
  table.pop();
}
void CaptureRebirth::revertPieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::stack<std::unique_ptr<Piece>>& table) const {
  table.push(std::move(board[rebirth_]));
  board[origin_] = std::move(board[target_]);
  board[target_] = std::move(table.top());
  table.pop();
}
void CaptureRebirth::preWrite(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    std::ostream& lanBuilder, int translate) const {
  lanBuilder << board[origin_]->getCode(translate) << toCode(board, origin_)
             << "x" << toCode(board, target_) << "("
             << board[target_]->getCode(translate) << toCode(board, rebirth_)
             << ")";
}

CaptureRebirthCastling::CaptureRebirthCastling(int origin, int target,
                                               int rebirth)
    : CaptureRebirth(origin, target, rebirth) {}
void CaptureRebirthCastling::write(std::ostream& output) const {
  output << "CaptureRebirthCastling[origin=" << origin_
         << ", target=" << target_ << ", rebirth=" << rebirth_ << "]";
}
void CaptureRebirthCastling::updateCastlings(std::set<int>& castlings) const {
  castlings.erase(origin_);
  castlings.erase(target_);
  castlings.insert(rebirth_);
}

EnPassantRebirth::EnPassantRebirth(int origin, int target, int stop,
                                   int rebirth)
    : EnPassant(origin, target, stop), rebirth_(rebirth) {}
void EnPassantRebirth::write(std::ostream& output) const {
  output << "EnPassantRebirth[origin=" << origin_ << ", target=" << target_
         << ", stop=" << stop_ << ", rebirth=" << rebirth_ << "]";
}
void EnPassantRebirth::updatePieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::stack<std::unique_ptr<Piece>>& table) const {
  table.push(std::move(board[stop_]));
  board[target_] = std::move(board[origin_]);
  board[rebirth_] = std::move(table.top());
  table.pop();
}
void EnPassantRebirth::revertPieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::stack<std::unique_ptr<Piece>>& table) const {
  table.push(std::move(board[rebirth_]));
  board[origin_] = std::move(board[target_]);
  board[stop_] = std::move(table.top());
  table.pop();
}
void EnPassantRebirth::preWrite(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    std::ostream& lanBuilder, int translate) const {
  lanBuilder << board[origin_]->getCode(translate) << toCode(board, origin_)
             << "x" << toCode(board, target_) << " e.p." << "("
             << board[stop_]->getCode(translate) << toCode(board, rebirth_)
             << ")";
}

EnPassantRebirthCastling::EnPassantRebirthCastling(int origin, int target,
                                                   int stop, int rebirth)
    : EnPassantRebirth(origin, target, stop, rebirth) {}
void EnPassantRebirthCastling::write(std::ostream& output) const {
  output << "EnPassantRebirthCastling[origin=" << origin_
         << ", target=" << target_ << ", stop=" << stop_
         << ", rebirth=" << rebirth_ << "]";
}
void EnPassantRebirthCastling::updateCastlings(std::set<int>& castlings) const {
  castlings.erase(origin_);
  castlings.erase(target_);
  castlings.erase(stop_);
  castlings.insert(rebirth_);
}

PromotionCaptureRebirth::PromotionCaptureRebirth(int origin, int target,
                                                 bool black, int order,
                                                 int rebirth)
    : PromotionCapture(origin, target, black, order), rebirth_(rebirth) {}
void PromotionCaptureRebirth::write(std::ostream& output) const {
  output << "PromotionCaptureRebirth[origin=" << origin_
         << ", target=" << target_ << ", black=" << black_
         << ", order=" << order_ << ", rebirth=" << rebirth_ << "]";
}
void PromotionCaptureRebirth::updatePieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box,
    std::stack<std::unique_ptr<Piece>>& table) const {
  table.push(std::move(board[target_]));
  box.at(black_).at(order_).push_back(std::move(board[origin_]));
  board[target_] = std::move(box.at(black_).at(order_).front());
  box.at(black_).at(order_).pop_front();
  board[rebirth_] = std::move(table.top());
  table.pop();
}
void PromotionCaptureRebirth::revertPieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box,
    std::stack<std::unique_ptr<Piece>>& table) const {
  table.push(std::move(board[rebirth_]));
  box.at(black_).at(order_).push_front(std::move(board[target_]));
  board[origin_] = std::move(box.at(black_).at(order_).back());
  box.at(black_).at(order_).pop_back();
  board[target_] = std::move(table.top());
  table.pop();
}
void PromotionCaptureRebirth::preWrite(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    std::ostream& lanBuilder, int translate) const {
  lanBuilder << board[origin_]->getCode(translate) << toCode(board, origin_)
             << "x" << toCode(board, target_) << "="
             << box.at(black_).at(order_).front()->getCode(translate) << "("
             << board[target_]->getCode(translate) << toCode(board, rebirth_)
             << ")";
}

PromotionCaptureRebirthCastling::PromotionCaptureRebirthCastling(
    int origin, int target, bool black, int order, int rebirth)
    : PromotionCaptureRebirth(origin, target, black, order, rebirth) {}
void PromotionCaptureRebirthCastling::write(std::ostream& output) const {
  output << "PromotionCaptureRebirthCastling[origin=" << origin_
         << ", target=" << target_ << ", black=" << black_
         << ", order=" << order_ << ", rebirth=" << rebirth_ << "]";
}
void PromotionCaptureRebirthCastling::updateCastlings(
    std::set<int>& castlings) const {
  castlings.erase(origin_);
  castlings.erase(target_);
  castlings.insert(rebirth_);
}

void CirceMoveFactory::write(std::ostream& output) const {
  output << "CirceMoveFactory[]";
}
void CirceMoveFactory::generateCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, std::vector<std::shared_ptr<Move>>& moves) const {
  int rebirth = board.at(target)->findRebirthSquare(board, target);
  if (!board.at(rebirth) || rebirth == origin) {
    if (board[target]->isCastling()) {
      moves.push_back(
          std::make_shared<CaptureRebirthCastling>(origin, target, rebirth));
    } else {
      moves.push_back(
          std::make_shared<CaptureRebirth>(origin, target, rebirth));
    }
  } else {
    moves.push_back(std::make_shared<Capture>(origin, target));
  }
}
void CirceMoveFactory::generateEnPassant(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int stop, std::vector<std::shared_ptr<Move>>& moves) const {
  int rebirth = board.at(stop)->findRebirthSquare(board, stop);
  if (!board.at(rebirth) || rebirth == origin) {
    if (board[stop]->isCastling()) {
      moves.push_back(std::make_shared<EnPassantRebirthCastling>(
          origin, target, stop, rebirth));
    } else {
      moves.push_back(
          std::make_shared<EnPassantRebirth>(origin, target, stop, rebirth));
    }
  } else {
    moves.push_back(std::make_shared<EnPassant>(origin, target, stop));
  }
}
void CirceMoveFactory::generatePromotionCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    int origin, int target, bool black, int order,
    std::vector<std::shared_ptr<Move>>& moves) const {
  int rebirth = board.at(target)->findRebirthSquare(board, target);
  if (!board.at(rebirth) || rebirth == origin) {
    if (board[target]->isCastling()) {
      moves.push_back(std::make_shared<PromotionCaptureRebirthCastling>(
          origin, target, black, order, rebirth));
    } else {
      moves.push_back(std::make_shared<PromotionCaptureRebirth>(
          origin, target, black, order, rebirth));
    }
  } else {
    moves.push_back(
        std::make_shared<PromotionCapture>(origin, target, black, order));
  }
}

}  // namespace moderato
