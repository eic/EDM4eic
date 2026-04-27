// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2022 - 2024 Whitney Armstrong, Sylvester Joosten, Wouter Deconinck

#ifndef EDM4EIC_UTILS_ANALYSIS_HH
#define EDM4EIC_UTILS_ANALYSIS_HH

#include <algorithm>
#include <cmath>
#include <vector>

#include <Math/Vector4D.h>

#include <edm4hep/MCParticleCollection.h>
#include <edm4eic/ReconstructedParticleCollection.h>
#include <edm4eic/ReconstructedParticleData.h>
#include <edm4eic/TrackParametersCollection.h>

namespace edm4eic {

/** Get a vector of 4-momenta from track parameters, using an externally
 *  provided mass assumption.
 */
inline auto
momenta(const std::vector<edm4eic::TrackParametersData>& tracks, const double mass) {
  std::vector<ROOT::Math::PxPyPzMVector> result{tracks.size()};
  std::transform(tracks.begin(), tracks.end(), result.begin(),
                 [mass](const auto& track) {
                   if (fabs(track.qOverP) < 1e-9) {
                     return ROOT::Math::PxPyPzMVector{};
                   }
                   const double p  = fabs(1. / track.qOverP);
                   const double px = p * cos(track.phi) * sin(track.theta);
                   const double py = p * sin(track.phi) * sin(track.theta);
                   const double pz = p * cos(track.theta);
                   return ROOT::Math::PxPyPzMVector{px, py, pz, mass};
                 });
  return result;
}

/** Get a vector of 4-momenta from reconstructed particles.
 */
inline auto
momenta(const std::vector<edm4eic::ReconstructedParticleData>& parts) {
  std::vector<ROOT::Math::PxPyPzMVector> result{parts.size()};
  std::transform(parts.begin(), parts.end(), result.begin(),
                 [](const auto& part) {
                   return ROOT::Math::PxPyPzMVector{part.momentum.x, part.momentum.y,
                                                    part.momentum.z, part.mass};
                 });
  return result;
}

/** Get a vector of 4-momenta from Monte Carlo particles.
 */
inline auto
momenta(const std::vector<edm4hep::MCParticleData>& parts) {
  std::vector<ROOT::Math::PxPyPzMVector> result{parts.size()};
  std::transform(parts.begin(), parts.end(), result.begin(),
                 [](const auto& part) {
                   return ROOT::Math::PxPyPzMVector{part.momentum.x, part.momentum.y,
                                                    part.momentum.z, part.mass};
                 });
  return result;
}

/** @deprecated Use momenta(tracks, mass) instead. */
[[deprecated("Use edm4eic::momenta(tracks, mass) instead")]]
inline auto
momenta_from_tracking(const std::vector<edm4eic::TrackParametersData>& tracks,
                      const double mass) {
  return momenta(tracks, mass);
}

} // namespace edm4eic
#endif
