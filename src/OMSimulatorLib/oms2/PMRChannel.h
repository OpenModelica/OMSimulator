/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.2.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3,
 * ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from OSMC, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or
 * http://www.openmodelica.org, and in the OpenModelica distribution.
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

#ifndef _OMS_PMRChannel_H_
#define _OMS_PMRChannel_H_


#include <string>
#include <map>
#include <assert.h>
#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Logging.h"

namespace oms2
{
  /**
   * \brief Parallel Multi-Rate simulation communication Channel using Atomic polling.
   *
   * \warning Experimental class.
   *
   * This class uses atomic polling for synchronization. It is not recommanded to use
   * this class for (hard) real-time applications. The synchronization is done
   * using busy-waiting on atomic variables, therefore a higher-priority task can
   * block a lower priority task even if it is busy-wating for inputs
   * (hence, danger of deadlock).
   * https://stackoverflow.com/questions/16350473/why-do-i-need-stdcondition-variable
   */
  template <class T>
  class PMRChannelA
  {
  public:
    explicit PMRChannelA(int k) : k_(k), n_(0), is_produced_(false) {};
    ~PMRChannelA() noexcept {}
    PMRChannelA(const PMRChannelA& other) = delete; //!< Copy constructor
    PMRChannelA (PMRChannelA&& other) noexcept = delete; //!< Move constructor
    PMRChannelA& operator= (const PMRChannelA& other) = delete; //!< Copy assignment operator
    PMRChannelA& operator= (PMRChannelA&& other) noexcept = delete; //!< Move assignment operator

    void write(T v)
    {
      logTrace();
      while (is_produced_.load()) std::this_thread::yield;
      v_ = v;
      n_++;
      // std::cout << "store n: " << n_ << ", k: " << k_ << std::endl;
      is_produced_.store(n_ == k_);
    }

    T read()
    {
      logTrace();
      T v;
      while (!is_produced_.load()) std::this_thread::yield;
      v = v_;
      n_ = 0;
      is_produced_.store(false);
      return v;
    }

  private:
    const int k_; //!< activation ratio
    int n_; //!< A counter with the property n <= k.
    T v_; // !< storage place for value
    std::atomic<bool> is_produced_;
  };

  /**
   * \brief Parallel Multi-Rate simulation communication Channel using Condition Variables for synchronization.
   *
   * \warning Experimental class.
   *
   * This class uses condition variables for synchronization
   * https://stackoverflow.com/questions/16350473/why-do-i-need-stdcondition-variable
   */
  template <class T>
  class PMRChannelCV
  {
  public:
    explicit PMRChannelCV(int k) : k_(k), n_(0), is_produced_(false) {};
    ~PMRChannelCV() noexcept {}
    PMRChannelCV(const PMRChannelCV& other) = delete; //!< Copy constructor
    PMRChannelCV (PMRChannelCV&& other) noexcept = delete; //!< Move constructor
    PMRChannelCV& operator= (const PMRChannelCV& other) = delete; //!< Copy assignment operator
    PMRChannelCV& operator= (PMRChannelCV&& other) noexcept = delete; //!< Move assignment operator

    void write(T v) {
      logTrace();
      std::unique_lock<std::mutex> lk(m_);
      while (is_produced_) {
          cv_.wait(lk);
          // if (is_produced_) std::cout << "PMRChannelCV::store: Spurious wake up!\n";
      }
      v_ = v;
      n_++;
      if (n_ == k_) {
        is_produced_ = n_ == k_;
        lk.unlock();
        cv_.notify_one();
      }
    }

    T read() {
      logTrace();
      T v;
      std::unique_lock<std::mutex> lk(m_);
      while (!is_produced_) {
          cv_.wait(lk);
          // if (!is_produced_) std::cout << "PMRChannelCV::recv: Spurious wake up!\n";
      }
      v = v_;
      n_ = 0;
      is_produced_ = false;
      lk.unlock();
      cv_.notify_one();
      return v;
    }

  private:
    const int k_; //!< activation ratio
    int n_; //!< A counter with the property n <= k.
    T v_; // !< storage place for value
    bool is_produced_;
    std::mutex m_;
    std::condition_variable cv_;
  };
}

#endif // _OMS_PMRChannel_H_
