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

#include "Problem.h"
#include "Solution.h"

namespace moderato {

class HelpProblem : virtual public Problem {
 protected:
  const bool halfMove_;
  HelpProblem(bool halfMove);
};

class MateProblem : virtual public Problem {
 protected:
  const bool stalemate_;
  MateProblem(bool stalemate);
};

class BattlePlay {
  void analyseMax(
      Position& position, bool stalemate, int depth,
      const std::vector<std::unique_ptr<Move>>& pseudoLegalMovesMax,
      std::vector<
          std::pair<std::pair<Play, std::string>,
                    std::vector<std::deque<std::pair<Play, std::string>>>>>&
          branches,
      int translate, bool includeVariations, bool includeThreats,
      bool includeShortVariations, bool includeSetPlay, int includeTries,
      bool includeActualPlay, bool markKeys, bool logMoves);
  void analyseMin(
      Position& position, bool stalemate, int depth,
      const std::vector<std::unique_ptr<Move>>& pseudoLegalMovesMin,
      std::vector<
          std::pair<std::pair<Play, std::string>,
                    std::vector<std::deque<std::pair<Play, std::string>>>>>&
          branches,
      int translate, bool includeVariations, bool includeThreats,
      bool includeShortVariations, bool includeSetPlay);
  virtual int searchMax(
      Position& position, bool stalemate, int depth,
      const std::vector<std::unique_ptr<Move>>& pseudoLegalMovesMax) = 0;
  virtual int searchMin(
      Position& position, bool stalemate, int depth,
      const std::vector<std::unique_ptr<Move>>& pseudoLegalMovesMin,
      int nRefutations) = 0;
  virtual int getTerminalDepth() const = 0;

 protected:
  void solve(Position& position, bool stalemate, int nMoves,
             bool includeSetPlay, int includeTries, bool includeVariations,
             bool includeThreats, bool includeShortVariations, int translate,
             bool logMoves);
};

class Directmate : public MateProblem, BattlePlay {
  int searchMax(
      Position& position, bool stalemate, int depth,
      const std::vector<std::unique_ptr<Move>>& pseudoLegalMovesMax) override;
  int searchMin(Position& position, bool stalemate, int depth,
                const std::vector<std::unique_ptr<Move>>& pseudoLegalMovesMin,
                int nRefutations) override;
  int getTerminalDepth() const override;
  void write(std::ostream& output) const override;

 public:
  Directmate(Position position, bool stalemate, int nMoves);
  void solve(const AnalysisOptions& analysisOptions,
             const DisplayOptions& displayOptions) override;
};

class Selfmate : public MateProblem, BattlePlay {
  int searchMax(
      Position& position, bool stalemate, int depth,
      const std::vector<std::unique_ptr<Move>>& pseudoLegalMovesMax) override;
  int searchMin(Position& position, bool stalemate, int depth,
                const std::vector<std::unique_ptr<Move>>& pseudoLegalMovesMin,
                int nRefutations) override;
  int getTerminalDepth() const override;
  void write(std::ostream& output) const override;

 public:
  Selfmate(Position position, bool stalemate, int nMoves);
  void solve(const AnalysisOptions& analysisOptions,
             const DisplayOptions& displayOptions) override;
};

class Helpmate : public HelpProblem, public MateProblem {
  void solve(Position& position, bool stalemate, int nMoves, bool halfMove,
             bool includeSetPlay, bool includeTempoTries, int translate,
             bool logMoves);
  int analyseMax(
      Position& position, bool stalemate, int depth,
      const std::vector<std::unique_ptr<Move>>& pseudoLegalMovesMax,
      std::vector<
          std::pair<std::pair<Play, std::string>,
                    std::vector<std::deque<std::pair<Play, std::string>>>>>&
          branchesMax,
      int translate, bool includeTempoTries, bool includeSetPlay,
      bool includeActualPlay, bool logMoves);
  int analyseMin(
      Position& position, bool stalemate, int depth,
      const std::vector<std::unique_ptr<Move>>& pseudoLegalMovesMin,
      std::vector<
          std::pair<std::pair<Play, std::string>,
                    std::vector<std::deque<std::pair<Play, std::string>>>>>&
          branchesMin,
      int translate, bool includeTempoTries, bool includeSetPlay,
      bool includeActualPlay, bool logMoves);
  void write(std::ostream& output) const override;

 public:
  Helpmate(Position position, bool stalemate, int nMoves, bool halfMove);
  void solve(const AnalysisOptions& analysisOptions,
             const DisplayOptions& displayOptions) override;
};

class MateSearch : public Problem {
  void solve(Position& position, int nMoves, int translate);
  int searchMax(Position& position, int depth,
                const std::vector<std::unique_ptr<Move>>& pseudoLegalMovesMax);
  int searchMin(Position& position, int depth,
                const std::vector<std::unique_ptr<Move>>& pseudoLegalMovesMin);
  void write(std::ostream& output) const override;

 public:
  MateSearch(Position position, int nMoves);
  void solve(const AnalysisOptions& analysisOptions,
             const DisplayOptions& displayOptions) override;
};

class Perft : public HelpProblem {
  void solve(Position& position, int nMoves, bool halfMove);
  long analyse(Position& position, int depth,
               const std::vector<std::unique_ptr<Move>>& pseudoLegalMoves);
  void write(std::ostream& output) const override;

 public:
  Perft(Position position, int nMoves, bool halfMove);
  void solve(const AnalysisOptions& analysisOptions,
             const DisplayOptions& displayOptions) override;
};

}  // namespace moderato
