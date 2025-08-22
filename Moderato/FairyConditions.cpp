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
  int rebirth = piece->findRebirthSquare(board, target, false);
  if (!board.at(rebirth) || rebirth == origin) {
    bool castling = piece->isCastling();
    moves.push_back(
        std::make_unique<CirceCapture>(origin, target, rebirth, castling));
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
  int rebirth = piece->findRebirthSquare(board, stop, false);
  if ((!board.at(rebirth) || rebirth == origin || rebirth == stop) &&
      !(rebirth == target)) {
    bool castling = piece->isCastling();
    moves.push_back(std::make_unique<CirceEnPassant>(origin, target, stop,
                                                     rebirth, castling));
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
  int rebirth = piece->findRebirthSquare(board, target, false);
  if (!board.at(rebirth) || rebirth == origin) {
    bool castling = piece->isCastling();
    moves.push_back(std::make_unique<CircePromotionCapture>(
        origin, target, black, order, rebirth, castling));
  } else {
    moves.push_back(
        std::make_unique<PromotionCapture>(origin, target, black, order));
  }
  return true;
}

AntiCirceMoveFactory::AntiCirceMoveFactory(bool calvet) : calvet_(calvet) {}
void AntiCirceMoveFactory::write(std::ostream& output) const {
  output << "AntiCirceMoveFactory[calvet=" << calvet_ << "]";
}
bool AntiCirceMoveFactory::generateCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, std::vector<std::unique_ptr<Move>>& moves) const {
  const std::unique_ptr<Piece>& piece = board.at(origin);
  int rebirth = piece->findRebirthSquare(board, target, false);
  if (!board.at(rebirth) || rebirth == origin || calvet_ && rebirth == target) {
    if (board.at(target)->isRoyal()) {
      return false;
    }
    bool castling = piece->isCastling();
    moves.push_back(
        std::make_unique<AntiCirceCapture>(origin, target, rebirth, castling));
  }
  return true;
}
bool AntiCirceMoveFactory::generateCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target) const {
  const std::unique_ptr<Piece>& piece = board.at(origin);
  int rebirth = piece->findRebirthSquare(board, target, false);
  if (!board.at(rebirth) || rebirth == origin || calvet_ && rebirth == target) {
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
  int rebirth = piece->findRebirthSquare(board, target, false);
  if ((!board.at(rebirth) || rebirth == origin || rebirth == stop) &&
      (calvet_ || !(rebirth == target))) {
    if (board.at(stop)->isRoyal()) {
      return false;
    }
    bool castling = piece->isCastling();
    moves.push_back(std::make_unique<AntiCirceEnPassant>(origin, target, stop,
                                                         rebirth, castling));
  }
  return true;
}
bool AntiCirceMoveFactory::generateEnPassant(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int stop) const {
  const std::unique_ptr<Piece>& piece = board.at(origin);
  int rebirth = piece->findRebirthSquare(board, target, false);
  if ((!board.at(rebirth) || rebirth == origin || rebirth == stop) &&
      (calvet_ || !(rebirth == target))) {
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
  int rebirth = piece->findRebirthSquare(board, target, false);
  if (!board.at(rebirth) || rebirth == origin || calvet_ && rebirth == target) {
    if (board.at(target)->isRoyal()) {
      return false;
    }
    bool castling = piece->isCastling();
    moves.push_back(std::make_unique<AntiCircePromotionCapture>(
        origin, target, black, order, rebirth, castling));
  }
  return true;
}
bool AntiCirceMoveFactory::generatePromotionCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    int origin, int target, bool black, int order) const {
  const std::unique_ptr<Piece>& piece = box.at(black).at(order).front();
  int rebirth = piece->findRebirthSquare(board, target, false);
  if (!board.at(rebirth) || rebirth == origin || calvet_ && rebirth == target) {
    if (board.at(target)->isRoyal()) {
      return false;
    }
  }
  return true;
}

void AndernachMoveFactory::write(std::ostream& output) const {
  output << "AndernachMoveFactory[]";
}
bool AndernachMoveFactory::generateCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, std::vector<std::unique_ptr<Move>>& moves) const {
  if (board.at(target)->isRoyal()) {
    return false;
  }
  const std::unique_ptr<Piece>& piece = board.at(origin);
  if (!piece->isRoyal()) {
    bool castling = piece->isCastling() &&
                    piece->findRebirthSquare(board, target, true) == target;
    moves.push_back(
        std::make_unique<AndernachCapture>(origin, target, castling));
  } else {
    moves.push_back(std::make_unique<Capture>(origin, target));
  }
  return true;
}
bool AndernachMoveFactory::generateEnPassant(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int stop, std::vector<std::unique_ptr<Move>>& moves) const {
  if (board.at(stop)->isRoyal()) {
    return false;
  }
  const std::unique_ptr<Piece>& piece = board.at(origin);
  if (!piece->isRoyal()) {
    bool castling = piece->isCastling() &&
                    piece->findRebirthSquare(board, target, true) == target;
    moves.push_back(
        std::make_unique<AndernachEnPassant>(origin, target, stop, castling));
  } else {
    moves.push_back(std::make_unique<EnPassant>(origin, target, stop));
  }
  return true;
}
bool AndernachMoveFactory::generatePromotionCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    int origin, int target, bool black, int order,
    std::vector<std::unique_ptr<Move>>& moves) const {
  if (board.at(target)->isRoyal()) {
    return false;
  }
  const std::unique_ptr<Piece>& piece = box.at(black).at(order).front();
  if (!piece->isRoyal()) {
    bool castling = piece->isCastling() &&
                    piece->findRebirthSquare(board, target, true) == target;
    moves.push_back(std::make_unique<AndernachPromotionCapture>(
        origin, target, black, order, castling));
  } else {
    moves.push_back(
        std::make_unique<PromotionCapture>(origin, target, black, order));
  }
  return true;
}

