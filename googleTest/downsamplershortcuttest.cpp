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

#include "downsamplershortcuttest.h"

/*
 * TODO: investigate thread safety of FFTW. I have observed the FftAdapterTest
 * ForwardAndBackward failing when run alongside the LPF lines below, but only
 * when the two were using the same FFT frame size. Once that changed, no more
 * collision. This could get very ugly if not handled.
 */

#include <fstream>

TEST (DownsamplerShortcutTest, EverythingWorksWithShortcutFactor) {

  unsigned int channels = 2;
  unsigned int frameRate = 44100;
  unsigned int frames = frameRate * 5;
  float magnitude = 1000.0;
  float tolerance = 0.05 * magnitude;
  float highFrequency = 21000.0;
  float lowFrequency = 800.0;
  float cornerFrequency = 6500.0;
  unsigned int factor = 3;
  unsigned int filterOrder = 160;
  unsigned int filterFFT = 2048;

  // make two sine waves, several seconds long
  KeyFinder::AudioData a;
  a.setChannels(channels);
  a.setFrameRate(frameRate);
  a.addToFrameCount(frames);
  for (unsigned int i = 0; i < frames; i++) {
    float sample = 0.0;
    sample += sine_wave(i, highFrequency, frameRate, magnitude); // high freq
    sample += sine_wave(i, lowFrequency, frameRate, magnitude); // low freq
    for (unsigned int j = 0; j < channels; j++) {
      a.setSample(i, j, sample);
    }
  }

  KeyFinder::LowPassFilter* lpf = new KeyFinder::LowPassFilter(filterOrder, frameRate, cornerFrequency, filterFFT);
  KeyFinder::Workspace w;
  lpf->filter(a, w, factor);
  delete lpf;

  // test for lower wave only, and for flattening of non-useful samples
  for (unsigned int i = 0; i < frames; i++) {
    if (i % factor == 0) {
      float expected = sine_wave(i, lowFrequency, frameRate, magnitude);
      for (unsigned int j = 0; j < channels; j++) {
        ASSERT_NEAR(expected, a.getSample(i, j), tolerance);
      }
    } else {
      for (unsigned int j = 0; j < channels; j++) {
        ASSERT_FLOAT_EQ(a.getSample(i - 1, j), a.getSample(i, j));
      }
    }
  }

  KeyFinder::Downsampler ds;
  ds.downsample(a, factor);

  ASSERT_EQ(channels, a.getChannels());
  ASSERT_EQ(frameRate / factor, a.getFrameRate());
  ASSERT_EQ(frames / factor, a.getFrameCount());

  // and test for integrity of wave after downsample
  for (unsigned int i = 0; i < frames / factor; i++) {
    float expected = sine_wave(i, lowFrequency, frameRate / factor, magnitude);
    for (unsigned int j = 0; j < channels; j++) {
      ASSERT_NEAR(expected, a.getSample(i, j), tolerance);
    }
  }

}
