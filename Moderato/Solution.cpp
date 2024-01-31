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

#include "Solution.h"

#include <algorithm>
#include <iterator>
#include <sstream>

#include "Move.h"

namespace moderato {

std::vector<std::pair<std::pair<Play, std::string>,
                      std::vector<std::deque<std::pair<Play, std::string>>>>>
toMapped(const std::vector<std::deque<std::pair<Play, std::string>>>& lines);

std::vector<std::pair<std::pair<Play, std::string>,
                      std::vector<std::deque<std::pair<Play, std::string>>>>>
toOrdered(const std::vector<
          std::pair<std::pair<Play, std::string>,
                    std::vector<std::deque<std::pair<Play, std::string>>>>>&
              branches);

std::vector<std::pair<std::pair<Play, std::string>,
                      std::vector<std::deque<std::pair<Play, std::string>>>>>
toGrouped(const std::vector<
          std::pair<std::pair<Play, std::string>,
                    std::vector<std::deque<std::pair<Play, std::string>>>>>&
              branches);

void write(const std::vector<std::deque<std::pair<Play, std::string>>>& lines,
           int moveNo, bool newline, bool tab, bool space,
           std::ostream& output);

void write(
    const std::pair<std::pair<Play, std::string>,
                    std::vector<std::deque<std::pair<Play, std::string>>>>&
        branch,
    int moveNo, bool newline, bool tab, bool space, std::ostream& output);

bool operator==(const std::pair<Play, std::shared_ptr<Move>>& point1,
                const std::pair<Play, std::shared_ptr<Move>>& point2) {
  return point1.first == point2.first && *point1.second == *point2.second;
}

std::vector<std::deque<std::pair<Play, std::shared_ptr<Move>>>> toFlattened(
    const std::vector<std::pair<
        std::pair<Play, std::shared_ptr<Move>>,
        std::vector<std::deque<std::pair<Play, std::shared_ptr<Move>>>>>>&
        branches) {
  std::vector<std::deque<std::pair<Play, std::shared_ptr<Move>>>> lines;
  for (auto iBranch = branches.cbegin(); iBranch != branches.cend();
       iBranch++) {
    if (iBranch->second.empty()) {
      lines.push_back({iBranch->first});
    } else {
      for (auto iLine = iBranch->second.cbegin();
           iLine != iBranch->second.cend(); iLine++) {
        std::deque<std::pair<Play, std::shared_ptr<Move>>> line = *iLine;
        line.push_front(iBranch->first);
        lines.push_back(line);
      }
    }
  }
  return lines;
}

std::vector<std::deque<std::pair<Play, std::string>>> toTransformed(
    const std::vector<std::deque<std::pair<Play, std::shared_ptr<Move>>>>&
        lines,
    Position& position, int translate) {
  std::vector<std::deque<std::pair<Play, std::string>>> results;
  for (auto iLine = lines.cbegin(); iLine != lines.cend(); iLine++) {
    std::deque<std::pair<Play, std::string>> result;
    for (auto iPoint = iLine->cbegin(); iPoint != iLine->cend(); iPoint++) {
      std::ostringstream lanBuilder;
      iPoint->second->make(position, lanBuilder, translate);
      result.push_back({iPoint->first, lanBuilder.str()});
    }
    for (auto iPoint = iLine->crbegin(); iPoint != iLine->crend(); iPoint++) {
      iPoint->second->unmake(position);
    }
    results.push_back(result);
  }
  return results;
}

std::string toFormatted(
    const std::vector<std::deque<std::pair<Play, std::string>>>& lines) {
  std::ostringstream stringBuilder;
  write(lines, 1, false, true, false, stringBuilder);
  return stringBuilder.str();
}

std::vector<std::pair<std::pair<Play, std::string>,
                      std::vector<std::deque<std::pair<Play, std::string>>>>>
toMapped(const std::vector<std::deque<std::pair<Play, std::string>>>& lines) {
  std::vector<std::pair<std::pair<Play, std::string>,
                        std::vector<std::deque<std::pair<Play, std::string>>>>>
      branches;
  for (auto iLine = lines.cbegin(); iLine != lines.cend(); iLine++) {
    auto iBranch = std::find_if(
        branches.begin(), branches.end(),
        [iLine](const auto& branch) { return branch.first == iLine->front(); });
    if (iBranch == branches.end()) {
      if (iLine->size() == 1) {
        branches.push_back({iLine->front(), {}});
      } else {
        branches.push_back(
            {iLine->front(), {{std::next(iLine->cbegin()), iLine->end()}}});
      }
    } else {
      iBranch->second.push_back({std::next(iLine->cbegin()), iLine->end()});
    }
  }
  return branches;
}

std::vector<std::pair<std::pair<Play, std::string>,
                      std::vector<std::deque<std::pair<Play, std::string>>>>>
toOrdered(const std::vector<
          std::pair<std::pair<Play, std::string>,
                    std::vector<std::deque<std::pair<Play, std::string>>>>>&
              branches) {
  std::vector<std::pair<std::pair<Play, std::string>,
                        std::vector<std::deque<std::pair<Play, std::string>>>>>
      results = branches;
  std::stable_sort(results.begin(), results.end(),
                   [](const auto& branch1, const auto& branch2) {
                     return (branch1.first.first < branch2.first.first);
                   });
  return results;
}

std::vector<std::pair<std::pair<Play, std::string>,
                      std::vector<std::deque<std::pair<Play, std::string>>>>>
toGrouped(const std::vector<
          std::pair<std::pair<Play, std::string>,
                    std::vector<std::deque<std::pair<Play, std::string>>>>>&
              branches) {
  std::vector<std::pair<std::pair<Play, std::string>,
                        std::vector<std::deque<std::pair<Play, std::string>>>>>
      results;
  for (auto iBranch = branches.cbegin(); iBranch != branches.cend();
       iBranch++) {
    auto iResult = std::find_if(
        results.begin(), results.end(), [iBranch](const auto& result) {
          return result.first.first == iBranch->first.first &&
                 result.second == iBranch->second;
        });
    if (iResult == results.end()) {
      results.push_back(*iBranch);
    } else {
      iResult->first.second += "," + iBranch->first.second;
    }
  }
  return results;
}

void write(const std::vector<std::deque<std::pair<Play, std::string>>>& lines,
           int moveNo, bool newline, bool tab, bool space,
           std::ostream& output) {
  const std::vector<
      std::pair<std::pair<Play, std::string>,
                std::vector<std::deque<std::pair<Play, std::string>>>>>
      branches = toGrouped(toOrdered(toMapped(lines)));
  auto iBranch = branches.cbegin();
  if (iBranch != branches.cend()) {
    write(*iBranch, moveNo, newline, tab, space, output);
    while (++iBranch != branches.cend()) {
      write(*iBranch, moveNo, true, true, false, output);
    }
  }
}

void write(
    const std::pair<std::pair<Play, std::string>,
                    std::vector<std::deque<std::pair<Play, std::string>>>>&
        branch,
    int moveNo, bool newline, bool tab, bool space, std::ostream& output) {
  const auto& play = branch.first.first;
  const auto& move = branch.first.second;
  const auto& lines = branch.second;
  if (play == Play::SET) {
    write(lines, moveNo, newline, tab, space, output);
  } else {
    if (newline) {
      output << std::endl;
    }
    if (tab) {
      output << std::string(moveNo - 1, '\t');
    } else if (space) {
      output << " ";
    }
    if (play == Play::ZUGZWANG || play == Play::THREAT) {
      output << "(";
      if (play == Play::ZUGZWANG) {
        output << "zz";
        write(lines, moveNo + 1, true, true, false, output);
      } else {
        write(lines, moveNo + 1, false, false, false, output);
      }
      output << ")";
    } else if (play == Play::VARIATION || play == Play::REFUTATION ||
               play == Play::HELP_2ND || play == Play::TEMPO_2ND) {
      if (tab) {
        output << moveNo;
        output << "...";
      }
      if (play == Play::TEMPO_2ND) {
        output << "??";
      } else {
        output << move;
        if (play == Play::REFUTATION) {
          output << "!";
        }
      }
      write(lines, moveNo + 1, false, false, true, output);
    } else {
      output << moveNo;
      output << ".";
      if (play == Play::TEMPO_1ST) {
        output << "??";
      } else {
        output << move;
        if (play == Play::TRY) {
          output << "?";
        } else if (play == Play::KEY) {
          output << "!";
        }
      }
      write(lines, moveNo, false, false, true, output);
    }
  }
}

std::vector<std::pair<std::string, std::string>> toTransformed(
    const std::vector<std::pair<std::string, std::shared_ptr<Move>>>& points,
    Position& position, int translate) {
  std::vector<std::pair<std::string, std::string>> results;
  for (auto iPoint = points.cbegin(); iPoint != points.cend(); iPoint++) {
    std::ostringstream lanBuilder;
    iPoint->second->make(position, lanBuilder, translate);
    results.push_back({iPoint->first, lanBuilder.str()});
    iPoint->second->unmake(position);
  }
  return results;
}

std::string toFormatted(
    const std::vector<std::pair<std::string, std::string>>& points) {
  std::vector<std::pair<std::string, std::string>> results = points;
  std::sort(results.begin(), results.end());
  std::ostringstream stringBuilder;
  auto iPoint = results.cbegin();
  if (iPoint != results.cend()) {
    stringBuilder << iPoint->first << '\t' << iPoint->second;
    while (++iPoint != results.cend()) {
      stringBuilder << std::endl << iPoint->first << '\t' << iPoint->second;
    }
  }
  return stringBuilder.str();
}

}  // namespace moderato