void AntiAndernachMoveFactory::write(std::ostream& output) const {
  output << "AntiAndernachMoveFactory[]";
}
void AntiAndernachMoveFactory::generateQuietMove(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, std::vector<std::unique_ptr<Move>>& moves) const {
  const std::unique_ptr<Piece>& piece = board.at(origin);
  if (!piece->isRoyal()) {
    bool castling = piece->isCastling() &&
                    piece->findRebirthSquare(board, target, true) == target;
    moves.push_back(
        std::make_unique<AntiAndernachQuietMove>(origin, target, castling));
  } else {
    moves.push_back(std::make_unique<QuietMove>(origin, target));
  }
}
void AntiAndernachMoveFactory::generateDoubleStep(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int stop, std::vector<std::unique_ptr<Move>>& moves) const {
  const std::unique_ptr<Piece>& piece = board.at(origin);
  if (!piece->isRoyal()) {
    bool castling = piece->isCastling() &&
                    piece->findRebirthSquare(board, target, true) == target;
    moves.push_back(
        std::make_unique<AntiAndernachQuietMove>(origin, target, castling));
  } else {
    moves.push_back(std::make_unique<DoubleStep>(origin, target, stop));
  }
}
void AntiAndernachMoveFactory::generatePromotion(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    int origin, int target, bool black, int order,
    std::vector<std::unique_ptr<Move>>& moves) const {
  const std::unique_ptr<Piece>& piece = box.at(black).at(order).front();
  if (!piece->isRoyal()) {
    bool castling = piece->isCastling() &&
                    piece->findRebirthSquare(board, target, true) == target;
    moves.push_back(std::make_unique<AntiAndernachPromotion>(
        origin, target, black, order, castling));
  } else {
    moves.push_back(std::make_unique<Promotion>(origin, target, black, order));
  }
}

