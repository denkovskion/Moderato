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

#include "FairyMoves.h"

namespace moderato {

CirceMove::CirceMove(int rebirth, bool castling)
    : rebirth_(rebirth), castling_(castling) {}

CirceCapture::CirceCapture(int origin, int target, int rebirth, bool castling)
    : Capture(origin, target), CirceMove(rebirth, castling) {}
void CirceCapture::write(std::ostream& output) const {
  output << "CirceCapture[origin=" << origin_ << ", target=" << target_
         << ", rebirth=" << rebirth_ << ", castling=" << castling_ << "]";
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
void CirceCapture::updateCastlings(std::set<int>& castlings) const {
  castlings.erase(origin_);
  castlings.erase(target_);
  if (castling_) {
    castlings.insert(rebirth_);
  } else {
    castlings.erase(rebirth_);
  }
}
void CirceCapture::preWrite(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    std::ostream& lanBuilder, int translate) const {
  lanBuilder << board.at(origin_)->getCode(translate) << toCode(board, origin_)
             << "x" << toCode(board, target_) << "("
             << board.at(target_)->getCode(translate) << toCode(board, rebirth_)
             << ")";
}

CirceEnPassant::CirceEnPassant(int origin, int target, int stop, int rebirth,
                               bool castling)
    : EnPassant(origin, target, stop), CirceMove(rebirth, castling) {}
void CirceEnPassant::write(std::ostream& output) const {
  output << "CirceEnPassant[origin=" << origin_ << ", target=" << target_
         << ", stop=" << stop_ << ", rebirth=" << rebirth_
         << ", castling=" << castling_ << "]";
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
void CirceEnPassant::updateCastlings(std::set<int>& castlings) const {
  castlings.erase(origin_);
  castlings.erase(target_);
  castlings.erase(stop_);
  if (castling_) {
    castlings.insert(rebirth_);
  } else {
    castlings.erase(rebirth_);
  }
}
void CirceEnPassant::preWrite(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    std::ostream& lanBuilder, int translate) const {
  lanBuilder << board.at(origin_)->getCode(translate) << toCode(board, origin_)
             << "x" << toCode(board, target_) << " e.p." << "("
             << board.at(stop_)->getCode(translate) << toCode(board, rebirth_)
             << ")";
}

CircePromotionCapture::CircePromotionCapture(int origin, int target, bool black,
                                             int order, int rebirth,
                                             bool castling)
    : PromotionCapture(origin, target, black, order),
      CirceMove(rebirth, castling) {}
void CircePromotionCapture::write(std::ostream& output) const {
  output << "CircePromotionCapture[origin=" << origin_ << ", target=" << target_
         << ", black=" << black_ << ", order=" << order_
         << ", rebirth=" << rebirth_ << ", castling=" << castling_ << "]";
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
void CircePromotionCapture::updateCastlings(std::set<int>& castlings) const {
  castlings.erase(origin_);
  castlings.erase(target_);
  if (castling_) {
    castlings.insert(rebirth_);
  } else {
    castlings.erase(rebirth_);
  }
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

AntiCirceCapture::AntiCirceCapture(int origin, int target, int rebirth,
                                   bool castling)
    : Capture(origin, target), CirceMove(rebirth, castling) {}
void AntiCirceCapture::write(std::ostream& output) const {
  output << "AntiCirceCapture[origin=" << origin_ << ", target=" << target_
         << ", rebirth=" << rebirth_ << ", castling=" << castling_ << "]";
}
void AntiCirceCapture::updatePieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::stack<std::unique_ptr<Piece>>& table) const {
  table.push(std::move(board.at(target_)));
  board.at(rebirth_) = std::move(board.at(origin_));
}
void AntiCirceCapture::revertPieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::stack<std::unique_ptr<Piece>>& table) const {
  board.at(origin_) = std::move(board.at(rebirth_));
  board.at(target_) = std::move(table.top());
  table.pop();
}
void AntiCirceCapture::updateCastlings(std::set<int>& castlings) const {
  castlings.erase(origin_);
  castlings.erase(target_);
  if (castling_) {
    castlings.insert(rebirth_);
  } else {
    castlings.erase(rebirth_);
  }
}
void AntiCirceCapture::preWrite(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    std::ostream& lanBuilder, int translate) const {
  lanBuilder << board.at(origin_)->getCode(translate) << toCode(board, origin_)
             << "x" << toCode(board, target_) << "("
             << board.at(origin_)->getCode(translate) << toCode(board, rebirth_)
             << ")";
}

AntiCirceEnPassant::AntiCirceEnPassant(int origin, int target, int stop,
                                       int rebirth, bool castling)
    : EnPassant(origin, target, stop), CirceMove(rebirth, castling) {}
void AntiCirceEnPassant::write(std::ostream& output) const {
  output << "AntiCirceEnPassant[origin=" << origin_ << ", target=" << target_
         << ", stop=" << stop_ << ", rebirth=" << rebirth_
         << ", castling=" << castling_ << "]";
}
void AntiCirceEnPassant::updatePieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::stack<std::unique_ptr<Piece>>& table) const {
  table.push(std::move(board.at(stop_)));
  board.at(rebirth_) = std::move(board.at(origin_));
}
void AntiCirceEnPassant::revertPieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::stack<std::unique_ptr<Piece>>& table) const {
  board.at(origin_) = std::move(board.at(rebirth_));
  board.at(stop_) = std::move(table.top());
  table.pop();
}
void AntiCirceEnPassant::updateCastlings(std::set<int>& castlings) const {
  castlings.erase(origin_);
  castlings.erase(target_);
  castlings.erase(stop_);
  if (castling_) {
    castlings.insert(rebirth_);
  } else {
    castlings.erase(rebirth_);
  }
}
void AntiCirceEnPassant::preWrite(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    std::ostream& lanBuilder, int translate) const {
  lanBuilder << board.at(origin_)->getCode(translate) << toCode(board, origin_)
             << "x" << toCode(board, target_) << " e.p." << "("
             << board.at(origin_)->getCode(translate) << toCode(board, rebirth_)
             << ")";
}

AntiCircePromotionCapture::AntiCircePromotionCapture(int origin, int target,
                                                     bool black, int order,
                                                     int rebirth, bool castling)
    : PromotionCapture(origin, target, black, order),
      CirceMove(rebirth, castling) {}
void AntiCircePromotionCapture::write(std::ostream& output) const {
  output << "AntiCircePromotionCapture[origin=" << origin_
         << ", target=" << target_ << ", black=" << black_
         << ", order=" << order_ << ", rebirth=" << rebirth_
         << ", castling=" << castling_ << "]";
}
void AntiCircePromotionCapture::updatePieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box,
    std::stack<std::unique_ptr<Piece>>& table) const {
  table.push(std::move(board.at(target_)));
  box.at(black_).at(order_).push_back(std::move(board.at(origin_)));
  board.at(rebirth_) = std::move(box.at(black_).at(order_).front());
  box.at(black_).at(order_).pop_front();
}
void AntiCircePromotionCapture::revertPieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box,
    std::stack<std::unique_ptr<Piece>>& table) const {
  box.at(black_).at(order_).push_front(std::move(board.at(rebirth_)));
  board.at(origin_) = std::move(box.at(black_).at(order_).back());
  box.at(black_).at(order_).pop_back();
  board.at(target_) = std::move(table.top());
  table.pop();
}
void AntiCircePromotionCapture::updateCastlings(
    std::set<int>& castlings) const {
  castlings.erase(origin_);
  castlings.erase(target_);
  if (castling_) {
    castlings.insert(rebirth_);
  } else {
    castlings.erase(rebirth_);
  }
}
void AntiCircePromotionCapture::preWrite(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    std::ostream& lanBuilder, int translate) const {
  lanBuilder << board.at(origin_)->getCode(translate) << toCode(board, origin_)
             << "x" << toCode(board, target_) << "="
             << box.at(black_).at(order_).front()->getCode(translate) << "("
             << box.at(black_).at(order_).front()->getCode(translate)
             << toCode(board, rebirth_) << ")";
}

}  // namespace moderato
