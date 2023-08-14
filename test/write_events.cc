// SPDX-License-Identifier: Apache-2.0

#include "write_events.h"
#include "podio/ROOTFrameWriter.h"

int main(int argc, char* argv[]) {

  write<podio::ROOTFrameWriter>("edm4eic_events.root");
}
