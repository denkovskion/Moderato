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

#include "FairyConditions.h"

#include "FairyMoves.h"

namespace moderato {

void CirceMoveFactory::write(std::ostream& output) const {
  output << "CirceMoveFactory[]";
}
bool CirceMoveFactory::generateCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, std::vector<std::unique_ptr<Move>>& moves) const {
  const std::unique_ptr<Piece>& piece = board.at(target);
  if (piece->isRoyal()) {
    return false;
  }
  int rebirth = piece->findRebirthSquare(board, target);
  if (!board.at(rebirth) || rebirth == origin) {
    if (piece->isCastling()) {
      moves.push_back(
          std::make_unique<CirceCaptureCastling>(origin, target, rebirth));
    } else {
      moves.push_back(std::make_unique<CirceCapture>(origin, target, rebirth));
    }
  } else {
    moves.push_back(std::make_unique<Capture>(origin, target));
  }
  return true;
}
bool CirceMoveFactory::generateEnPassant(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int stop, std::vector<std::unique_ptr<Move>>& moves) const {
  const std::unique_ptr<Piece>& piece = board.at(stop);
  if (piece->isRoyal()) {
    return false;
  }
  int rebirth = piece->findRebirthSquare(board, stop);
  if ((!board.at(rebirth) || rebirth == origin || rebirth == stop) &&
      !(rebirth == target)) {
    if (piece->isCastling()) {
      moves.push_back(std::make_unique<CirceEnPassantCastling>(origin, target,
                                                               stop, rebirth));
    } else {
      moves.push_back(
          std::make_unique<CirceEnPassant>(origin, target, stop, rebirth));
    }
  } else {
    moves.push_back(std::make_unique<EnPassant>(origin, target, stop));
  }
  return true;
}
bool CirceMoveFactory::generatePromotionCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    int origin, int target, bool black, int order,
    std::vector<std::unique_ptr<Move>>& moves) const {
  const std::unique_ptr<Piece>& piece = board.at(target);
  if (piece->isRoyal()) {
    return false;
  }
  int rebirth = piece->findRebirthSquare(board, target);
  if (!board.at(rebirth) || rebirth == origin) {
    if (piece->isCastling()) {
      moves.push_back(std::make_unique<CircePromotionCaptureCastling>(
          origin, target, black, order, rebirth));
    } else {
      moves.push_back(std::make_unique<CircePromotionCapture>(
          origin, target, black, order, rebirth));
    }
  } else {
    moves.push_back(
        std::make_unique<PromotionCapture>(origin, target, black, order));
  }
  return true;
}

void NoCaptureMoveFactory::write(std::ostream& output) const {
  output << "NoCaptureMoveFactory[]";
}
bool NoCaptureMoveFactory::generateCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, std::vector<std::unique_ptr<Move>>& moves) const {
  return !board.at(target)->isRoyal();
}
bool NoCaptureMoveFactory::generateEnPassant(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int stop, std::vector<std::unique_ptr<Move>>& moves) const {
  return !board.at(stop)->isRoyal();
}
bool NoCaptureMoveFactory::generatePromotionCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    int origin, int target, bool black, int order,
    std::vector<std::unique_ptr<Move>>& moves) const {
  return !board.at(target)->isRoyal();
}

void AntiCirceMoveFactory::write(std::ostream& output) const {
  output << "AntiCirceMoveFactory[]";
}
bool AntiCirceMoveFactory::generateCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, std::vector<std::unique_ptr<Move>>& moves) const {
  const std::unique_ptr<Piece>& piece = board.at(origin);
  int rebirth = piece->findRebirthSquare(board, target);
  if (!board.at(rebirth) || rebirth == origin) {
    if (board.at(target)->isRoyal()) {
      return false;
    }
    if (piece->isCastling()) {
      moves.push_back(
          std::make_unique<AntiCirceCaptureCastling>(origin, target, rebirth));
    } else {
      moves.push_back(
          std::make_unique<AntiCirceCapture>(origin, target, rebirth));
    }
  }
  return true;
}
bool AntiCirceMoveFactory::generateCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target) const {
  const std::unique_ptr<Piece>& piece = board.at(origin);
  int rebirth = piece->findRebirthSquare(board, target);
  if (!board.at(rebirth) || rebirth == origin) {
    if (board.at(target)->isRoyal()) {
      return false;
    }
  }
  return true;
}
bool AntiCirceMoveFactory::generateEnPassant(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int stop, std::vector<std::unique_ptr<Move>>& moves) const {
  const std::unique_ptr<Piece>& piece = board.at(origin);
  int rebirth = piece->findRebirthSquare(board, target);
  if ((!board.at(rebirth) || rebirth == origin || rebirth == stop) &&
      !(rebirth == target)) {
    if (board.at(stop)->isRoyal()) {
      return false;
    }
    if (piece->isCastling()) {
      moves.push_back(std::make_unique<AntiCirceEnPassantCastling>(
          origin, target, stop, rebirth));
    } else {
      moves.push_back(
          std::make_unique<AntiCirceEnPassant>(origin, target, stop, rebirth));
    }
  }
  return true;
}
bool AntiCirceMoveFactory::generateEnPassant(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int stop) const {
  const std::unique_ptr<Piece>& piece = board.at(origin);
  int rebirth = piece->findRebirthSquare(board, target);
  if ((!board.at(rebirth) || rebirth == origin || rebirth == stop) &&
      !(rebirth == target)) {
    if (board.at(stop)->isRoyal()) {
      return false;
    }
  }
  return true;
}
bool AntiCirceMoveFactory::generatePromotionCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    int origin, int target, bool black, int order,
    std::vector<std::unique_ptr<Move>>& moves) const {
  const std::unique_ptr<Piece>& piece = box.at(black).at(order).front();
  int rebirth = piece->findRebirthSquare(board, target);
  if (!board.at(rebirth) || rebirth == origin) {
    if (board.at(target)->isRoyal()) {
      return false;
    }
    if (piece->isCastling()) {
      moves.push_back(std::make_unique<AntiCircePromotionCaptureCastling>(
          origin, target, black, order, rebirth));
    } else {
      moves.push_back(std::make_unique<AntiCircePromotionCapture>(
          origin, target, black, order, rebirth));
    }
  }
  return true;
}
bool AntiCirceMoveFactory::generatePromotionCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    int origin, int target, bool black, int order) const {
  const std::unique_ptr<Piece>& piece = box.at(black).at(order).front();
  int rebirth = piece->findRebirthSquare(board, target);
  if (!board.at(rebirth) || rebirth == origin) {
    if (board.at(target)->isRoyal()) {
      return false;
    }
  }
  return true;
}

