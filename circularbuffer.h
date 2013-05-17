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

#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include "exception.h"

namespace KeyFinder {

  template <class T>
  class Binode {
  public:
    Binode(T x = 0): l(0), r(0), data(x) {}
    Binode* l, *r;
    T data;
  };

  class CircularBuffer {
  public:
    CircularBuffer(unsigned int size);
    ~CircularBuffer();
    float getData(int index) const;
    unsigned int getSize() const;
    void setData(int index, float value);
    void clear();
    void shiftZeroIndex(int count);
  protected:
    Binode<float>* p;
    unsigned int size;
  };

}

#endif