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

#include "spectrumanalyser.h"

namespace KeyFinder {

  SpectrumAnalyser::SpectrumAnalyser(
    unsigned int f,
    const Parameters& params,
    ChromaTransformFactory& ctFactory
  ) {
    octaves = params.getOctaves();
    bandsPerSemitone = params.getBandsPerSemitone();
    hopSize = params.getHopSize();
    ct = ctFactory.getChromaTransform(f, params);
    unsigned int fftFrameSize = params.getFftFrameSize();
    WindowFunction win;
    temporalWindow = std::vector<float>(fftFrameSize);
    for (unsigned int i = 0; i < fftFrameSize; i++) {
      temporalWindow[i] = win.window(params.getTemporalWindow(), i, fftFrameSize);
    }
  }

  Chromagram* SpectrumAnalyser::chromagramOfWholeFrames(
    const AudioData& audio,
    FftAdapter* const fft
  ) const {
    if (audio.getChannels() != 1)
      throw Exception("Audio must be monophonic to be analysed");
    unsigned int frmSize = fft->getFrameSize();
    if (audio.getSampleCount() < frmSize)
      return new Chromagram(0, octaves, bandsPerSemitone);
    unsigned int hops = 1 + ((audio.getSampleCount() - frmSize) / hopSize);
    Chromagram* c = new Chromagram(hops, octaves, bandsPerSemitone);
    for (unsigned int hop = 0; hop < hops; hop++) {
      for (unsigned int sample = 0; sample < frmSize; sample++) {
        fft->setInput(sample, audio.getSample((hop * hopSize) + sample) * temporalWindow[sample]);
      }
      fft->execute();
      std::vector<float> cv = ct->chromaVector(fft);
      for (unsigned int band = 0; band < c->getBands(); band++) {
        c->setMagnitude(hop, band, cv[band]);
      }
    }
    return c;
  }

}
