macro(qss_add_test component)
    set(tname "test-${component}")
    set(tprog "test-${component}")
    add_executable(${tprog} ${component}.cpp ${SSQT_AUTO_QT5_SOURCES})
    add_dependencies(${tprog} ${SSQT_CoreLib})
    target_link_libraries(${tprog}
        Qt5::Core Qt5::Network Qt5::Test
        ${SSQT_CoreLib} ${botan2_LIBRARIES} ${SSQT_AUTO_QT5_LIBRARIES}
    )
    add_test(NAME ${tname} COMMAND $<TARGET_FILE:${tprog}>)
endmacro()

qss_add_test(chacha)
qss_add_test(cipher)
qss_add_test(address)
qss_add_test(profile)
qss_add_test(encryptor)
