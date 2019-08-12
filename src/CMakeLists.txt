set(JISHO_FILES
    main.cc
    definition.cc
    curl.cc
    sqlite.cc)
set(EXECUTABLE_OUTPUT_PATH ${JISHO_BINARY_DIR})

add_executable(jisho ${JISHO_FILES})
TARGET_LINK_LIBRARIES(jisho curl boost_program_options sqlite3)