void AntiCirceCaptureRebirthMoveFactory::write(std::ostream& output) const {
  output << "AntiCirceCaptureRebirthMoveFactory[]";
}
bool AntiCirceCaptureRebirthMoveFactory::generateCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, std::vector<std::unique_ptr<Move>>& moves) const {
  const std::unique_ptr<Piece>& piece = board.at(origin);
  int rebirth = piece->findRebirthSquare(board, target);
  if (!board.at(rebirth) || rebirth == origin || rebirth == target) {
    if (board.at(target)->isRoyal()) {
      return false;
    }
    if (piece->isCastling()) {
      moves.push_back(
          std::make_unique<AntiCirceCaptureCastling>(origin, target, rebirth));
    } else {
      moves.push_back(
          std::make_unique<AntiCirceCapture>(origin, target, rebirth));
    }
  }
  return true;
}
bool AntiCirceCaptureRebirthMoveFactory::generateCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target) const {
  const std::unique_ptr<Piece>& piece = board.at(origin);
  int rebirth = piece->findRebirthSquare(board, target);
  if (!board.at(rebirth) || rebirth == origin || rebirth == target) {
    if (board.at(target)->isRoyal()) {
      return false;
    }
  }
  return true;
}
bool AntiCirceCaptureRebirthMoveFactory::generateEnPassant(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int stop, std::vector<std::unique_ptr<Move>>& moves) const {
  const std::unique_ptr<Piece>& piece = board.at(origin);
  int rebirth = piece->findRebirthSquare(board, target);
  if (!board.at(rebirth) || rebirth == origin || rebirth == stop) {
    if (board.at(stop)->isRoyal()) {
      return false;
    }
    if (piece->isCastling()) {
      moves.push_back(std::make_unique<AntiCirceEnPassantCastling>(
          origin, target, stop, rebirth));
    } else {
      moves.push_back(
          std::make_unique<AntiCirceEnPassant>(origin, target, stop, rebirth));
    }
  }
  return true;
}
bool AntiCirceCaptureRebirthMoveFactory::generateEnPassant(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int stop) const {
  const std::unique_ptr<Piece>& piece = board.at(origin);
  int rebirth = piece->findRebirthSquare(board, target);
  if (!board.at(rebirth) || rebirth == origin || rebirth == stop) {
    if (board.at(stop)->isRoyal()) {
      return false;
    }
  }
  return true;
}
bool AntiCirceCaptureRebirthMoveFactory::generatePromotionCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    int origin, int target, bool black, int order,
    std::vector<std::unique_ptr<Move>>& moves) const {
  const std::unique_ptr<Piece>& piece = box.at(black).at(order).front();
  int rebirth = piece->findRebirthSquare(board, target);
  if (!board.at(rebirth) || rebirth == origin || rebirth == target) {
    if (board.at(target)->isRoyal()) {
      return false;
    }
    if (piece->isCastling()) {
      moves.push_back(std::make_unique<AntiCircePromotionCaptureCastling>(
          origin, target, black, order, rebirth));
    } else {
      moves.push_back(std::make_unique<AntiCircePromotionCapture>(
          origin, target, black, order, rebirth));
    }
  }
  return true;
}
bool AntiCirceCaptureRebirthMoveFactory::generatePromotionCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    int origin, int target, bool black, int order) const {
  const std::unique_ptr<Piece>& piece = box.at(black).at(order).front();
  int rebirth = piece->findRebirthSquare(board, target);
  if (!board.at(rebirth) || rebirth == origin || rebirth == target) {
    if (board.at(target)->isRoyal()) {
      return false;
    }
  }
  return true;
}

}  // namespace moderato
