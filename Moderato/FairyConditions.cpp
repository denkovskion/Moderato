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

CirceCapture::CirceCapture(int origin, int target, int rebirth)
    : Capture(origin, target), rebirth_(rebirth) {}
void CirceCapture::write(std::ostream& output) const {
  output << "CirceCapture[origin=" << origin_ << ", target=" << target_
         << ", rebirth=" << rebirth_ << "]";
}
void CirceCapture::updatePieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::stack<std::unique_ptr<Piece>>& table) const {
  table.push(std::move(board.at(target_)));
  board.at(target_) = std::move(board.at(origin_));
  board.at(rebirth_) = std::move(table.top());
  table.pop();
}
void CirceCapture::revertPieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::stack<std::unique_ptr<Piece>>& table) const {
  table.push(std::move(board.at(rebirth_)));
  board.at(origin_) = std::move(board.at(target_));
  board.at(target_) = std::move(table.top());
  table.pop();
}
void CirceCapture::preWrite(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    std::ostream& lanBuilder, int translate) const {
  lanBuilder << board.at(origin_)->getCode(translate) << toCode(board, origin_)
             << "x" << toCode(board, target_) << "("
             << board.at(target_)->getCode(translate) << toCode(board, rebirth_)
             << ")";
}

CirceCaptureCastling::CirceCaptureCastling(int origin, int target, int rebirth)
    : CirceCapture(origin, target, rebirth) {}
void CirceCaptureCastling::write(std::ostream& output) const {
  output << "CirceCaptureCastling[origin=" << origin_ << ", target=" << target_
         << ", rebirth=" << rebirth_ << "]";
}
void CirceCaptureCastling::updateCastlings(std::set<int>& castlings) const {
  castlings.erase(origin_);
  castlings.erase(target_);
  castlings.insert(rebirth_);
}

CirceEnPassant::CirceEnPassant(int origin, int target, int stop, int rebirth)
    : EnPassant(origin, target, stop), rebirth_(rebirth) {}
void CirceEnPassant::write(std::ostream& output) const {
  output << "CirceEnPassant[origin=" << origin_ << ", target=" << target_
         << ", stop=" << stop_ << ", rebirth=" << rebirth_ << "]";
}
void CirceEnPassant::updatePieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::stack<std::unique_ptr<Piece>>& table) const {
  table.push(std::move(board.at(stop_)));
  board.at(target_) = std::move(board.at(origin_));
  board.at(rebirth_) = std::move(table.top());
  table.pop();
}
void CirceEnPassant::revertPieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::stack<std::unique_ptr<Piece>>& table) const {
  table.push(std::move(board.at(rebirth_)));
  board.at(origin_) = std::move(board.at(target_));
  board.at(stop_) = std::move(table.top());
  table.pop();
}
void CirceEnPassant::preWrite(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    std::ostream& lanBuilder, int translate) const {
  lanBuilder << board.at(origin_)->getCode(translate) << toCode(board, origin_)
             << "x" << toCode(board, target_) << " e.p." << "("
             << board.at(stop_)->getCode(translate) << toCode(board, rebirth_)
             << ")";
}

CirceEnPassantCastling::CirceEnPassantCastling(int origin, int target, int stop,
                                               int rebirth)
    : CirceEnPassant(origin, target, stop, rebirth) {}
void CirceEnPassantCastling::write(std::ostream& output) const {
  output << "CirceEnPassantCastling[origin=" << origin_
         << ", target=" << target_ << ", stop=" << stop_
         << ", rebirth=" << rebirth_ << "]";
}
void CirceEnPassantCastling::updateCastlings(std::set<int>& castlings) const {
  castlings.erase(origin_);
  castlings.erase(target_);
  castlings.erase(stop_);
  castlings.insert(rebirth_);
}

CircePromotionCapture::CircePromotionCapture(int origin, int target, bool black,
                                             int order, int rebirth)
    : PromotionCapture(origin, target, black, order), rebirth_(rebirth) {}
void CircePromotionCapture::write(std::ostream& output) const {
  output << "CircePromotionCapture[origin=" << origin_ << ", target=" << target_
         << ", black=" << black_ << ", order=" << order_
         << ", rebirth=" << rebirth_ << "]";
}
void CircePromotionCapture::updatePieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box,
    std::stack<std::unique_ptr<Piece>>& table) const {
  table.push(std::move(board.at(target_)));
  box.at(black_).at(order_).push_back(std::move(board.at(origin_)));
  board.at(target_) = std::move(box.at(black_).at(order_).front());
  box.at(black_).at(order_).pop_front();
  board.at(rebirth_) = std::move(table.top());
  table.pop();
}
void CircePromotionCapture::revertPieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box,
    std::stack<std::unique_ptr<Piece>>& table) const {
  table.push(std::move(board.at(rebirth_)));
  box.at(black_).at(order_).push_front(std::move(board.at(target_)));
  board.at(origin_) = std::move(box.at(black_).at(order_).back());
  box.at(black_).at(order_).pop_back();
  board.at(target_) = std::move(table.top());
  table.pop();
}
void CircePromotionCapture::preWrite(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    std::ostream& lanBuilder, int translate) const {
  lanBuilder << board.at(origin_)->getCode(translate) << toCode(board, origin_)
             << "x" << toCode(board, target_) << "="
             << box.at(black_).at(order_).front()->getCode(translate) << "("
             << board.at(target_)->getCode(translate) << toCode(board, rebirth_)
             << ")";
}

CircePromotionCaptureCastling::CircePromotionCaptureCastling(
    int origin, int target, bool black, int order, int rebirth)
    : CircePromotionCapture(origin, target, black, order, rebirth) {}
void CircePromotionCaptureCastling::write(std::ostream& output) const {
  output << "CircePromotionCaptureCastling[origin=" << origin_
         << ", target=" << target_ << ", black=" << black_
         << ", order=" << order_ << ", rebirth=" << rebirth_ << "]";
}
void CircePromotionCaptureCastling::updateCastlings(
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
    if (board.at(target)->isCastling()) {
      moves.push_back(
          std::make_shared<CirceCaptureCastling>(origin, target, rebirth));
    } else {
      moves.push_back(std::make_shared<CirceCapture>(origin, target, rebirth));
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
    if (board.at(stop)->isCastling()) {
      moves.push_back(std::make_shared<CirceEnPassantCastling>(origin, target,
                                                               stop, rebirth));
    } else {
      moves.push_back(
          std::make_shared<CirceEnPassant>(origin, target, stop, rebirth));
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
    if (board.at(target)->isCastling()) {
      moves.push_back(std::make_shared<CircePromotionCaptureCastling>(
          origin, target, black, order, rebirth));
    } else {
      moves.push_back(std::make_shared<CircePromotionCapture>(
          origin, target, black, order, rebirth));
    }
  } else {
    moves.push_back(
        std::make_shared<PromotionCapture>(origin, target, black, order));
  }
}

}  // namespace moderato
