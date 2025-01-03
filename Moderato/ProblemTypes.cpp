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

#include "ProblemTypes.h"

#include <algorithm>
#include <iostream>
#include <sstream>

#include "Move.h"

namespace moderato {

HelpProblem::HelpProblem(bool halfMove) : halfMove_(halfMove) {}

MateProblem::MateProblem(bool stalemate) : stalemate_(stalemate) {}

void BattlePlay::solve(Position& position, bool stalemate, int nMoves,
                       bool includeSetPlay, int includeTries,
                       bool includeVariations, bool includeThreats,
                       bool includeShortVariations, int translate,
                       bool logMoves) {
  std::vector<std::shared_ptr<Move>> pseudoLegalMoves;
  bool includeActualPlay = position.isLegal(pseudoLegalMoves);
  if (includeActualPlay || includeSetPlay) {
    std::vector<
        std::pair<std::pair<Play, std::string>,
                  std::vector<std::deque<std::pair<Play, std::string>>>>>
        branches;
    analyseMax(position, stalemate, nMoves, pseudoLegalMoves, branches,
               translate, includeVariations, includeThreats,
               includeShortVariations, includeSetPlay, includeTries,
               includeActualPlay, includeActualPlay, logMoves);
    std::cout << toFormatted(toFlattened(branches)) << std::endl;
  }
  if (!includeActualPlay) {
    if (includeSetPlay) {
      std::cout << "Illegal position in actual play." << std::endl;
    } else {
      std::cout << "Illegal position." << std::endl;
    }
  }
}
void BattlePlay::analyseMax(
    Position& position, bool stalemate, int depth,
    const std::vector<std::shared_ptr<Move>>& pseudoLegalMovesMax,
    std::vector<
        std::pair<std::pair<Play, std::string>,
                  std::vector<std::deque<std::pair<Play, std::string>>>>>&
        branches,
    int translate, bool includeVariations, bool includeThreats,
    bool includeShortVariations, bool includeSetPlay, int includeTries,
    bool includeActualPlay, bool markKeys, bool logMoves) {
  if (includeSetPlay && !(depth == getTerminalDepth())) {
    std::shared_ptr<Move> move = std::make_shared<NullMove>();
    std::vector<std::shared_ptr<Move>> pseudoLegalMovesMin;
    if (move->make(position, pseudoLegalMovesMin)) {
      int score = searchMin(position, stalemate, depth, pseudoLegalMovesMin, 0);
      std::vector<
          std::pair<std::pair<Play, std::string>,
                    std::vector<std::deque<std::pair<Play, std::string>>>>>
          variations;
      if (score > 0) {
        analyseMin(position, stalemate, depth - score + 1, pseudoLegalMovesMin,
                   variations, translate, includeVariations, includeThreats,
                   includeShortVariations, true);
      } else {
        analyseMin(position, stalemate, depth, pseudoLegalMovesMin, variations,
                   translate, includeVariations, includeThreats,
                   includeShortVariations, true);
      }
      branches.push_back({{Play::SET, "null"}, toFlattened(variations)});
      if (logMoves) {
        if (score >= 0) {
          logger(std::clog) << "depth=" << depth << " move=*" << *move
                            << " score=" << score << std::endl;
        } else {
          logger(std::clog) << "depth=" << depth << " move=*" << *move
                            << " score<0" << std::endl;
        }
      }
    } else {
      std::cout << "Illegal position in set play." << std::endl;
    }
    move->unmake(position);
  }
  if (includeActualPlay) {
    for (const std::shared_ptr<Move>& move : pseudoLegalMovesMax) {
      std::vector<std::shared_ptr<Move>> pseudoLegalMovesMin;
      std::ostringstream lanBuilder;
      if (move->make(position, pseudoLegalMovesMin, lanBuilder, translate)) {
        int score = searchMin(position, stalemate, depth, pseudoLegalMovesMin,
                              includeTries);
        if (score > 0) {
          if (includeVariations && !(depth == getTerminalDepth())) {
            std::vector<std::pair<
                std::pair<Play, std::string>,
                std::vector<std::deque<std::pair<Play, std::string>>>>>
                variations;
            analyseMin(position, stalemate, depth - score + 1,
                       pseudoLegalMovesMin, variations, translate, true,
                       includeThreats, includeShortVariations, false);
            move->postWrite(position, pseudoLegalMovesMin, lanBuilder);
            if (markKeys) {
              branches.push_back(
                  {{Play::KEY, lanBuilder.str()}, toFlattened(variations)});
            } else {
              branches.push_back({{Play::CONTINUATION, lanBuilder.str()},
                                  toFlattened(variations)});
            }
          } else {
            move->postWrite(position, pseudoLegalMovesMin, lanBuilder);
            if (markKeys) {
              branches.push_back({{Play::KEY, lanBuilder.str()}, {}});
            } else {
              branches.push_back({{Play::CONTINUATION, lanBuilder.str()}, {}});
            }
          }
        } else if (score >= -includeTries) {
          std::vector<
              std::pair<std::pair<Play, std::string>,
                        std::vector<std::deque<std::pair<Play, std::string>>>>>
              variations;
          analyseMin(position, stalemate, depth, pseudoLegalMovesMin,
                     variations, translate, includeVariations, includeThreats,
                     includeShortVariations, false);
          move->postWrite(position, pseudoLegalMovesMin, lanBuilder);
          branches.push_back(
              {{Play::TRY, lanBuilder.str()}, toFlattened(variations)});
        }
        if (logMoves) {
          if (score >= -includeTries) {
            logger(std::clog) << "depth=" << depth << " move=*" << *move
                              << " score=" << score << std::endl;
          } else {
            logger(std::clog) << "depth=" << depth << " move=*" << *move
                              << " score<" << -includeTries << std::endl;
          }
        }
      }
      move->unmake(position);
    }
  }
}
void BattlePlay::analyseMin(
    Position& position, bool stalemate, int depth,
    const std::vector<std::shared_ptr<Move>>& pseudoLegalMovesMin,
    std::vector<
        std::pair<std::pair<Play, std::string>,
                  std::vector<std::deque<std::pair<Play, std::string>>>>>&
        branches,
    int translate, bool includeVariations, bool includeThreats,
    bool includeShortVariations, bool includeSetPlay) {
  if (depth == getTerminalDepth()) {
    for (const std::shared_ptr<Move>& move : pseudoLegalMovesMin) {
      std::vector<std::shared_ptr<Move>> pseudoLegalMovesMax;
      std::ostringstream lanBuilder;
      if (move->make(position, pseudoLegalMovesMax, lanBuilder, translate)) {
        move->postWrite(position, pseudoLegalMovesMax, lanBuilder);
        branches.push_back({{Play::REFUTATION, lanBuilder.str()}, {}});
      }
      move->unmake(position);
    }
  } else {
    std::vector<
        std::pair<std::pair<Play, std::string>,
                  std::vector<std::deque<std::pair<Play, std::string>>>>>
        threats;
    if (depth > 1 && includeVariations && includeThreats && !includeSetPlay) {
      std::shared_ptr<Move> move = std::make_shared<NullMove>();
      std::vector<std::shared_ptr<Move>> pseudoLegalMovesMax;
      if (move->make(position, pseudoLegalMovesMax)) {
        int score =
            searchMax(position, stalemate, depth - 1, pseudoLegalMovesMax);
        if (score > 0) {
          analyseMax(position, stalemate, depth - score, pseudoLegalMovesMax,
                     threats, translate, true, true, includeShortVariations,
                     false, 0, true, false, false);
          branches.push_back({{Play::THREAT, "null"}, toFlattened(threats)});
        } else {
          branches.push_back({{Play::ZUGZWANG, "null"}, {}});
        }
      }
      move->unmake(position);
    }
    for (const std::shared_ptr<Move>& move : pseudoLegalMovesMin) {
      std::vector<std::shared_ptr<Move>> pseudoLegalMovesMax;
      std::ostringstream lanBuilder;
      if (move->make(position, pseudoLegalMovesMax, lanBuilder, translate)) {
        int score =
            searchMax(position, stalemate, depth - 1, pseudoLegalMovesMax);
        if (score > 0) {
          if ((includeVariations || includeSetPlay) &&
              (score == 1 || includeShortVariations)) {
            std::vector<std::pair<
                std::pair<Play, std::string>,
                std::vector<std::deque<std::pair<Play, std::string>>>>>
                continuations;
            analyseMax(position, stalemate, depth - score, pseudoLegalMovesMax,
                       continuations, translate, includeVariations,
                       includeThreats, includeShortVariations, false, 0, true,
                       false, false);
            if (std::find_first_of(continuations.begin(), continuations.end(),
                                   threats.begin(),
                                   threats.end()) == continuations.end()) {
              move->postWrite(position, pseudoLegalMovesMax, lanBuilder);
              branches.push_back({{Play::VARIATION, lanBuilder.str()},
                                  toFlattened(continuations)});
            }
          }
        } else if (!includeSetPlay) {
          move->postWrite(position, pseudoLegalMovesMax, lanBuilder);
          branches.push_back({{Play::REFUTATION, lanBuilder.str()}, {}});
        }
      }
      move->unmake(position);
    }
  }
}

Directmate::Directmate(Position position, bool stalemate, int nMoves)
    : Problem(std::move(position), nMoves), MateProblem(stalemate) {}
void Directmate::solve(const AnalysisOptions& analysisOptions,
                       const DisplayOptions& displayOptions) {
  BattlePlay::solve(position_, stalemate_, nMoves_, analysisOptions.setPlay,
                    analysisOptions.nRefutations, analysisOptions.variations,
                    analysisOptions.threats, analysisOptions.shortVariations,
                    displayOptions.outputLanguage,
                    displayOptions.internalProgress);
}
int Directmate::searchMax(
    Position& position, bool stalemate, int depth,
    const std::vector<std::shared_ptr<Move>>& pseudoLegalMovesMax) {
  int max = INT_MIN;
  for (const std::shared_ptr<Move>& move : pseudoLegalMovesMax) {
    std::vector<std::shared_ptr<Move>> pseudoLegalMovesMin;
    if (move->make(position, pseudoLegalMovesMin)) {
      int score = searchMin(position, stalemate, depth, pseudoLegalMovesMin, 0);
      if (score > max) {
        max = score;
      }
    }
    move->unmake(position);
    if (max == depth) {
      break;
    }
  }
  return max;
}
int Directmate::searchMin(
    Position& position, bool stalemate, int depth,
    const std::vector<std::shared_ptr<Move>>& pseudoLegalMovesMin,
    int nRefutations) {
  int min = 0;
  if (depth == 1) {
    for (const std::shared_ptr<Move>& move : pseudoLegalMovesMin) {
      if (move->make(position)) {
        min--;
      }
      move->unmake(position);
      if (min < -nRefutations) {
        min = INT_MIN;
        break;
      }
    }
  } else {
    for (const std::shared_ptr<Move>& move : pseudoLegalMovesMin) {
      std::vector<std::shared_ptr<Move>> pseudoLegalMovesMax;
      if (move->make(position, pseudoLegalMovesMax)) {
        int score =
            searchMax(position, stalemate, depth - 1, pseudoLegalMovesMax);
        if (min == 0) {
          if (score < 0) {
            min = -1;
          } else {
            min = score;
          }
        } else if (min > 0) {
          if (score < 0) {
            min = -1;
          } else {
            if (score < min) {
              min = score;
            }
          }
        } else {
          if (score < 0) {
            min--;
          }
        }
      }
      move->unmake(position);
      if (min < -nRefutations) {
        min = INT_MIN;
        break;
      }
    }
  }
  if (min == 0) {
    if (evaluateTerminalNode(position, stalemate)) {
      min = depth;
    } else {
      min = INT_MIN;
    }
  }
  return min;
}
int Directmate::getTerminalDepth() const { return 1; }
void Directmate::write(std::ostream& output) const {
  output << "Directmate[position=" << position_ << ", stalemate=" << stalemate_
         << ", nMoves=" << nMoves_ << "]";
}

Selfmate::Selfmate(Position position, bool stalemate, int nMoves)
    : Problem(std::move(position), nMoves), MateProblem(stalemate) {}
void Selfmate::solve(const AnalysisOptions& analysisOptions,
                     const DisplayOptions& displayOptions) {
  BattlePlay::solve(position_, stalemate_, nMoves_, analysisOptions.setPlay,
                    analysisOptions.nRefutations, analysisOptions.variations,
                    analysisOptions.threats, analysisOptions.shortVariations,
                    displayOptions.outputLanguage,
                    displayOptions.internalProgress);
}
int Selfmate::searchMax(
    Position& position, bool stalemate, int depth,
    const std::vector<std::shared_ptr<Move>>& pseudoLegalMovesMax) {
  int max = 0;
  if (depth == 0) {
    for (const std::shared_ptr<Move>& move : pseudoLegalMovesMax) {
      if (move->make(position)) {
        max = INT_MIN;
      }
      move->unmake(position);
      if (max < 0) {
        break;
      }
    }
  } else {
    for (const std::shared_ptr<Move>& move : pseudoLegalMovesMax) {
      std::vector<std::shared_ptr<Move>> pseudoLegalMovesMin;
      if (move->make(position, pseudoLegalMovesMin)) {
        int score =
            searchMin(position, stalemate, depth, pseudoLegalMovesMin, 0);
        if (max == 0) {
          max = score;
        } else {
          if (score > max) {
            max = score;
          }
        }
      }
      move->unmake(position);
      if (max == depth) {
        break;
      }
    }
  }
  if (max == 0) {
    if (evaluateTerminalNode(position, stalemate)) {
      max = depth + 1;
    } else {
      max = INT_MIN;
    }
  }
  return max;
}
int Selfmate::searchMin(
    Position& position, bool stalemate, int depth,
    const std::vector<std::shared_ptr<Move>>& pseudoLegalMovesMin,
    int nRefutations) {
  int min = 0;
  for (const std::shared_ptr<Move>& move : pseudoLegalMovesMin) {
    std::vector<std::shared_ptr<Move>> pseudoLegalMovesMax;
    if (move->make(position, pseudoLegalMovesMax)) {
      int score =
          searchMax(position, stalemate, depth - 1, pseudoLegalMovesMax);
      if (min == 0) {
        if (score < 0) {
          min = -1;
        } else {
          min = score;
        }
      } else if (min > 0) {
        if (score < 0) {
          min = -1;
        } else {
          if (score < min) {
            min = score;
          }
        }
      } else {
        if (score < 0) {
          min--;
        }
      }
    }
    move->unmake(position);
    if (min < -nRefutations) {
      min = INT_MIN;
      break;
    }
  }
  if (min == 0) {
    min = INT_MIN;
  }
  return min;
}
int Selfmate::getTerminalDepth() const { return 0; }
void Selfmate::write(std::ostream& output) const {
  output << "Selfmate[position=" << position_ << ", stalemate=" << stalemate_
         << ", nMoves=" << nMoves_ << "]";
}

Helpmate::Helpmate(Position position, bool stalemate, int nMoves, bool halfMove)
    : Problem(std::move(position), nMoves),
      HelpProblem(halfMove),
      MateProblem(stalemate) {}
void Helpmate::solve(const AnalysisOptions& analysisOptions,
                     const DisplayOptions& displayOptions) {
  solve(position_, stalemate_, nMoves_, halfMove_, analysisOptions.setPlay,
        analysisOptions.tempoTries, displayOptions.outputLanguage,
        displayOptions.internalProgress);
}
void Helpmate::solve(Position& position, bool stalemate, int nMoves,
                     bool halfMove, bool includeSetPlay, bool includeTempoTries,
                     int translate, bool logMoves) {
  std::vector<std::shared_ptr<Move>> pseudoLegalMoves;
  bool includeActualPlay = position.isLegal(pseudoLegalMoves);
  if (includeActualPlay || includeSetPlay) {
    std::vector<
        std::pair<std::pair<Play, std::string>,
                  std::vector<std::deque<std::pair<Play, std::string>>>>>
        branches;
    if (halfMove) {
      analyseMax(position, stalemate, nMoves + 1, pseudoLegalMoves, branches,
                 translate, includeTempoTries, includeSetPlay,
                 includeActualPlay, logMoves);
    } else {
      analyseMin(position, stalemate, nMoves, pseudoLegalMoves, branches,
                 translate, includeTempoTries, includeSetPlay,
                 includeActualPlay, logMoves);
    }
    std::cout << toFormatted(toFlattened(branches)) << std::endl;
  }
  if (!includeActualPlay) {
    if (includeSetPlay) {
      std::cout << "Illegal position in actual play." << std::endl;
    } else {
      std::cout << "Illegal position." << std::endl;
    }
  }
}
int Helpmate::analyseMax(
    Position& position, bool stalemate, int depth,
    const std::vector<std::shared_ptr<Move>>& pseudoLegalMovesMax,
    std::vector<
        std::pair<std::pair<Play, std::string>,
                  std::vector<std::deque<std::pair<Play, std::string>>>>>&
        branchesMax,
    int translate, bool includeTempoTries, bool includeSetPlay,
    bool includeActualPlay, bool logMoves) {
  int max = 0;
  if (includeSetPlay || includeTempoTries) {
    std::shared_ptr<Move> move = std::make_shared<NullMove>();
    std::vector<std::shared_ptr<Move>> pseudoLegalMovesMin;
    if (move->make(position, pseudoLegalMovesMin)) {
      std::vector<
          std::pair<std::pair<Play, std::string>,
                    std::vector<std::deque<std::pair<Play, std::string>>>>>
          branchesMin;
      if (analyseMin(position, stalemate, depth - 1, pseudoLegalMovesMin,
                     branchesMin, translate, includeTempoTries, false, true,
                     false) != 0) {
        max++;
        if (includeSetPlay) {
          branchesMax.push_back(
              {{Play::SET, "null"}, toFlattened(branchesMin)});
        } else {
          branchesMax.push_back(
              {{Play::TEMPO_2ND, "null"}, toFlattened(branchesMin)});
        }
      }
      if (logMoves) {
        logger(std::clog) << "depth=" << depth << " move=*" << *move
                          << " branches.size()=" << branchesMax.size()
                          << std::endl;
      }
    } else {
      if (includeSetPlay) {
        std::cout << "Illegal position in set play." << std::endl;
      }
    }
    move->unmake(position);
  }
  if (includeActualPlay) {
    for (const std::shared_ptr<Move>& move : pseudoLegalMovesMax) {
      std::vector<std::shared_ptr<Move>> pseudoLegalMovesMin;
      std::ostringstream lanBuilder;
      if (move->make(position, pseudoLegalMovesMin, lanBuilder, translate)) {
        std::vector<
            std::pair<std::pair<Play, std::string>,
                      std::vector<std::deque<std::pair<Play, std::string>>>>>
            branchesMin;
        if (analyseMin(position, stalemate, depth - 1, pseudoLegalMovesMin,
                       branchesMin, translate, includeTempoTries, false, true,
                       false) != 0) {
          max++;
          move->postWrite(position, pseudoLegalMovesMin, lanBuilder);
          branchesMax.push_back(
              {{Play::HELP_2ND, lanBuilder.str()}, toFlattened(branchesMin)});
        }
        if (logMoves) {
          logger(std::clog)
              << "depth=" << depth << " move=*" << *move
              << " branches.size()=" << branchesMax.size() << std::endl;
        }
      }
      move->unmake(position);
    }
  }
  return max;
}
int Helpmate::analyseMin(
    Position& position, bool stalemate, int depth,
    const std::vector<std::shared_ptr<Move>>& pseudoLegalMovesMin,
    std::vector<
        std::pair<std::pair<Play, std::string>,
                  std::vector<std::deque<std::pair<Play, std::string>>>>>&
        branchesMin,
    int translate, bool includeTempoTries, bool includeSetPlay,
    bool includeActualPlay, bool logMoves) {
  int min = 0;
  int nLegalMoves = 0;
  if (depth == 0) {
    for (const std::shared_ptr<Move>& move : pseudoLegalMovesMin) {
      if (move->make(position)) {
        nLegalMoves++;
      }
      move->unmake(position);
      if (nLegalMoves != 0) {
        break;
      }
    }
  } else {
    if (includeSetPlay || includeTempoTries) {
      std::shared_ptr<Move> move = std::make_shared<NullMove>();
      std::vector<std::shared_ptr<Move>> pseudoLegalMovesMax;
      if (move->make(position, pseudoLegalMovesMax)) {
        std::vector<
            std::pair<std::pair<Play, std::string>,
                      std::vector<std::deque<std::pair<Play, std::string>>>>>
            branchesMax;
        if (analyseMax(position, stalemate, depth, pseudoLegalMovesMax,
                       branchesMax, translate, includeTempoTries, false, true,
                       false) != 0) {
          min++;
          if (includeSetPlay) {
            branchesMin.push_back(
                {{Play::SET, "null"}, toFlattened(branchesMax)});
          } else {
            branchesMin.push_back(
                {{Play::TEMPO_1ST, "null"}, toFlattened(branchesMax)});
          }
        }
        if (logMoves) {
          logger(std::clog)
              << "depth=" << depth << " move=*" << *move
              << " branches.size()=" << branchesMin.size() << std::endl;
        }
      } else {
        if (includeSetPlay) {
          std::cout << "Illegal position in set play." << std::endl;
        }
      }
      move->unmake(position);
    }
    if (includeActualPlay) {
      for (const std::shared_ptr<Move>& move : pseudoLegalMovesMin) {
        std::vector<std::shared_ptr<Move>> pseudoLegalMovesMax;
        std::ostringstream lanBuilder;
        if (move->make(position, pseudoLegalMovesMax, lanBuilder, translate)) {
          nLegalMoves++;
          std::vector<
              std::pair<std::pair<Play, std::string>,
                        std::vector<std::deque<std::pair<Play, std::string>>>>>
              branchesMax;
          if (analyseMax(position, stalemate, depth, pseudoLegalMovesMax,
                         branchesMax, translate, includeTempoTries, false, true,
                         false) != 0) {
            min++;
            move->postWrite(position, pseudoLegalMovesMax, lanBuilder);
            branchesMin.push_back(
                {{Play::HELP_1ST, lanBuilder.str()}, toFlattened(branchesMax)});
          }
          if (logMoves) {
            logger(std::clog)
                << "depth=" << depth << " move=*" << *move
                << " branches.size()=" << branchesMin.size() << std::endl;
          }
        }
        move->unmake(position);
      }
    }
  }
  if (nLegalMoves == 0) {
    if (includeActualPlay) {
      if (evaluateTerminalNode(position, stalemate)) {
        min = 1;
      } else {
        min = 0;
      }
    }
  }
  return min;
}
void Helpmate::write(std::ostream& output) const {
  output << "Helpmate[position=" << position_ << ", stalemate=" << stalemate_
         << ", nMoves=" << nMoves_ << ", halfMove=" << halfMove_ << "]";
}

MateSearch::MateSearch(Position position, int nMoves)
    : Problem(std::move(position), nMoves) {}
void MateSearch::solve(const AnalysisOptions& analysisOptions,
                       const DisplayOptions& displayOptions) {
  solve(position_, nMoves_, displayOptions.outputLanguage);
}
void MateSearch::solve(Position& position, int nMoves, int translate) {
  std::vector<std::shared_ptr<Move>> pseudoLegalMovesMax;
  if (position.isLegal(pseudoLegalMovesMax)) {
    std::vector<std::pair<std::string, std::string>> points;
    for (const std::shared_ptr<Move>& move : pseudoLegalMovesMax) {
      std::vector<std::shared_ptr<Move>> pseudoLegalMovesMin;
      std::ostringstream lanBuilder;
      if (move->make(position, pseudoLegalMovesMin, lanBuilder, translate)) {
        for (int depth = 1; depth <= nMoves; depth++) {
          int score = searchMin(position, depth, pseudoLegalMovesMin);
          if (score > 0) {
            move->postWrite(position, pseudoLegalMovesMin, lanBuilder);
            points.push_back({"+M" + std::to_string(depth), lanBuilder.str()});
            break;
          }
        }
      }
      move->unmake(position);
    }
    std::cout << toOrderedAndFormatted(points) << std::endl;
  } else {
    std::cout << "Illegal position." << std::endl;
  }
}
int MateSearch::searchMax(
    Position& position, int depth,
    const std::vector<std::shared_ptr<Move>>& pseudoLegalMovesMax) {
  int max = -1;
  for (const std::shared_ptr<Move>& move : pseudoLegalMovesMax) {
    std::vector<std::shared_ptr<Move>> pseudoLegalMovesMin;
    if (move->make(position, pseudoLegalMovesMin)) {
      max = searchMin(position, depth, pseudoLegalMovesMin);
    }
    move->unmake(position);
    if (max > 0) {
      break;
    }
  }
  return max;
}
int MateSearch::searchMin(
    Position& position, int depth,
    const std::vector<std::shared_ptr<Move>>& pseudoLegalMovesMin) {
  int min = 0;
  if (depth == 1) {
    for (const std::shared_ptr<Move>& move : pseudoLegalMovesMin) {
      if (move->make(position)) {
        min = -1;
      }
      move->unmake(position);
      if (min < 0) {
        break;
      }
    }
  } else {
    for (const std::shared_ptr<Move>& move : pseudoLegalMovesMin) {
      std::vector<std::shared_ptr<Move>> pseudoLegalMovesMax;
      if (move->make(position, pseudoLegalMovesMax)) {
        min = searchMax(position, depth - 1, pseudoLegalMovesMax);
      }
      move->unmake(position);
      if (min < 0) {
        break;
      }
    }
  }
  if (min == 0) {
    if (evaluateTerminalNode(position, false)) {
      min = 1;
    } else {
      min = -1;
    }
  }
  return min;
}
void MateSearch::write(std::ostream& output) const {
  output << "MateSearch[position=" << position_ << ", nMoves=" << nMoves_
         << "]";
}

Perft::Perft(Position position, int nMoves, bool halfMove)
    : Problem(std::move(position), nMoves), HelpProblem(halfMove) {}
void Perft::solve(const AnalysisOptions& analysisOptions,
                  const DisplayOptions& displayOptions) {
  solve(position_, nMoves_, halfMove_);
}
void Perft::solve(Position& position, int nMoves, bool halfMove) {
  std::vector<std::shared_ptr<Move>> pseudoLegalMoves;
  if (position.isLegal(pseudoLegalMoves)) {
    long nNodes;
    if (halfMove) {
      nNodes = analyse(position, nMoves * 2 + 1, pseudoLegalMoves);
    } else {
      nNodes = analyse(position, nMoves * 2, pseudoLegalMoves);
    }
    std::cout << nNodes << std::endl;
  } else {
    std::cout << "Illegal position." << std::endl;
  }
}
long Perft::analyse(
    Position& position, int depth,
    const std::vector<std::shared_ptr<Move>>& pseudoLegalMoves) {
  if (depth == 0) {
    return 1;
  }
  long nNodes = 0;
  for (const std::shared_ptr<Move>& move : pseudoLegalMoves) {
    std::vector<std::shared_ptr<Move>> pseudoLegalMovesNext;
    if (move->make(position, pseudoLegalMovesNext)) {
      nNodes += analyse(position, depth - 1, pseudoLegalMovesNext);
    }
    move->unmake(position);
  }
  return nNodes;
}
void Perft::write(std::ostream& output) const {
  output << "Perft[position=" << position_ << ", nMoves=" << nMoves_
         << ", halfMove=" << halfMove_ << "]";
}

}  // namespace moderato
