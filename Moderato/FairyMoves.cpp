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

AntiCirceMove::AntiCirceMove(int rebirth, bool castling)
    : rebirth_(rebirth), castling_(castling) {}

AntiCirceCapture::AntiCirceCapture(int origin, int target, int rebirth,
                                   bool castling)
    : Capture(origin, target), AntiCirceMove(rebirth, castling) {}
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
    : EnPassant(origin, target, stop), AntiCirceMove(rebirth, castling) {}
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
      AntiCirceMove(rebirth, castling) {}
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

AndernachMove::AndernachMove(bool castling) : castling_(castling) {}

AndernachCapture::AndernachCapture(int origin, int target, bool castling)
    : Capture(origin, target), AndernachMove(castling) {}
void AndernachCapture::write(std::ostream& output) const {
  output << "AndernachCapture[origin=" << origin_ << ", target=" << target_
         << ", castling=" << castling_ << "]";
}
void AndernachCapture::updatePieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::stack<std::unique_ptr<Piece>>& table) const {
  table.push(std::move(board.at(target_)));
  board.at(target_) = std::move(board.at(origin_));
  board.at(target_)->isBlack() = !board.at(target_)->isBlack();
}
void AndernachCapture::revertPieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::stack<std::unique_ptr<Piece>>& table) const {
  board.at(target_)->isBlack() = !board.at(target_)->isBlack();
  board.at(origin_) = std::move(board.at(target_));
  board.at(target_) = std::move(table.top());
  table.pop();
}
void AndernachCapture::updateCastlings(std::set<int>& castlings) const {
  castlings.erase(origin_);
  if (castling_) {
    castlings.insert(target_);
  } else {
    castlings.erase(target_);
  }
}
void AndernachCapture::preWrite(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    std::ostream& lanBuilder, int translate) const {
  lanBuilder << board.at(origin_)->getCode(translate) << toCode(board, origin_)
             << "x" << toCode(board, target_) << "("
             << toCode(!board.at(origin_)->isBlack(), translate) << ")";
}

AndernachEnPassant::AndernachEnPassant(int origin, int target, int stop,
                                       bool castling)
    : EnPassant(origin, target, stop), AndernachMove(castling) {}
void AndernachEnPassant::write(std::ostream& output) const {
  output << "AndernachEnPassant[origin=" << origin_ << ", target=" << target_
         << ", stop=" << stop_ << ", castling=" << castling_ << "]";
}
void AndernachEnPassant::updatePieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::stack<std::unique_ptr<Piece>>& table) const {
  table.push(std::move(board.at(stop_)));
  board.at(target_) = std::move(board.at(origin_));
  board.at(target_)->isBlack() = !board.at(target_)->isBlack();
}
void AndernachEnPassant::revertPieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::stack<std::unique_ptr<Piece>>& table) const {
  board.at(target_)->isBlack() = !board.at(target_)->isBlack();
  board.at(origin_) = std::move(board.at(target_));
  board.at(stop_) = std::move(table.top());
  table.pop();
}
void AndernachEnPassant::updateCastlings(std::set<int>& castlings) const {
  castlings.erase(origin_);
  if (castling_) {
    castlings.insert(target_);
  } else {
    castlings.erase(target_);
  }
  castlings.erase(stop_);
}
void AndernachEnPassant::preWrite(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    std::ostream& lanBuilder, int translate) const {
  lanBuilder << board.at(origin_)->getCode(translate) << toCode(board, origin_)
             << "x" << toCode(board, target_) << " e.p." << "("
             << toCode(!board.at(origin_)->isBlack(), translate) << ")";
}

AndernachPromotionCapture::AndernachPromotionCapture(int origin, int target,
                                                     bool black, int order,
                                                     bool castling)
    : PromotionCapture(origin, target, black, order), AndernachMove(castling) {}
