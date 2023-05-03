// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2023 Dmitry Kalinkin

#pragma once

namespace edm4eic {

  namespace unit {

    //
    // This provides definition for a unique unit system for this EDM. It is
    // neither Geant, nor TGeo. The DD4hep, by default, uses a system matching
    // the TGeo one.
    //
    // +------------+---------------------|-----------------+
    // | Geant      | TGeo                | EDM4hep/EDM4eic |
    // +------------+---------------------|-----------------+
    // | milimeter  | centimeter          | centimeter      |
    // | nanosecond | second              | nanosecond      |
    // | MeV        | GeV                 | GeV             |
    // +------------+---------------------|-----------------+
    //

    // distance
    static constexpr double cm = 1.0; // centimeter
    static constexpr double mm = 1e-1 * cm; // millimeter
    static constexpr double micrometer = 1e-4 * cm; // micrometer
    static constexpr double nm = 1e-7 * nm; // micrometer
    static constexpr double m = 1e2 * cm; // meter

    // time
    static constexpr ns = 1.0; // nano second
    static constexpr second = 1e9 * ns; // second
    static constexpr milisecond = 1e6 * ns; // milisecond
    static constexpr microsecond = 1e3 * ns; // milisecond
    static constexpr ps = 1e-3 * ns; // pico second

    // energy
    static constexpr GeV = 1.0; // giga electron volt
    static constexpr MeV = 1e-3 * GeV; // mega electron volt
    static constexpr keV = 1e-6 * GeV; // mega electron volt
    static constexpr eV = 1e-9 * GeV; // mega electron volt

  }

}
