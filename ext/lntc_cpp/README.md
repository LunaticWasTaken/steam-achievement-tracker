# LNTC-CPP

### This is a header-only library for C++ 23 and higher. It contains tons of useful things that might or might not help you on your journeys.

## How to use

### Manually

- Manually include the ``lntc_cpp.hpp`` header file into your project.

### Via CMake

- Add the following to your CMakeLists.txt:

````cmake
include(FetchContent)
FetchContent_Declare(
        lntc_cpp
        GIT_REPOSITORY https://github.com/LunaticWasTaken/lntc_cpp.git
        GIT_TAG master # "master" for the latest, or a specific commit hash
)
FetchContent_MakeAvailable(lntc_cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE lntc_cpp) # Or add `lntc_cpp` to the end of your existing `target_link_libraries` call if present.
````

## Contributing

Just simply open a pull request with improvements/additions if you like.

However, keep in mind that any code you submit in PR's will be subject to the license below, and that I won't accept
random
``I don't want you to use my code anymore`` requests.

## License

Lunatic Code License

Copyright (c) 2025 Lunatic

You can use my code however you want, no restrictions. Giving me credit is appreciated, but not required.