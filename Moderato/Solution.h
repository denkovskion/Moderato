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

#pragma once

#include <deque>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace moderato {

class Move;
class Position;

enum class Play {
  SET,
  TRY,
  KEY,
  CONTINUATION,
  TEMPO_1ST,
  HELP_1ST,
  ZUGZWANG,
  THREAT,
  VARIATION,
  REFUTATION,
  TEMPO_2ND,
  HELP_2ND
};

std::vector<std::deque<std::pair<Play, std::string>>> toFlattened(
    const std::vector<
        std::pair<std::pair<Play, std::string>,
                  std::vector<std::deque<std::pair<Play, std::string>>>>>&
        branches);

std::vector<std::deque<std::pair<Play, std::string>>> toTransformed(
    const std::vector<std::deque<std::pair<Play, std::shared_ptr<Move>>>>&
        lines,
    Position& position, int translate);

std::string toFormatted(
    const std::vector<std::deque<std::pair<Play, std::string>>>& lines);

std::string toOrderedAndFormatted(
    const std::vector<std::pair<std::string, std::string>>& points);

}  // namespace moderato
