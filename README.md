[TOC]

EICd - EIC data model
=====================


## Overview

A podio based data model.

- https://eic.phy.anl.gov/eicd - this page
- [dd4pod](https://eic.phy.anl.gov/npdet/ref_doc/namespacedd4pod.html): data model for dd4hep output which comes with [npdet](https://eic.phy.anl.gov/npdet/).

## Full Description File

The entire data model is defined with a single YAML file. Here is the current definition:

\verbinclude eic_data.yaml


## Installing

```
mkdir build && cd build
cmake ../. -DCMAKE_INSTALL_PREFIX=$HOME/stow/eicd -DBUILD_DATA_MODEL=ON
make -j4 install
```

