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

#include "PieceCategories.h"

#include "MoveTypes.h"

namespace moderato {

bool Leaper::generateMoves(const std::array<std::unique_ptr<Piece>, 128>& board,
                           int origin,
                           std::vector<std::shared_ptr<Move>>& moves) const {
  const std::vector<int>& directions = getLeaps(board);
  for (int direction : directions) {
    int target = origin + direction;
    if (!(target & 136)) {
      const std::unique_ptr<Piece>& piece = board.at(target);
      if (piece) {
        if (piece->isBlack() != isBlack()) {
          if (piece->isRoyal()) {
            return false;
          }
          moves.push_back(std::make_shared<Capture>(origin, target));
        }
      } else {
        moves.push_back(std::make_shared<QuietMove>(origin, target));
      }
    }
  }
  return true;
}
bool Leaper::generateMoves(const std::array<std::unique_ptr<Piece>, 128>& board,
                           int origin) const {
  const std::vector<int>& directions = getLeaps(board);
  for (int direction : directions) {
    int target = origin + direction;
    if (!(target & 136)) {
      const std::unique_ptr<Piece>& piece = board.at(target);
      if (piece) {
        if (piece->isBlack() != isBlack()) {
          if (piece->isRoyal()) {
            return false;
          }
        }
      }
    }
  }
  return true;
}

bool Rider::generateMoves(const std::array<std::unique_ptr<Piece>, 128>& board,
                          int origin,
                          std::vector<std::shared_ptr<Move>>& moves) const {
  const std::vector<int>& directions = getRides(board);
  for (int direction : directions) {
    int distance = 1;
    while (true) {
      int target = origin + distance * direction;
      if (!(target & 136)) {
        const std::unique_ptr<Piece>& piece = board.at(target);
        if (piece) {
          if (piece->isBlack() != isBlack()) {
            if (piece->isRoyal()) {
              return false;
            }
            moves.push_back(std::make_shared<Capture>(origin, target));
          }
          break;
        } else {
          moves.push_back(std::make_shared<QuietMove>(origin, target));
          distance++;
        }
      } else {
        break;
      }
    }
  }
  return true;
}
bool Rider::generateMoves(const std::array<std::unique_ptr<Piece>, 128>& board,
                          int origin) const {
  const std::vector<int>& directions = getRides(board);
  for (int direction : directions) {
    int distance = 1;
    while (true) {
      int target = origin + distance * direction;
      if (!(target & 136)) {
        const std::unique_ptr<Piece>& piece = board.at(target);
        if (piece) {
          if (piece->isBlack() != isBlack()) {
            if (piece->isRoyal()) {
              return false;
            }
          }
          break;
        } else {
          distance++;
        }
      } else {
        break;
      }
    }
  }
  return true;
}

}  // namespace moderato
