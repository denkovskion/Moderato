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

#include "PieceCategories.h"

#include "MoveFactory.h"

namespace moderato {

bool Leaper::generateMoves(const std::array<std::unique_ptr<Piece>, 128>& board,
                           int origin, const MoveFactory& moveFactory,
                           std::vector<std::shared_ptr<Move>>& moves) const {
  const std::vector<int>& directions = getLeaps(board);
  for (int direction : directions) {
    int target = origin + direction;
    if (!(target & 136)) {
      const std::unique_ptr<Piece>& piece = board.at(target);
      if (piece) {
        if (piece->isBlack() != isBlack()) {
          if (!moveFactory.generateCapture(board, origin, target, moves)) {
            return false;
          }
        }
      } else {
        moveFactory.generateQuietMove(board, origin, target, moves);
      }
    }
  }
  return true;
}
bool Leaper::generateMoves(const std::array<std::unique_ptr<Piece>, 128>& board,
                           int origin, const MoveFactory& moveFactory) const {
  const std::vector<int>& directions = getLeaps(board);
  for (int direction : directions) {
    int target = origin + direction;
    if (!(target & 136)) {
      const std::unique_ptr<Piece>& piece = board.at(target);
      if (piece) {
        if (piece->isBlack() != isBlack()) {
          if (!moveFactory.generateCapture(board, origin, target)) {
            return false;
          }
        }
      }
    }
  }
  return true;
}

bool Rider::generateMoves(const std::array<std::unique_ptr<Piece>, 128>& board,
                          int origin, const MoveFactory& moveFactory,
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
            if (!moveFactory.generateCapture(board, origin, target, moves)) {
              return false;
            }
          }
          break;
        } else {
          moveFactory.generateQuietMove(board, origin, target, moves);
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
                          int origin, const MoveFactory& moveFactory) const {
  const std::vector<int>& directions = getRides(board);
  for (int direction : directions) {
    int distance = 1;
    while (true) {
      int target = origin + distance * direction;
      if (!(target & 136)) {
        const std::unique_ptr<Piece>& piece = board.at(target);
        if (piece) {
          if (piece->isBlack() != isBlack()) {
            if (!moveFactory.generateCapture(board, origin, target)) {
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

bool Hopper::generateMoves(const std::array<std::unique_ptr<Piece>, 128>& board,
                           int origin, const MoveFactory& moveFactory,
                           std::vector<std::shared_ptr<Move>>& moves) const {
  const std::vector<int>& directions = getHops(board);
  for (int direction : directions) {
    int distance = 1;
    while (true) {
      int target = origin + distance * direction;
      if (!(target & 136)) {
        if (board.at(target)) {
          target = origin + (distance + 1) * direction;
          if (!(target & 136)) {
            const std::unique_ptr<Piece>& piece = board.at(target);
            if (piece) {
              if (piece->isBlack() != isBlack()) {
                if (!moveFactory.generateCapture(board, origin, target,
                                                 moves)) {
                  return false;
                }
              }
            } else {
              moveFactory.generateQuietMove(board, origin, target, moves);
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
bool Hopper::generateMoves(const std::array<std::unique_ptr<Piece>, 128>& board,
                           int origin, const MoveFactory& moveFactory) const {
  const std::vector<int>& directions = getHops(board);
  for (int direction : directions) {
    int distance = 1;
    while (true) {
      int target = origin + distance * direction;
      if (!(target & 136)) {
        if (board.at(target)) {
          target = origin + (distance + 1) * direction;
          if (!(target & 136)) {
            const std::unique_ptr<Piece>& piece = board.at(target);
            if (piece) {
              if (piece->isBlack() != isBlack()) {
                if (!moveFactory.generateCapture(board, origin, target)) {
                  return false;
                }
              }
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
