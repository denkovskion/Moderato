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

#pragma once

#include <istream>

#include "Position.h"

namespace moderato {

struct AnalysisOptions {
  bool zero;
  bool setPlay;
  int nRefutations;
  bool variations;
  bool threats;
  bool shortVariations;
  bool tempoTries;
};
std::ostream& operator<<(std::ostream& output,
                         const AnalysisOptions& analysisOptions);

struct DisplayOptions {
  int outputLanguage;
  bool internalModel;
  bool internalProgress;
};
std::ostream& operator<<(std::ostream& output,
                         const DisplayOptions& displayOptions);

class Problem {
  virtual void write(std::ostream& output) const = 0;

 protected:
  Position position_;
  const int nMoves_;
  Problem(Position position, int nMoves);

 public:
  virtual ~Problem();
  virtual void solve(const AnalysisOptions& analysisOptions,
                     const DisplayOptions& displayOptions) = 0;
  friend std::ostream& operator<<(std::ostream& output, const Problem& problem);
};

struct Task {
  std::unique_ptr<Problem> problem;
  AnalysisOptions analysisOptions;
  DisplayOptions displayOptions;
};
void solve(const Task& task);
std::ostream& operator<<(std::ostream& output, const Task& task);
std::istream& operator>>(std::istream& input, std::vector<Task>& tasks);

std::ostream& logger(std::ostream& output);

}  // namespace moderato
