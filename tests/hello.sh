#!/bin/bash

test_name=$(basename "$0" .sh)
t=out/tests/$test_name

mkdir -p "$t"
# echo $t

CC=riscv64-linux-gnu-gcc

cat <<EOF | $CC -o "$t"/a.o -c -xc -
#include <stdio.h>
int main(void) {
    printf("Hello, World\n");
    return 0;
}
EOF

# riscv64-linux-gnu-gcc -B .. -static ./out/tests/hello/a.o -o ./out/tests/hello/out
$CC -B . -static "$t"/a.o -o "$t"/out
qemu-riscv64 "$t"/out