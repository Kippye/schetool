######### INSTALL #########
cmake_minimum_required(VERSION 3.25)

install(TARGETS ${PROGRAM_NAME}
    DESTINATION "."
)
install(FILES "README.md"
    DESTINATION "."
)
install(DIRECTORY "fonts"
    DESTINATION "."
)
install(DIRECTORY "textures"
    DESTINATION "."
)