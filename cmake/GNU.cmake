set(CHROMIUM_COMPILER_FLAGS -nostdinc++ -fno-rtti)
set(WARNING_FLAGS -Wall -Wextra -Wpedantic -Werror=switch-enum -Werror=return-type -Werror=reorder -Werror=implicit-fallthrough -Werror=missing-field-initializers -fmax-errors=3)
set(coverage_debug_flags -fno-inline -g3 -ggdb3 -O0 )
