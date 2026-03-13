# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/luca-miniati/programming/gto-plus-plus/gto++/build/_deps/eigen-src"
  "/home/luca-miniati/programming/gto-plus-plus/gto++/build/_deps/eigen-build"
  "/home/luca-miniati/programming/gto-plus-plus/gto++/build/_deps/eigen-subbuild/eigen-populate-prefix"
  "/home/luca-miniati/programming/gto-plus-plus/gto++/build/_deps/eigen-subbuild/eigen-populate-prefix/tmp"
  "/home/luca-miniati/programming/gto-plus-plus/gto++/build/_deps/eigen-subbuild/eigen-populate-prefix/src/eigen-populate-stamp"
  "/home/luca-miniati/programming/gto-plus-plus/gto++/build/_deps/eigen-subbuild/eigen-populate-prefix/src"
  "/home/luca-miniati/programming/gto-plus-plus/gto++/build/_deps/eigen-subbuild/eigen-populate-prefix/src/eigen-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/luca-miniati/programming/gto-plus-plus/gto++/build/_deps/eigen-subbuild/eigen-populate-prefix/src/eigen-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/luca-miniati/programming/gto-plus-plus/gto++/build/_deps/eigen-subbuild/eigen-populate-prefix/src/eigen-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