void AndernachPromotionCapture::write(std::ostream& output) const {
  output << "AndernachPromotionCapture[origin=" << origin_
         << ", target=" << target_ << ", black=" << black_
         << ", order=" << order_ << ", castling=" << castling_ << "]";
}
void AndernachPromotionCapture::updatePieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box,
    std::stack<std::unique_ptr<Piece>>& table) const {
  table.push(std::move(board.at(target_)));
  box.at(black_).at(order_).push_back(std::move(board.at(origin_)));
  board.at(target_) = std::move(box.at(black_).at(order_).front());
  box.at(black_).at(order_).pop_front();
  board.at(target_)->isBlack() = !board.at(target_)->isBlack();
}
void AndernachPromotionCapture::revertPieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box,
    std::stack<std::unique_ptr<Piece>>& table) const {
  board.at(target_)->isBlack() = !board.at(target_)->isBlack();
  box.at(black_).at(order_).push_front(std::move(board.at(target_)));
  board.at(origin_) = std::move(box.at(black_).at(order_).back());
  box.at(black_).at(order_).pop_back();
  board.at(target_) = std::move(table.top());
  table.pop();
}
void AndernachPromotionCapture::updateCastlings(
    std::set<int>& castlings) const {
  castlings.erase(origin_);
  if (castling_) {
    castlings.insert(target_);
  } else {
    castlings.erase(target_);
  }
}
void AndernachPromotionCapture::preWrite(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    std::ostream& lanBuilder, int translate) const {
  lanBuilder << board.at(origin_)->getCode(translate) << toCode(board, origin_)
             << "x" << toCode(board, target_) << "="
             << box.at(black_).at(order_).front()->getCode(translate) << "("
             << toCode(!board.at(origin_)->isBlack(), translate) << ")";
}

AntiAndernachMove::AntiAndernachMove(bool castling) : castling_(castling) {}

AntiAndernachQuietMove::AntiAndernachQuietMove(int origin, int target,
                                               bool castling)
    : QuietMove(origin, target), AntiAndernachMove(castling) {}
void AntiAndernachQuietMove::write(std::ostream& output) const {
  output << "AntiAndernachQuietMove[origin=" << origin_
         << ", target=" << target_ << ", castling=" << castling_ << "]";
}
void AntiAndernachQuietMove::updatePieces(
    std::array<std::unique_ptr<Piece>, 128>& board) const {
  board.at(target_) = std::move(board.at(origin_));
  board.at(target_)->isBlack() = !board.at(target_)->isBlack();
}
void AntiAndernachQuietMove::revertPieces(
    std::array<std::unique_ptr<Piece>, 128>& board) const {
  board.at(target_)->isBlack() = !board.at(target_)->isBlack();
  board.at(origin_) = std::move(board.at(target_));
}
void AntiAndernachQuietMove::updateCastlings(std::set<int>& castlings) const {
  castlings.erase(origin_);
  if (castling_) {
    castlings.insert(target_);
  } else {
    castlings.erase(target_);
  }
}
void AntiAndernachQuietMove::preWrite(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    std::ostream& lanBuilder, int translate) const {
  lanBuilder << board.at(origin_)->getCode(translate) << toCode(board, origin_)
             << "-" << toCode(board, target_) << "("
             << toCode(!board.at(origin_)->isBlack(), translate) << ")";
}

AntiAndernachPromotion::AntiAndernachPromotion(int origin, int target,
                                               bool black, int order,
                                               bool castling)
    : Promotion(origin, target, black, order), AntiAndernachMove(castling) {}
void AntiAndernachPromotion::write(std::ostream& output) const {
  output << "AntiAndernachPromotion[origin=" << origin_
         << ", target=" << target_ << ", black=" << black_
         << ", order=" << order_ << ", castling=" << castling_ << "]";
}
void AntiAndernachPromotion::updatePieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box)
    const {
  box.at(black_).at(order_).push_back(std::move(board.at(origin_)));
  board.at(target_) = std::move(box.at(black_).at(order_).front());
  box.at(black_).at(order_).pop_front();
  board.at(target_)->isBlack() = !board.at(target_)->isBlack();
}
void AntiAndernachPromotion::revertPieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box)
    const {
  board.at(target_)->isBlack() = !board.at(target_)->isBlack();
  box.at(black_).at(order_).push_front(std::move(board.at(target_)));
  board.at(origin_) = std::move(box.at(black_).at(order_).back());
  box.at(black_).at(order_).pop_back();
}
void AntiAndernachPromotion::updateCastlings(std::set<int>& castlings) const {
  castlings.erase(origin_);
  if (castling_) {
    castlings.insert(target_);
  } else {
    castlings.erase(target_);
  }
}
void AntiAndernachPromotion::preWrite(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    std::ostream& lanBuilder, int translate) const {
  lanBuilder << board.at(origin_)->getCode(translate) << toCode(board, origin_)
             << "-" << toCode(board, target_) << "="
             << box.at(black_).at(order_).front()->getCode(translate) << "("
             << toCode(!board.at(origin_)->isBlack(), translate) << ")";
}

