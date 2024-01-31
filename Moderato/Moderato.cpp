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

#include <exception>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "Problem.h"

int main(int argc, char* argv[]) {
  std::cout << "Moderato"
#if _MSC_VER
#if _WIN64
            << " (x64)"
#elif _WIN32
            << " (x86)"
#endif
#if _DEBUG
            << " [DEBUG]"
#endif
#endif
            << " [" << __DATE__ << "] Copyright 2024 Ivan Denkovski"
            << std::endl;
  try {
    std::vector<moderato::Task> tasks;
    try {
      if (argc > 1) {
        std::ifstream input(argv[1]);
        if (input) {
          input >> tasks;
        } else {
          moderato::logger(std::cerr) << "Read failure (invalid file: \""
                                      << argv[1] << "\")." << std::endl;
        }
      } else {
        std::cin >> tasks;
      }
    } catch (const std::logic_error& failure) {
      moderato::logger(std::cerr) << failure.what() << std::endl;
      tasks.clear();
    }
    std::clog << std::boolalpha;
    for (moderato::Task& task : tasks) {
      moderato::solve(task);
    }
  } catch (const std::exception& error) {
    std::cerr << error.what() << std::endl;
  } catch (...) {
    std::cerr << "Program failure." << std::endl;
  }
}
