
set(SUBHOOK_TESTS FALSE)
set(SUBHOOK_STATIC TRUE)
set(SUBHOOK_INSTALL FALSE)
add_subdirectory(subhook)

add_subdirectory(sol2)

set(ASMJIT_STATIC TRUE)
add_subdirectory(asmjit)

add_subdirectory(RFGR_Types)