CirceAndernachMove::CirceAndernachMove(int rebirth, bool castling,
                                       bool castling2)
    : rebirth_(rebirth), castling_(castling), castling2_(castling2) {}

CirceAndernachCapture::CirceAndernachCapture(int origin, int target,
                                             int rebirth, bool castling,
                                             bool castling2)
    : Capture(origin, target),
      CirceAndernachMove(rebirth, castling, castling2) {}
void CirceAndernachCapture::write(std::ostream& output) const {
  output << "CirceAndernachCapture[origin=" << origin_ << ", target=" << target_
         << ", rebirth=" << rebirth_ << ", castling=" << castling_
         << ", castling2=" << castling2_ << "]";
}
void CirceAndernachCapture::updatePieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::stack<std::unique_ptr<Piece>>& table) const {
  table.push(std::move(board.at(target_)));
  board.at(target_) = std::move(board.at(origin_));
  board.at(rebirth_) = std::move(table.top());
  table.pop();
  board.at(target_)->isBlack() = !board.at(target_)->isBlack();
}
void CirceAndernachCapture::revertPieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::stack<std::unique_ptr<Piece>>& table) const {
  board.at(target_)->isBlack() = !board.at(target_)->isBlack();
  table.push(std::move(board.at(rebirth_)));
  board.at(origin_) = std::move(board.at(target_));
  board.at(target_) = std::move(table.top());
  table.pop();
}
void CirceAndernachCapture::updateCastlings(std::set<int>& castlings) const {
  castlings.erase(origin_);
  if (castling2_) {
    castlings.insert(target_);
  } else {
    castlings.erase(target_);
  }
  if (castling_) {
    castlings.insert(rebirth_);
  } else {
    castlings.erase(rebirth_);
  }
}
void CirceAndernachCapture::preWrite(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    std::ostream& lanBuilder, int translate) const {
  lanBuilder << board.at(origin_)->getCode(translate) << toCode(board, origin_)
             << "x" << toCode(board, target_) << "("
             << toCode(!board.at(origin_)->isBlack(), translate) << ";"
             << board.at(target_)->getCode(translate) << toCode(board, rebirth_)
             << ")";
}

CirceAndernachEnPassant::CirceAndernachEnPassant(int origin, int target,
                                                 int stop, int rebirth,
                                                 bool castling, bool castling2)
    : EnPassant(origin, target, stop),
      CirceAndernachMove(rebirth, castling, castling2) {}
void CirceAndernachEnPassant::write(std::ostream& output) const {
  output << "CirceAndernachEnPassant[origin=" << origin_
         << ", target=" << target_ << ", stop=" << stop_
         << ", rebirth=" << rebirth_ << ", castling=" << castling_
         << ", castling2=" << castling2_ << "]";
}
void CirceAndernachEnPassant::updatePieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::stack<std::unique_ptr<Piece>>& table) const {
  table.push(std::move(board.at(stop_)));
  board.at(target_) = std::move(board.at(origin_));
  board.at(rebirth_) = std::move(table.top());
  table.pop();
  board.at(target_)->isBlack() = !board.at(target_)->isBlack();
}
void CirceAndernachEnPassant::revertPieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::stack<std::unique_ptr<Piece>>& table) const {
  board.at(target_)->isBlack() = !board.at(target_)->isBlack();
  table.push(std::move(board.at(rebirth_)));
  board.at(origin_) = std::move(board.at(target_));
  board.at(stop_) = std::move(table.top());
  table.pop();
}
void CirceAndernachEnPassant::updateCastlings(std::set<int>& castlings) const {
  castlings.erase(origin_);
  if (castling2_) {
    castlings.insert(target_);
  } else {
    castlings.erase(target_);
  }
  castlings.erase(stop_);
  if (castling_) {
    castlings.insert(rebirth_);
  } else {
    castlings.erase(rebirth_);
  }
}
void CirceAndernachEnPassant::preWrite(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    std::ostream& lanBuilder, int translate) const {
  lanBuilder << board.at(origin_)->getCode(translate) << toCode(board, origin_)
             << "x" << toCode(board, target_) << " e.p." << "("
             << toCode(!board.at(origin_)->isBlack(), translate) << ";"
             << board.at(target_)->getCode(translate) << toCode(board, rebirth_)
             << ")";
}

