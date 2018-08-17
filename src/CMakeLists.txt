if(Mathematica_FOUND)
  set(LL_SRC librarylink.cpp)
  set(LL_LIB LibraryLink)

  Mathematica_ADD_LIBRARY(${LL_LIB} ${LL_SRC})

  target_link_libraries(${LL_LIB} PRIVATE TSIL::TSIL ${Mathematica_MathLink_LIBRARIES})
  set_target_properties(${LL_LIB} PROPERTIES LINK_FLAGS "${Mathematica_MathLink_LINKER_FLAGS}")
  target_include_directories(${LL_LIB} PRIVATE TSIL::TSIL ${Mathematica_INCLUDE_DIR} ${Mathematica_MathLink_INCLUDE_DIR})

  Mathematica_ABSOLUTIZE_LIBRARY_DEPENDENCIES(${LL_LIB})

  add_library(TSIL-MMA::LibraryLink ALIAS ${LL_LIB})
endif()
