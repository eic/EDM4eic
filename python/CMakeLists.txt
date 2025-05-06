# SPDX-License-Identifier: Apache-2.0

install(DIRECTORY edm4eic DESTINATION python
  REGEX .*\\.in EXCLUDE
  PATTERN __pycache__ EXCLUDE
)
