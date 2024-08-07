# SPDX-License-Identifier: Apache-2.0

"""Python module for the EDM4EIC datamodel and utilities."""
import sys

from .__version__ import __version__
import ROOT
res = ROOT.gSystem.Load('libedm4eicDict.so')
if res < 0:
    raise RuntimeError('Failed to load libedm4eicDict.so')

res = ROOT.gSystem.Load('libedm4eicRDF.so')
if res < 0:
    raise RuntimeError('Failed to load libedm4eicRDF.so')

res = ROOT.gInterpreter.LoadFile('edm4eic/utils/kinematics.h')
if res != 0:
    raise RuntimeError('Failed to load kinematics.h')

res = ROOT.gInterpreter.LoadFile('edm4eic/utils/dataframe.h')
if res != 0:
    raise RuntimeError('Failed to load dataframe.h')
from ROOT import edm4eic

# Make TAB completion work for utils
setattr(edm4eic, 'utils', edm4eic.utils)

# Make `import edm4eic` work
sys.modules['edm4eic'] = edm4eic
