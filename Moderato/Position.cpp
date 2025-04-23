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

#include "Position.h"

#include "Move.h"

namespace moderato {

std::ostream& operator<<(std::ostream& output,
                         const std::array<std::unique_ptr<Piece>, 128>& board);
std::ostream& operator<<(
    std::ostream& output,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box);
std::ostream& operator<<(std::ostream& output,
                         const std::stack<std::unique_ptr<Piece>>& table);
std::ostream& operator<<(
    std::ostream& output,
    const std::pair<std::set<int>, std::shared_ptr<int>>& state);
std::ostream& operator<<(
    std::ostream& output,
    const std::stack<std::pair<std::set<int>, std::shared_ptr<int>>>& memory);

Position::Position(
    std::array<std::unique_ptr<Piece>, 128> board,
    std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>> box,
    std::stack<std::unique_ptr<Piece>> table, bool blackToMove,
    std::pair<std::set<int>, std::shared_ptr<int>> state,
    std::stack<std::pair<std::set<int>, std::shared_ptr<int>>> memory,
    std::unique_ptr<MoveFactory> moveFactory)
    : board_(std::move(board)),
      box_(std::move(box)),
      table_(std::move(table)),
      blackToMove_(blackToMove),
      state_(std::move(state)),
      memory_(std::move(memory)),
      moveFactory_(std::move(moveFactory)) {}

std::array<std::unique_ptr<Piece>, 128>& Position::getBoard() { return board_; }
std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
Position::getBox() {
  return box_;
}
std::stack<std::unique_ptr<Piece>>& Position::getTable() { return table_; }
bool& Position::isBlackToMove() { return blackToMove_; }
std::pair<std::set<int>, std::shared_ptr<int>>& Position::getState() {
  return state_;
}
std::stack<std::pair<std::set<int>, std::shared_ptr<int>>>&
Position::getMemory() {
  return memory_;
}

bool Position::isLegal(std::vector<std::unique_ptr<Move>>& pseudoLegalMoves) {
  for (int square = 0; square < 128; square++) {
    if (!(square & 136)) {
      const std::unique_ptr<Piece>& piece = board_.at(square);
      if (piece && piece->isBlack() == blackToMove_) {
        if (!piece->generateMoves(board_, box_, state_, square, *moveFactory_,
                                  pseudoLegalMoves)) {
          return false;
        }
      }
    }
  }
  return true;
}
bool Position::isLegal() {
  for (int square = 0; square < 128; square++) {
    if (!(square & 136)) {
      const std::unique_ptr<Piece>& piece = board_.at(square);
      if (piece && piece->isBlack() == blackToMove_) {
        if (!piece->generateMoves(board_, box_, state_, square,
                                  *moveFactory_)) {
          return false;
        }
      }
    }
  }
  return true;
}
int Position::isCheck() {
  NullMove nullMove;
  nullMove.make(*this);
  int nChecks = 0;
  for (int square = 0; square < 128; square++) {
    if (!(square & 136)) {
      const std::unique_ptr<Piece>& piece = board_.at(square);
      if (piece && piece->isBlack() == blackToMove_) {
        if (!piece->generateMoves(board_, box_, state_, square,
                                  *moveFactory_)) {
          nChecks++;
        }
      }
    }
  }
  nullMove.unmake(*this);
  return nChecks;
}
bool Position::isTerminal(
    const std::vector<std::unique_ptr<Move>>& pseudoLegalMoves) {
  for (const std::unique_ptr<Move>& move : pseudoLegalMoves) {
    bool result = move->make(*this);
    move->unmake(*this);
    if (result) {
      return false;
    }
  }
  return true;
}

std::ostream& operator<<(std::ostream& output, const Position& position) {
  output << "Position[board=" << position.board_ << ", box=" << position.box_
         << ", table=" << position.table_
         << ", blackToMove=" << position.blackToMove_
         << ", state=" << position.state_ << ", memory=" << position.memory_
         << ", moveFactory=*" << *position.moveFactory_ << "]";
  return output;
}
std::ostream& operator<<(std::ostream& output,
                         const std::array<std::unique_ptr<Piece>, 128>& board) {
  output << "[";
  for (auto piece = board.cbegin(); piece != board.cend(); piece++) {
    if (piece != board.cbegin()) {
      output << ", ";
    }
    output << "*";
    if (*piece) {
      output << **piece;
    }
  }
  output << "]";
  return output;
}
std::ostream& operator<<(
    std::ostream& output,
    const std::map<bool, std::map<int, std::deque<std::unique_ptr<Piece>>>>&
        box) {
  output << "{";
  for (auto entry = box.cbegin(); entry != box.cend(); entry++) {
    if (entry != box.cbegin()) {
      output << ", ";
    }
    output << entry->first << "=[";
    for (auto section = entry->second.cbegin(); section != entry->second.cend();
         section++) {
      if (section != entry->second.cbegin()) {
        output << ", ";
      }
      output << section->first << "=[";
      for (auto piece = section->second.cbegin();
           piece != section->second.cend(); piece++) {
        if (piece != section->second.cbegin()) {
          output << ", ";
        }
        output << "*";
        if (*piece) {
          output << **piece;
        }
      }
      output << "]";
    }
    output << "]";
  }
  output << "}";
  return output;
}
std::ostream& operator<<(std::ostream& output,
                         const std::stack<std::unique_ptr<Piece>>& table) {
  output << "[";
  if (!table.empty()) {
    output << "top=*";
    if (table.top()) {
      output << *table.top();
    }
    output << ", size=" << table.size();
  }
  output << "]";
  return output;
}
std::ostream& operator<<(
    std::ostream& output,
    const std::pair<std::set<int>, std::shared_ptr<int>>& state) {
  output << "([";
  for (auto square = state.first.cbegin(); square != state.first.cend();
       square++) {
    if (square != state.first.cbegin()) {
      output << ", ";
    }
    output << *square;
  }
  output << "], *";
  if (state.second) {
    output << *state.second;
  }
  output << ")";
  return output;
}
std::ostream& operator<<(
    std::ostream& output,
    const std::stack<std::pair<std::set<int>, std::shared_ptr<int>>>& memory) {
  output << "[";
  if (!memory.empty()) {
    output << "top=" << memory.top() << ", size=" << memory.size();
  }
  output << "]";
  return output;
}

}  // namespace moderato
