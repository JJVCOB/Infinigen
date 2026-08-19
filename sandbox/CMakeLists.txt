# =============================================================================
#  The sandbox executable.
#
#  This is the only target that is allowed to look like a game. Everything in
#  here uses the engine's public headers and nothing else. If you find
#  yourself needing something the engine does not expose, the fix is to widen
#  the engine's public API deliberately - not to reach around it.
# =============================================================================

add_executable(sandbox src/main.cpp)

target_link_libraries(sandbox PRIVATE Engine2D::engine)

engine_set_warnings(sandbox)

# SDL supplies its own entry point on some platforms (notably Windows, where
# the real entry point is WinMain). SDL3::SDL3 handles this, but on Windows
# you also want a console window during development so your logs are visible.
if(WIN32)
    set_target_properties(sandbox PROPERTIES WIN32_EXECUTABLE FALSE)
endif()