void CirceAndernachMoveFactory::write(std::ostream& output) const {
  output << "CirceAndernachMoveFactory[]";
}
bool CirceAndernachMoveFactory::generateCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, std::vector<std::unique_ptr<Move>>& moves) const {
  const std::unique_ptr<Piece>& piece = board.at(target);
  if (piece->isRoyal()) {
    return false;
  }
  int rebirth = piece->findRebirthSquare(board, target, false);
  const std::unique_ptr<Piece>& piece2 = board.at(origin);
  if (!board.at(rebirth) || rebirth == origin) {
    bool castling = piece->isCastling();
    if (!piece2->isRoyal()) {
      bool castling2 = piece2->isCastling() &&
                       piece2->findRebirthSquare(board, target, true) == target;
      moves.push_back(std::make_unique<CirceAndernachCapture>(
          origin, target, rebirth, castling, castling2));
    } else {
      moves.push_back(
          std::make_unique<CirceCapture>(origin, target, rebirth, castling));
    }
  } else {
    if (!piece2->isRoyal()) {
      bool castling = piece2->isCastling() &&
                      piece2->findRebirthSquare(board, target, true) == target;
      moves.push_back(
          std::make_unique<AndernachCapture>(origin, target, castling));
    } else {
      moves.push_back(std::make_unique<Capture>(origin, target));
    }
  }
  return true;
}
bool CirceAndernachMoveFactory::generateEnPassant(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int stop, std::vector<std::unique_ptr<Move>>& moves) const {
  const std::unique_ptr<Piece>& piece = board.at(stop);
  if (piece->isRoyal()) {
    return false;
  }
  int rebirth = piece->findRebirthSquare(board, stop, false);
  const std::unique_ptr<Piece>& piece2 = board.at(origin);
  if ((!board.at(rebirth) || rebirth == origin || rebirth == stop) &&
      !(rebirth == target)) {
    bool castling = piece->isCastling();
    if (!piece2->isRoyal()) {
      bool castling2 = piece2->isCastling() &&
                       piece2->findRebirthSquare(board, target, true) == target;
      moves.push_back(std::make_unique<CirceAndernachEnPassant>(
          origin, target, stop, rebirth, castling, castling2));
    } else {
      moves.push_back(std::make_unique<CirceEnPassant>(origin, target, stop,
                                                       rebirth, castling));
    }
  } else {
    if (!piece2->isRoyal()) {
      bool castling = piece2->isCastling() &&
                      piece2->findRebirthSquare(board, target, true) == target;
      moves.push_back(
          std::make_unique<AndernachEnPassant>(origin, target, stop, castling));
    } else {
      moves.push_back(std::make_unique<EnPassant>(origin, target, stop));
    }
  }
  return true;
}
bool CirceAndernachMoveFactory::generatePromotionCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    int origin, int target, bool black, int order,
    std::vector<std::unique_ptr<Move>>& moves) const {
  const std::unique_ptr<Piece>& piece = board.at(target);
  if (piece->isRoyal()) {
    return false;
  }
  int rebirth = piece->findRebirthSquare(board, target, false);
  const std::unique_ptr<Piece>& piece2 = box.at(black).at(order).front();
  if (!board.at(rebirth) || rebirth == origin) {
    bool castling = piece->isCastling();
    if (!piece2->isRoyal()) {
      bool castling2 = piece2->isCastling() &&
                       piece2->findRebirthSquare(board, target, true) == target;
      moves.push_back(std::make_unique<CirceAndernachPromotionCapture>(
          origin, target, black, order, rebirth, castling, castling2));
    } else {
      moves.push_back(std::make_unique<CircePromotionCapture>(
          origin, target, black, order, rebirth, castling));
    }
  } else {
    if (!piece2->isRoyal()) {
      bool castling = piece2->isCastling() &&
                      piece2->findRebirthSquare(board, target, true) == target;
      moves.push_back(std::make_unique<AndernachPromotionCapture>(
          origin, target, black, order, castling));
    } else {
      moves.push_back(
          std::make_unique<PromotionCapture>(origin, target, black, order));
    }
  }
  return true;
}

AntiCirceAndernachMoveFactory::AntiCirceAndernachMoveFactory(bool calvet)
    : AntiCirceMoveFactory(calvet), AndernachMoveFactory() {}
