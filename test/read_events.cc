// SPDX-License-Identifier: Apache-2.0

#include "read_events.h"
#include "podio/ROOTFrameReader.h"

int main() {
  read_events<podio::ROOTFrameReader>("edm4eic_events.root");

  return 0;
}
