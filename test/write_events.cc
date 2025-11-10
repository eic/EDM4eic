// SPDX-License-Identifier: Apache-2.0

#include "write_events.h"

#include <podio/podioVersion.h>
#if PODIO_BUILD_VERSION >= PODIO_VERSION(0, 99, 0)
#include <podio/ROOTWriter.h>
#else
#include <podio/ROOTFrameWriter.h>
namespace podio {
using ROOTWriter = podio::ROOTFrameWriter;
}
#endif

int main(int argc, char *argv[]) {

  write<podio::ROOTWriter>("edm4eic_events.root");
}
