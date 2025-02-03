
register_flag_required(CMAKE_CXX_COMPILER
        "Absolute path to the AMD HIP C++ compiler")

register_flag_optional(MANAGED_ALLOC "Use UVM (hipMallocManaged) instead of the device-only allocation (hipMalloc)"
        "OFF")

register_flag_optional(SYNC_ALL_KERNELS
        "Fully synchronise all kernels after launch, this also enables synchronous error checking with line and file name"
        "OFF")


macro(setup)

    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)

    set(CMAKE_CXX_COMPILER hipcc)
    set(HIPCC_FLAGS  "-Xcompiler=-Xclang -Xcompiler=-load -Xcompiler=-Xclang -Xcompiler=/g/g92/lisan1/PFA/PerfFlowAspect/src/c/install-corona-clang14/lib64/libWeavePass.so")

    set(CMAKE_CXX_FLAGS "-I /opt/rocm-5.2.3/include -D__HIP_PLATFORM_AMD__" CACHE PATH "" FORCE)
    set(CMAKE_C_FLAGS "-I /opt/rocm-5.2.3/include -D__HIP_PLATFORM_AMD__" CACHE PATH "" FORCE)

    if (MANAGED_ALLOC)
        register_definitions(CLOVER_MANAGED_ALLOC)
    endif ()

    if (SYNC_ALL_KERNELS)
        register_definitions(CLOVER_SYNC_ALL_KERNELS)
    endif ()


endmacro()