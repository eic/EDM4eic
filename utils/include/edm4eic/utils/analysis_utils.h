// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2022 Whitney Armstrong, Sylvester Joosten

#ifndef EDM4EIC_UTILS_ANALYSIS_HH
#define EDM4EIC_UTILS_ANALYSIS_HH

#include <algorithm>
#include <cmath>
#include <exception>
#include <limits>
#include <string>
#include <vector>

#include <Math/Vector4D.h>

#include <edm4eic/ReconstructedParticleCollection.h>
#include <edm4eic/ReconstructedParticleData.h>
#include <edm4eic/TrackParametersCollection.h>

namespace edm4eic {

/** Four momentum from track and mass.
 * Get a vector of 4-momenta from raw tracking info, using an externally
 * provided particle mass assumption.
 */
inline auto
momenta_from_tracking(const std::vector<edm4eic::TrackParametersData>& tracks,
                      const double mass) {
  std::vector<ROOT::Math::PxPyPzMVector> momenta{tracks.size()};
  // transform our raw tracker info into proper 4-momenta
  std::transform(tracks.begin(), tracks.end(), momenta.begin(),
                 [mass](const auto& track) {
                   // make sure we don't divide by zero
                   if (fabs(track.qOverP) < 1e-9) {
                     return ROOT::Math::PxPyPzMVector{};
                   }
                   const double p = fabs(1. / track.qOverP);
                   const double px = p * cos(track.phi) * sin(track.theta);
                   const double py = p * sin(track.phi) * sin(track.theta);
                   const double pz = p * cos(track.theta);
                   return ROOT::Math::PxPyPzMVector{px, py, pz, mass};
                 });
  return momenta;
}
} // namespace edm4eic
#endif
