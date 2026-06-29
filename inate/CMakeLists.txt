add_executable(inate main.cpp pty.cxx)
target_sources(inate PUBLIC FILE_SET CXX_MODULES FILES pty.ixx)
