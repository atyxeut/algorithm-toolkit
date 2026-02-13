set_xmakever("3.0.6")
set_project("aty-general-algorithm-toolkit")
set_languages("c++latest")

add_rules("mode.debug", "mode.release")

set_config("builddir", "build")
set_config("runtimes", "c++_shared")
if is_host("windows") then
  set_config("plat", "mingw")
end

-- recommended llvm binary: https://github.com/mstorsjo/llvm-mingw
if get_config("toolchain") == "llvm" and is_host("macosx") then
  set_policy("build.c++.modules", true)
  -- binary from Homebrew: brew install llvm
  add_ldflags("-L/usr/local/opt/llvm@21/lib/c++")
end

-- recommended gcc binary: https://gcc-mcf.lhmouse.com/
if get_config("toolchain") == "gcc" and is_host("windows") then
  -- gcc 16 still requires this on Windows
  add_syslinks("stdc++exp")
end

target("gatk")
  set_kind("object")
  add_files("src/**.cppm", { public = true })
target_end()

target("test_main")
  set_kind("binary")
  add_deps("gatk")
  add_files("test/main.cpp")
  set_optimize("faster")
target_end()
