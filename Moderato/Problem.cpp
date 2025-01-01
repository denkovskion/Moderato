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

#include "Problem.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

namespace moderato {

Problem::Problem(Position position, int nMoves)
    : position_(std::move(position)), nMoves_(nMoves) {}
Problem::~Problem() {}
bool Problem::evaluateTerminalNode(Position& position, bool stalemate) {
  return position.isCheck() == 0 == stalemate;
}

void solve(const Task& task) {
  std::cout << std::string(72, '-') << std::endl;
  if (task.displayOptions.internalModel) {
    logger(std::clog) << "task=" << task << std::endl;
  }
  logger(std::clog) << "problem.solve(...)" << std::endl;
  std::chrono::steady_clock::time_point begin =
      std::chrono::steady_clock::now();
  task.problem->solve(task.analysisOptions, task.displayOptions);
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  logger(std::clog) << "duration="
                    << std::chrono::duration_cast<std::chrono::milliseconds>(
                           end - begin)
                           .count()
                    << "ms" << std::endl;
}

std::ostream& logger(std::ostream& output) {
  std::time_t calendarTime =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
#if _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996)
#endif
  std::tm* localTime = std::localtime(&calendarTime);
#if _MSC_VER
#pragma warning(pop)
#endif
  if (localTime) {
    output << std::put_time(localTime, "Moderato@%c ");
  }
  return output;
}

std::ostream& operator<<(std::ostream& output, const Task& task) {
  output << "Task[problem=*" << *task.problem
         << ", analysisOptions=" << task.analysisOptions
         << ", displayOptions=" << task.displayOptions << "]";
  return output;
}

std::ostream& operator<<(std::ostream& output, const Problem& problem) {
  problem.write(output);
  return output;
}

std::ostream& operator<<(std::ostream& output,
                         const AnalysisOptions& analysisOptions) {
  output << "AnalysisOptions[setPlay=" << analysisOptions.setPlay
         << ", nRefutations=" << analysisOptions.nRefutations
         << ", variations=" << analysisOptions.variations
         << ", threats=" << analysisOptions.threats
         << ", shortVariations=" << analysisOptions.shortVariations
         << ", tempoTries=" << analysisOptions.tempoTries << "]";
  return output;
}

std::ostream& operator<<(std::ostream& output,
                         const DisplayOptions& displayOptions) {
  output << "DisplayOptions[outputLanguage=" << displayOptions.outputLanguage
         << ", internalModel=" << displayOptions.internalModel
         << ", internalProgress=" << displayOptions.internalProgress << "]";
  return output;
}

}  // namespace moderato