CirceAndernachPromotionCapture::CirceAndernachPromotionCapture(
    int origin, int target, bool black, int order, int rebirth, bool castling,
    bool castling2)
    : PromotionCapture(origin, target, black, order),
      CirceAndernachMove(rebirth, castling, castling2) {}
void CirceAndernachPromotionCapture::write(std::ostream& output) const {
  output << "CirceAndernachPromotionCapture[origin=" << origin_
         << ", target=" << target_ << ", black=" << black_
         << ", order=" << order_ << ", rebirth=" << rebirth_
         << ", castling=" << castling_ << ", castling2=" << castling2_ << "]";
}
void CirceAndernachPromotionCapture::updatePieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box,
    std::stack<std::unique_ptr<Piece>>& table) const {
  table.push(std::move(board.at(target_)));
  box.at(black_).at(order_).push_back(std::move(board.at(origin_)));
  board.at(target_) = std::move(box.at(black_).at(order_).front());
  box.at(black_).at(order_).pop_front();
  board.at(rebirth_) = std::move(table.top());
  table.pop();
  board.at(target_)->isBlack() = !board.at(target_)->isBlack();
}
void CirceAndernachPromotionCapture::revertPieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box,
    std::stack<std::unique_ptr<Piece>>& table) const {
  board.at(target_)->isBlack() = !board.at(target_)->isBlack();
  table.push(std::move(board.at(rebirth_)));
  box.at(black_).at(order_).push_front(std::move(board.at(target_)));
  board.at(origin_) = std::move(box.at(black_).at(order_).back());
  box.at(black_).at(order_).pop_back();
  board.at(target_) = std::move(table.top());
  table.pop();
}
void CirceAndernachPromotionCapture::updateCastlings(
    std::set<int>& castlings) const {
  castlings.erase(origin_);
  if (castling2_) {
    castlings.insert(target_);
  } else {
    castlings.erase(target_);
  }
  if (castling_) {
    castlings.insert(rebirth_);
  } else {
    castlings.erase(rebirth_);
  }
}
void CirceAndernachPromotionCapture::preWrite(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    std::ostream& lanBuilder, int translate) const {
  lanBuilder << board.at(origin_)->getCode(translate) << toCode(board, origin_)
             << "x" << toCode(board, target_) << "="
             << box.at(black_).at(order_).front()->getCode(translate) << "("
             << toCode(!board.at(origin_)->isBlack(), translate) << ";"
             << board.at(target_)->getCode(translate) << toCode(board, rebirth_)
             << ")";
}

AntiCirceAndernachMove::AntiCirceAndernachMove(int rebirth, bool castling)
    : rebirth_(rebirth), castling_(castling) {}

AntiCirceAndernachCapture::AntiCirceAndernachCapture(int origin, int target,
                                                     int rebirth, bool castling)
    : Capture(origin, target), AntiCirceAndernachMove(rebirth, castling) {}
void AntiCirceAndernachCapture::write(std::ostream& output) const {
  output << "AntiCirceAndernachCapture[origin=" << origin_
         << ", target=" << target_ << ", rebirth=" << rebirth_
         << ", castling=" << castling_ << "]";
}
void AntiCirceAndernachCapture::updatePieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::stack<std::unique_ptr<Piece>>& table) const {
  table.push(std::move(board.at(target_)));
  board.at(rebirth_) = std::move(board.at(origin_));
  board.at(rebirth_)->isBlack() = !board.at(rebirth_)->isBlack();
}
void AntiCirceAndernachCapture::revertPieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::stack<std::unique_ptr<Piece>>& table) const {
  board.at(rebirth_)->isBlack() = !board.at(rebirth_)->isBlack();
  board.at(origin_) = std::move(board.at(rebirth_));
  board.at(target_) = std::move(table.top());
  table.pop();
}
void AntiCirceAndernachCapture::updateCastlings(
    std::set<int>& castlings) const {
  castlings.erase(origin_);
  castlings.erase(target_);
  if (castling_) {
    castlings.insert(rebirth_);
  } else {
    castlings.erase(rebirth_);
  }
}
void AntiCirceAndernachCapture::preWrite(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    std::ostream& lanBuilder, int translate) const {
  lanBuilder << board.at(origin_)->getCode(translate) << toCode(board, origin_)
             << "x" << toCode(board, target_) << "("
             << board.at(origin_)->getCode(translate) << toCode(board, rebirth_)
             << toCode(!board.at(origin_)->isBlack(), translate) << ")";
}

