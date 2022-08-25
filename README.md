EICd - EIC data model
=====================

A data model for EIC defined with [podio](https://github.com/AIDASoft/podio) and based on [EDM4hep](https://github.com/key4hep/EDM4hep).

## Full Description File

The entire data model is defined with a single YAML file, `eic_data.yml`.

\verbinclude eic_data.yaml

## Installing

To install the data model into `~/local`, use the following commands:
```console
git clone https://github.com/eic/eicd
cd eicd
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=~/local
cmake --build build
cmake --install build
```

