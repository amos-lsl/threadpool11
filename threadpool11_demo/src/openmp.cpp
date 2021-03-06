/*!
 * threadpool11
 * Copyright (C) 2013, 2014, 2015, 2016, 2017, 2028  Tolga HOSGOR
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "threadpool11/threadpool11.hpp"

#include <iostream>

std::size_t factorial(std::size_t i) {
  i = std::max(1ul, i);

  std::size_t res = i;

  while (i > 2) {
    res *= --i;
  }

  return res;
}

int main(int argc, char* argv[]) {
  threadpool11::Pool pool;

  std::cout << "Your machine's hardware concurrency is " << std::thread::hardware_concurrency() << std::endl
            << std::endl;

  const constexpr auto iter = 300000ul;
  std::array<volatile std::size_t, iter> a;

  {
    std::array<std::future<std::size_t>, iter> futures;

    const auto begin = std::chrono::high_resolution_clock::now();

    for (auto i = 0u; i < iter; ++i) {
      futures[i] = pool.postWork<std::size_t>([i]() { return factorial(i); });
    }

    for (auto i = 0u; i < iter; ++i) {
      a[i] = futures[i].get();
    }

    const auto end = std::chrono::high_resolution_clock::now();
    std::cout << "threadpool11 execution took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
              << " milliseconds." << std::endl << std::endl;
  }

  {
    std::array<volatile std::size_t, iter> a;

    const auto begin = std::chrono::high_resolution_clock::now();

#pragma omp parallel for
    for (auto i = 0u; i < iter; ++i) {
      a[i] = factorial(i);
    }

    const auto end = std::chrono::high_resolution_clock::now();
    std::cout << "openmp execution took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
              << " milliseconds." << std::endl << std::endl;
  }

  return 0;
}
