if(true)
    # Download git repo & build
    XmakeDepRepoBuild(  libgtest
        REPO_URL    https://github.com/google/googletest.git
        CONFIG_CMD  ${CMAKE_COMMAND} -E make_directory build
            COMMAND cd build && ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}"
                -DCMAKE_BUILD_TYPE=${DEPS_BUILD_TYPE}
                -DCMAKE_INSTALL_PREFIX=${DEPS_INSTALL_DIR} ..
        BUILD_CMD   ${MAKE_PROG} -C build
        INSTALL_CMD ${MAKE_PROG} -C build install
    )
else()
    # Download tarball & build
    XmakeDepTarballBuild(libgtest
        VERSION      1.8.1
        URL          https://github.com/google/googletest/archive/release-1.8.1.tar.gz
        SHA256       9bf1fe5182a604b4135edc1a425ae356c9ad15e9b23f9f12a02e80184c3a249c
        CONFIG_CMD   ${CMAKE_COMMAND} -E make_directory build
            COMMAND  cd build && ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}"
                -DCMAKE_BUILD_TYPE=${DEPS_BUILD_TYPE}
                -DCMAKE_INSTALL_PREFIX=${DEPS_INSTALL_DIR} ..
        BUILD_CMD   ${MAKE_PROG} -C build
        INSTALL_CMD ${MAKE_PROG} -C build install
    )
endif()

# This determines the thread library of the system, see 'FindThreads.cmake'
find_package(Threads REQUIRED)
#message(STATUS "THREAD_LIBS=${CMAKE_THREAD_LIBS_INIT}") # the thread library
#message(STATUS "THREAD_USE_SPROC=${CMAKE_USE_SPROC_INIT}") # are we using sproc?
#message(STATUS "THREAD_USE_WIN32=${CMAKE_USE_WIN32_THREADS_INIT}") # using WIN32 threads?
#message(STATUS "THREAD_USE_PTHREADS=${CMAKE_USE_PTHREADS_INIT}") # are we using pthreads
#message(STATUS "THREAD_HP_PTHREADS=${CMAKE_HP_PTHREADS_INIT}") # are we using hp pthreads

list(APPEND ${XMAKE}_GTEST_LIBRARIES ${DEPS_INSTALL_DIR}/lib/libgtest.a)
list(APPEND ${XMAKE}_GTEST_LIBRARIES ${DEPS_INSTALL_DIR}/lib/libgmock_main.a)
list(APPEND ${XMAKE}_GTEST_LIBRARIES ${DEPS_INSTALL_DIR}/lib/libgmock.a)
list(APPEND ${XMAKE}_GTEST_LIBRARIES ${DEPS_INSTALL_DIR}/lib/libgtest_main.a)
list(APPEND ${XMAKE}_GTEST_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