AntiCirceAndernachEnPassant::AntiCirceAndernachEnPassant(int origin, int target,
                                                         int stop, int rebirth,
                                                         bool castling)
    : EnPassant(origin, target, stop),
      AntiCirceAndernachMove(rebirth, castling) {}
void AntiCirceAndernachEnPassant::write(std::ostream& output) const {
  output << "AntiCirceAndernachEnPassant[origin=" << origin_
         << ", target=" << target_ << ", stop=" << stop_
         << ", rebirth=" << rebirth_ << ", castling=" << castling_ << "]";
}
void AntiCirceAndernachEnPassant::updatePieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::stack<std::unique_ptr<Piece>>& table) const {
  table.push(std::move(board.at(stop_)));
  board.at(rebirth_) = std::move(board.at(origin_));
  board.at(rebirth_)->isBlack() = !board.at(rebirth_)->isBlack();
}
void AntiCirceAndernachEnPassant::revertPieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::stack<std::unique_ptr<Piece>>& table) const {
  board.at(rebirth_)->isBlack() = !board.at(rebirth_)->isBlack();
  board.at(origin_) = std::move(board.at(rebirth_));
  board.at(stop_) = std::move(table.top());
  table.pop();
}
void AntiCirceAndernachEnPassant::updateCastlings(
    std::set<int>& castlings) const {
  castlings.erase(origin_);
  castlings.erase(target_);
  castlings.erase(stop_);
  if (castling_) {
    castlings.insert(rebirth_);
  } else {
    castlings.erase(rebirth_);
  }
}
void AntiCirceAndernachEnPassant::preWrite(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    std::ostream& lanBuilder, int translate) const {
  lanBuilder << board.at(origin_)->getCode(translate) << toCode(board, origin_)
             << "x" << toCode(board, target_) << " e.p." << "("
             << board.at(origin_)->getCode(translate) << toCode(board, rebirth_)
             << toCode(!board.at(origin_)->isBlack(), translate) << ")";
}

AntiCirceAndernachPromotionCapture::AntiCirceAndernachPromotionCapture(
    int origin, int target, bool black, int order, int rebirth, bool castling)
    : PromotionCapture(origin, target, black, order),
      AntiCirceAndernachMove(rebirth, castling) {}
void AntiCirceAndernachPromotionCapture::write(std::ostream& output) const {
  output << "AntiCirceAndernachPromotionCapture[origin=" << origin_
         << ", target=" << target_ << ", black=" << black_
         << ", order=" << order_ << ", rebirth=" << rebirth_
         << ", castling=" << castling_ << "]";
}
void AntiCirceAndernachPromotionCapture::updatePieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box,
    std::stack<std::unique_ptr<Piece>>& table) const {
  table.push(std::move(board.at(target_)));
  box.at(black_).at(order_).push_back(std::move(board.at(origin_)));
  board.at(rebirth_) = std::move(box.at(black_).at(order_).front());
  box.at(black_).at(order_).pop_front();
  board.at(rebirth_)->isBlack() = !board.at(rebirth_)->isBlack();
}
void AntiCirceAndernachPromotionCapture::revertPieces(
    std::array<std::unique_ptr<Piece>, 128>& board,
    std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>& box,
    std::stack<std::unique_ptr<Piece>>& table) const {
  board.at(rebirth_)->isBlack() = !board.at(rebirth_)->isBlack();
  box.at(black_).at(order_).push_front(std::move(board.at(rebirth_)));
  board.at(origin_) = std::move(box.at(black_).at(order_).back());
  box.at(black_).at(order_).pop_back();
  board.at(target_) = std::move(table.top());
  table.pop();
}
void AntiCirceAndernachPromotionCapture::updateCastlings(
    std::set<int>& castlings) const {
  castlings.erase(origin_);
  castlings.erase(target_);
  if (castling_) {
    castlings.insert(rebirth_);
  } else {
    castlings.erase(rebirth_);
  }
}
void AntiCirceAndernachPromotionCapture::preWrite(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    std::ostream& lanBuilder, int translate) const {
  lanBuilder << board.at(origin_)->getCode(translate) << toCode(board, origin_)
             << "x" << toCode(board, target_) << "="
             << box.at(black_).at(order_).front()->getCode(translate) << "("
             << box.at(black_).at(order_).front()->getCode(translate)
             << toCode(board, rebirth_)
             << toCode(!board.at(origin_)->isBlack(), translate) << ")";
}

}  // namespace moderato