void AntiCirceAndernachMoveFactory::write(std::ostream& output) const {
  output << "AntiCirceAndernachMoveFactory[calvet=" << calvet_ << "]";
}
bool AntiCirceAndernachMoveFactory::generateCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, std::vector<std::unique_ptr<Move>>& moves) const {
  const std::unique_ptr<Piece>& piece = board.at(origin);
  int rebirth = piece->findRebirthSquare(board, target, false);
  if (!board.at(rebirth) || rebirth == origin || calvet_ && rebirth == target) {
    if (board.at(target)->isRoyal()) {
      return false;
    }
    if (!piece->isRoyal()) {
      bool castling = piece->isCastling() &&
                      piece->findRebirthSquare(board, rebirth, true) == rebirth;
      moves.push_back(std::make_unique<AntiCirceAndernachCapture>(
          origin, target, rebirth, castling));
    } else {
      bool castling = piece->isCastling();
      moves.push_back(std::make_unique<AntiCirceCapture>(origin, target,
                                                         rebirth, castling));
    }
  }
  return true;
}
bool AntiCirceAndernachMoveFactory::generateCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target) const {
  return AntiCirceMoveFactory::generateCapture(board, origin, target);
}
bool AntiCirceAndernachMoveFactory::generateEnPassant(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int stop, std::vector<std::unique_ptr<Move>>& moves) const {
  const std::unique_ptr<Piece>& piece = board.at(origin);
  int rebirth = piece->findRebirthSquare(board, target, false);
  if ((!board.at(rebirth) || rebirth == origin || rebirth == stop) &&
      (calvet_ || !(rebirth == target))) {
    if (board.at(stop)->isRoyal()) {
      return false;
    }
    if (!piece->isRoyal()) {
      bool castling = piece->isCastling() &&
                      piece->findRebirthSquare(board, rebirth, true) == rebirth;
      moves.push_back(std::make_unique<AntiCirceAndernachEnPassant>(
          origin, target, stop, rebirth, castling));
    } else {
      bool castling = piece->isCastling();
      moves.push_back(std::make_unique<AntiCirceEnPassant>(origin, target, stop,
                                                           rebirth, castling));
    }
  }
  return true;
}
bool AntiCirceAndernachMoveFactory::generateEnPassant(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int stop) const {
  return AntiCirceMoveFactory::generateEnPassant(board, origin, target, stop);
}
bool AntiCirceAndernachMoveFactory::generatePromotionCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    int origin, int target, bool black, int order,
    std::vector<std::unique_ptr<Move>>& moves) const {
  const std::unique_ptr<Piece>& piece = box.at(black).at(order).front();
  int rebirth = piece->findRebirthSquare(board, target, false);
  if (!board.at(rebirth) || rebirth == origin || calvet_ && rebirth == target) {
    if (board.at(target)->isRoyal()) {
      return false;
    }
    if (!piece->isRoyal()) {
      bool castling = piece->isCastling() &&
                      piece->findRebirthSquare(board, rebirth, true) == rebirth;
      moves.push_back(std::make_unique<AntiCirceAndernachPromotionCapture>(
          origin, target, black, order, rebirth, castling));
    } else {
      bool castling = piece->isCastling();
      moves.push_back(std::make_unique<AntiCircePromotionCapture>(
          origin, target, black, order, rebirth, castling));
    }
  }
  return true;
}
bool AntiCirceAndernachMoveFactory::generatePromotionCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    int origin, int target, bool black, int order) const {
  return AntiCirceMoveFactory::generatePromotionCapture(board, box, origin,
                                                        target, black, order);
}

void NoCaptureAntiAndernachMoveFactory::write(std::ostream& output) const {
  output << "NoCaptureAntiAndernachMoveFactory[]";
}
void NoCaptureAntiAndernachMoveFactory::generateQuietMove(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, std::vector<std::unique_ptr<Move>>& moves) const {
  AntiAndernachMoveFactory::generateQuietMove(board, origin, target, moves);
}
bool NoCaptureAntiAndernachMoveFactory::generateCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, std::vector<std::unique_ptr<Move>>& moves) const {
  return NoCaptureMoveFactory::generateCapture(board, origin, target, moves);
}
void NoCaptureAntiAndernachMoveFactory::generateDoubleStep(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int stop, std::vector<std::unique_ptr<Move>>& moves) const {
  AntiAndernachMoveFactory::generateDoubleStep(board, origin, target, stop,
                                               moves);
}
bool NoCaptureAntiAndernachMoveFactory::generateEnPassant(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int stop, std::vector<std::unique_ptr<Move>>& moves) const {
  return NoCaptureMoveFactory::generateEnPassant(board, origin, target, stop,
                                                 moves);
}
void NoCaptureAntiAndernachMoveFactory::generatePromotion(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    int origin, int target, bool black, int order,
    std::vector<std::unique_ptr<Move>>& moves) const {
  AntiAndernachMoveFactory::generatePromotion(board, box, origin, target, black,
                                              order, moves);
}
bool NoCaptureAntiAndernachMoveFactory::generatePromotionCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    int origin, int target, bool black, int order,
    std::vector<std::unique_ptr<Move>>& moves) const {
  return NoCaptureMoveFactory::generatePromotionCapture(
      board, box, origin, target, black, order, moves);
}

