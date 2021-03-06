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

#include "keyfindertest.h"

TEST (KeyFinderTest, BasicUseCase) {
  unsigned int sampleRate = 44100;
  KeyFinder::AudioData inputAudio;
  inputAudio.setChannels(1);
  inputAudio.setFrameRate(sampleRate);
  inputAudio.addToSampleCount(sampleRate);
  for (unsigned int i = 0; i < sampleRate; i++) {
    float sample = 0.0;
    sample += sine_wave(i, 440.0000, sampleRate, 1);
    sample += sine_wave(i, 523.2511, sampleRate, 1);
    sample += sine_wave(i, 659.2551, sampleRate, 1);
    inputAudio.setSample(i, sample);
  }
  KeyFinder::KeyFinder kf;
  ASSERT_EQ(KeyFinder::A_MINOR, kf.keyOfAudio(inputAudio).globalKeyEstimate);
}

TEST (KeyFinderTest, ProgressiveUseCase) {

  /*
   * Build a second of audio, to be added ten times. The default settings will
   * lead to a downsample factor of 10, so there'll be 44100 samples of audio
   * after pre-processing. That'll be 7 hops, with 15428 samples left in the
   * buffer. Then finish that off with finalChromagramOfAudio, which should add
   * 4 more hops and leave 12288 zeroed samples in the buffer.
   */

  unsigned int sampleRate = 44100;
  KeyFinder::AudioData inputAudio;
  inputAudio.setFrameRate(sampleRate);
  inputAudio.setChannels(1);
  inputAudio.addToSampleCount(sampleRate);
  for (unsigned int i = 0; i < sampleRate; i++) {
    float sample = 0.0;
    sample += sine_wave(i, 440.0000, sampleRate, 1);
    sample += sine_wave(i, 523.2511, sampleRate, 1);
    sample += sine_wave(i, 659.2551, sampleRate, 1);
    inputAudio.setSample(i, sample);
  }

  KeyFinder::KeyFinder k;
  KeyFinder::Workspace w;
  KeyFinder::FftAdapter* testFftPointer = NULL;
  for (unsigned int i = 0; i < 10; i++) {
    k.progressiveChromagram(inputAudio, w);
    // ensure we're using the same FFT adapter throughout
    if (testFftPointer == NULL) testFftPointer = w.getFftAdapter();
    ASSERT_EQ(testFftPointer, w.getFftAdapter());
  }

  ASSERT_EQ(4410, w.buffer.getFrameRate());
  ASSERT_EQ(1, w.buffer.getChannels());

  // progressive result without emptying buffer
  ASSERT_EQ(7, w.chroma->getHops());
  ASSERT_EQ(15428, w.buffer.getSampleCount());

  // after emptying buffer
  k.finalChromagram(w);
  ASSERT_EQ(11, w.chroma->getHops());
  ASSERT_EQ(12288, w.buffer.getSampleCount());
  for (unsigned int i = 0; i < w.buffer.getSampleCount(); i++)
    ASSERT_FLOAT_EQ(0.0, w.buffer.getSample(i));

  ASSERT_EQ(KeyFinder::A_MINOR, k.keyOfChromagram(w).globalKeyEstimate);
}

TEST (KeyFinderTest, KeyOfChromagramReturnsSilence) {
  KeyFinder::Workspace w;
  w.chroma = new KeyFinder::Chromagram(1,1,1);
  KeyFinder::KeyFinder kf;
  KeyFinder::KeyDetectionResult kdr = kf.keyOfChromagram(w);
  ASSERT_EQ(KeyFinder::SILENCE, kdr.globalKeyEstimate);
  ASSERT_EQ(1, kdr.segments.size());
  ASSERT_EQ(12, kdr.segments[0].chromaVector.size());
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[0]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[1]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[2]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[3]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[4]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[5]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[6]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[7]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[8]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[9]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[10]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[11]);
}

