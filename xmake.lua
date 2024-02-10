add_rules("mode.debug", "mode.release")

add_requires("glog", "spdlog","gtest", "boost", "fmt",{configs = {
    program_options = true
}})

add_includedirs("src/utils")


local PROJECT_DIR = os.projectdir()

--task("test")
--on_run(function ()
--    os.exec("xmake f -m debug --test=y")
--    os.exec("xmake build -g test")
--    os.exec("xmake run -g test")
--end)

target("rvld")
    set_languages("c++23")
    set_kind("binary")
    add_files("src/*.cpp", "src/utils/file/*.cpp")
    add_packages("boost", "spdlog")
    add_defines(string.format("PROJECT_DIR=\"%s\"", PROJECT_DIR))

-- target("test_main")
--     set_languages("c++23")
--     set_kind("binary")
--     add_includedirs("src")
--     add_files("src/*.cpp|main.cpp", "src/utils/file/*.cpp","tests/*.cpp")
--     add_packages("gtest", "boost", "spdlog")
--     add_defines(string.format("PROJECT_DIR=\"%s\"", PROJECT_DIR))

target("test")
    set_languages("c++23")
    set_kind("binary")
    set_default(false)
    add_includedirs("src/utils")
    add_files("src/utils/file/*.cpp", "tests/TestFile.cpp")
    add_packages("boost", "spdlog", "gtest")
    add_tests("default")
    add_defines(string.format("PROJECT_DIR=\"%s\"", PROJECT_DIR))
