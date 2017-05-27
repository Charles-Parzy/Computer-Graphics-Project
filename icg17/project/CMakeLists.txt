# the exercise name is nothing else than the directory
get_filename_component(EXERCISENAME ${CMAKE_CURRENT_LIST_DIR} NAME)
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

# list all the files you want to copy everytime
# you build (ie, you want the shaders there)
file(GLOB SHADERS
  terrain/terrain_vshader.glsl
  terrain/terrain_fshader.glsl
  heightmap/heightmap_vshader.glsl
  heightmap/heightmap_fshader.glsl
  skybox/skybox_vshader.glsl
  skybox/skybox_fshader.glsl
  waveheightmap/*.glsl
  wavenormalmap/*.glsl)
deploy_shaders_to_build_dir(${SHADERS})

add_executable(${EXERCISENAME} ${SOURCES} ${HEADERS} ${SHADERS})
target_link_libraries(${EXERCISENAME} ${COMMON_LIBS})
