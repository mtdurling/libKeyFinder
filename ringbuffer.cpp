/*************************************************************************

  Copyright 2011-2013 Ibrahim Sha'ath

  This file is part of LibKeyFinder.

  LibKeyFinder is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  LibKeyFinder is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with LibKeyFinder.  If not, see <http://www.gnu.org/licenses/>.

*************************************************************************/

#include "ringbuffer.h"

#include <iostream>

namespace KeyFinder {

  RingBuffer::RingBuffer(unsigned int s) : buffer(s) {
    if (s == 0) throw Exception("Size must be > 0");
    zeroIndex = 0;
  }

  unsigned int RingBuffer::getSize() const {
    return buffer.size();
  }

  void RingBuffer::clear() {
    for (unsigned int i = 0; i < buffer.size(); i++) {
        buffer[i] = 0.0;
    }
    zeroIndex = 0;
  }

  void RingBuffer::shiftZeroIndex(int count) {
    zeroIndex += count;
    while (zeroIndex < 0) zeroIndex += buffer.size();
    zeroIndex %= buffer.size();
  }

  float RingBuffer::getData(int index) const {
    index += zeroIndex;
    while (index < 0) index += buffer.size();
    index %= buffer.size();
    return buffer[index];
  }

  void RingBuffer::setData(int index, float value) {
    index += zeroIndex;
    while (index < 0) index += buffer.size();
    index %= buffer.size();
    buffer[index] = value;
  }

}