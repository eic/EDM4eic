#!/usr/bin/env python3

# SPDX-License-Identifier: Apache-2.0

import ROOT
import edm4eic

ROOT.EnableImplicitMT()

print('Create RDataFrame ...')
df = ROOT.RDataFrame('events', 'edm4eic_events.root')

print('Apply selectors and define new branches ...')
df2 = (df.Define('MCParticles_pt', 'edm4eic::utils::pt(MCParticles)')
       .Define('MCParticles_eta', 'edm4eic::utils::eta(MCParticles)')
       .Define('MCParticles_cosTheta', 'edm4eic::utils::cos_theta(MCParticles)')
       .Define('SimTrackerHits_r', 'edm4eic::utils::r(SimTrackerHits)')
       .Define('SimTrackerHits_pt', 'edm4eic::utils::pt(SimTrackerHits)')
       .Define('TrackerHits_r', 'edm4eic::utils::r(TrackerHitPlanes)')
       )

filename = 'edm4eic_events_py_rdf.root'
print(f'Writing snapshot to disk ... \t{filename}')

df2.Snapshot('events', filename,
             ['MCParticles_pt',
              'MCParticles_eta',
              'MCParticles_cosTheta',
              'SimTrackerHits_r',
              'SimTrackerHits_pt',
              'TrackerHits_r'])