TEST (KeyFinderTest, KeyOfChromagramPassesThroughChromaData) {
  KeyFinder::Workspace w;
  w.chroma = new KeyFinder::Chromagram(1,1,1);
  w.chroma->setMagnitude(0, 0, 1.0);
  w.chroma->setMagnitude(0, 3, 1.0);
  w.chroma->setMagnitude(0, 7, 1.0);
  KeyFinder::KeyFinder kf;

  KeyFinder::KeyDetectionResult kdr = kf.keyOfChromagram(w);
  ASSERT_FLOAT_EQ(1.0, kdr.segments[0].chromaVector[0]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[1]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[2]);
  ASSERT_FLOAT_EQ(1.0, kdr.segments[0].chromaVector[3]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[4]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[5]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[6]);
  ASSERT_FLOAT_EQ(1.0, kdr.segments[0].chromaVector[7]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[8]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[9]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[10]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[11]);
  ASSERT_EQ(KeyFinder::C_MINOR, kdr.globalKeyEstimate);
}

TEST (KeyFinderTest, KeyOfChromagramCollapsesTimeDimension) {
  KeyFinder::Workspace w;
  w.chroma = new KeyFinder::Chromagram(5,1,1);
  for (int i = 0; i < 5; i++) {
    w.chroma->setMagnitude(i, 0, 1.0);
    w.chroma->setMagnitude(i, 3, 1.0);
    w.chroma->setMagnitude(i, 7, 1.0);
  }
  KeyFinder::KeyFinder kf;
  KeyFinder::KeyDetectionResult kdr = kf.keyOfChromagram(w);
  ASSERT_FLOAT_EQ(5.0, kdr.segments[0].chromaVector[0]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[1]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[2]);
  ASSERT_FLOAT_EQ(5.0, kdr.segments[0].chromaVector[3]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[4]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[5]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[6]);
  ASSERT_FLOAT_EQ(5.0, kdr.segments[0].chromaVector[7]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[8]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[9]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[10]);
  ASSERT_FLOAT_EQ(0.0, kdr.segments[0].chromaVector[11]);
  ASSERT_EQ(KeyFinder::C_MINOR, kdr.globalKeyEstimate);
}

TEST (KeyFinderTest, FlatBeatRegressionTest) {
  KeyFinder::KeyFinder kf;
  KeyFinder::Parameters p;
  KeyFinder::KeyDetectionResult kdr;
  KeyFinder::Workspace w;
  w.chroma = new KeyFinder::Chromagram(1,1,1);

  // Flat Beat by Mr Oizo gets a different result from each of the tone
  // profiles; this chroma vector represents it.
  w.chroma->setMagnitude(0,  0, 2236193024);
  w.chroma->setMagnitude(0,  1, 1869016576);
  w.chroma->setMagnitude(0,  2, 2052115584);
  w.chroma->setMagnitude(0,  3, 1794053632);
  w.chroma->setMagnitude(0,  4, 1920909568);
  w.chroma->setMagnitude(0,  5, 1918255616);
  w.chroma->setMagnitude(0,  6, 1902896640);
  w.chroma->setMagnitude(0,  7, 2394525184);
  w.chroma->setMagnitude(0,  8, 2541725952);
  w.chroma->setMagnitude(0,  9, 3349090304);
  w.chroma->setMagnitude(0, 10, 3699921408);
  w.chroma->setMagnitude(0, 11, 3248228096);

  p.setToneProfile(KeyFinder::TONE_PROFILE_KRUMHANSL);
  kdr = kf.keyOfChromagram(w, p);
  ASSERT_EQ(KeyFinder::G_MINOR, kdr.globalKeyEstimate);

  p.setToneProfile(KeyFinder::TONE_PROFILE_TEMPERLEY);
  kdr = kf.keyOfChromagram(w, p);
  ASSERT_EQ(KeyFinder::B_FLAT_MAJOR, kdr.globalKeyEstimate);

  p.setToneProfile(KeyFinder::TONE_PROFILE_GOMEZ);
  kdr = kf.keyOfChromagram(w, p);
  ASSERT_EQ(KeyFinder::E_FLAT_MINOR, kdr.globalKeyEstimate);

  p.setToneProfile(KeyFinder::TONE_PROFILE_SHAATH);
  kdr = kf.keyOfChromagram(w, p);
  ASSERT_EQ(KeyFinder::B_MINOR, kdr.globalKeyEstimate);
}
