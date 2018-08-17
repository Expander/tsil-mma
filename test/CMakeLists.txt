if(Mathematica_FOUND)
  Mathematica_WolframLibrary_ADD_TEST (
    NAME test_LibraryLink
    TARGET TSIL-MMA::LibraryLink
    SCRIPT test_LibraryLink.m)
endif()