void CirceAntiAndernachMoveFactory::write(std::ostream& output) const {
  output << "CirceAntiAndernachMoveFactory[]";
}
void CirceAntiAndernachMoveFactory::generateQuietMove(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, std::vector<std::unique_ptr<Move>>& moves) const {
  AntiAndernachMoveFactory::generateQuietMove(board, origin, target, moves);
}
bool CirceAntiAndernachMoveFactory::generateCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, std::vector<std::unique_ptr<Move>>& moves) const {
  return CirceMoveFactory::generateCapture(board, origin, target, moves);
}
void CirceAntiAndernachMoveFactory::generateDoubleStep(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int stop, std::vector<std::unique_ptr<Move>>& moves) const {
  AntiAndernachMoveFactory::generateDoubleStep(board, origin, target, stop,
                                               moves);
}
bool CirceAntiAndernachMoveFactory::generateEnPassant(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int stop, std::vector<std::unique_ptr<Move>>& moves) const {
  return CirceMoveFactory::generateEnPassant(board, origin, target, stop,
                                             moves);
}
void CirceAntiAndernachMoveFactory::generatePromotion(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    int origin, int target, bool black, int order,
    std::vector<std::unique_ptr<Move>>& moves) const {
  AntiAndernachMoveFactory::generatePromotion(board, box, origin, target, black,
                                              order, moves);
}
bool CirceAntiAndernachMoveFactory::generatePromotionCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    int origin, int target, bool black, int order,
    std::vector<std::unique_ptr<Move>>& moves) const {
  return CirceMoveFactory::generatePromotionCapture(board, box, origin, target,
                                                    black, order, moves);
}

AntiCirceAntiAndernachMoveFactory::AntiCirceAntiAndernachMoveFactory(
    bool calvet)
    : AntiCirceMoveFactory(calvet), AntiAndernachMoveFactory() {}
void AntiCirceAntiAndernachMoveFactory::write(std::ostream& output) const {
  output << "AntiCirceAntiAndernachMoveFactory[calvet=" << calvet_ << "]";
}
void AntiCirceAntiAndernachMoveFactory::generateQuietMove(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, std::vector<std::unique_ptr<Move>>& moves) const {
  AntiAndernachMoveFactory::generateQuietMove(board, origin, target, moves);
}
bool AntiCirceAntiAndernachMoveFactory::generateCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, std::vector<std::unique_ptr<Move>>& moves) const {
  return AntiCirceMoveFactory::generateCapture(board, origin, target, moves);
}
bool AntiCirceAntiAndernachMoveFactory::generateCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target) const {
  return AntiCirceMoveFactory::generateCapture(board, origin, target);
}
void AntiCirceAntiAndernachMoveFactory::generateDoubleStep(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int stop, std::vector<std::unique_ptr<Move>>& moves) const {
  AntiAndernachMoveFactory::generateDoubleStep(board, origin, target, stop,
                                               moves);
}
bool AntiCirceAntiAndernachMoveFactory::generateEnPassant(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int stop, std::vector<std::unique_ptr<Move>>& moves) const {
  return AntiCirceMoveFactory::generateEnPassant(board, origin, target, stop,
                                                 moves);
}
bool AntiCirceAntiAndernachMoveFactory::generateEnPassant(
    const std::array<std::unique_ptr<Piece>, 128>& board, int origin,
    int target, int stop) const {
  return AntiCirceMoveFactory::generateEnPassant(board, origin, target, stop);
}
void AntiCirceAntiAndernachMoveFactory::generatePromotion(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    int origin, int target, bool black, int order,
    std::vector<std::unique_ptr<Move>>& moves) const {
  AntiAndernachMoveFactory::generatePromotion(board, box, origin, target, black,
                                              order, moves);
}
bool AntiCirceAntiAndernachMoveFactory::generatePromotionCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    int origin, int target, bool black, int order,
    std::vector<std::unique_ptr<Move>>& moves) const {
  return AntiCirceMoveFactory::generatePromotionCapture(
      board, box, origin, target, black, order, moves);
}
bool AntiCirceAntiAndernachMoveFactory::generatePromotionCapture(
    const std::array<std::unique_ptr<Piece>, 128>& board,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box,
    int origin, int target, bool black, int order) const {
  return AntiCirceMoveFactory::generatePromotionCapture(board, box, origin,
                                                        target, black, order);
}

}  // namespace moderato
