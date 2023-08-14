// SPDX-License-Identifier: Apache-2.0

#include "edm4eic/utils/dataframe.h"

#include "edm4eic/RawTrackerHitData.h"

#include <ROOT/RDataFrame.hxx>

#include <iostream>

int main(int argc, char* argv[]) {

  ROOT::EnableImplicitMT();

  std::cout << "Create RDataFrame ..." << std::endl;

  ROOT::RDataFrame df("events", "edm4eic_events.root");

  std::cout << "Apply selectors and define new branches ..." << std::endl;
  auto df2 = df.Define("MCParticles_pt", edm4eic::utils::pt<edm4eic::MCParticleData>, {"MCParticles"})
                 .Define("MCParticles_eta", edm4eic::utils::eta<edm4eic::MCParticleData>, {"MCParticles"})
                 .Define("MCParticles_cosTheta", edm4eic::utils::cos_theta<edm4eic::MCParticleData>, {"MCParticles"})
                 .Define("SimTrackerHits_r", edm4eic::utils::r<edm4eic::SimTrackerHitData>, {"SimTrackerHits"})
                 .Define("SimTrackerHit_pt", edm4eic::utils::pt<edm4eic::SimTrackerHitData>, {"SimTrackerHits"})
                 .Define("TrackerHits_r", edm4eic::utils::r<edm4eic::TrackerHitPlaneData>, {"TrackerHitPlanes"});

  std::string outfilename = "edm4eic_events_rdf.root";
  std::cout << "Writing snapshot to disk ... \t" << outfilename << std::endl;

  df2.Snapshot("events", outfilename,
               {"MCParticles_pt", "MCParticles_eta", "MCParticles_cosTheta", "SimTrackerHits_r", "SimTrackerHit_pt",
                "TrackerHits_r"});

  return 0;
}
