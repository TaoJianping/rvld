#!/bin/bash

sudo apt update
sudo apt install -y gcc-10-riscv64-linux-gnu qemu-user
sudo ln -sf /usr/bin/riscv64-linux-gnu-gcc-10 /usr/bin/riscv64-linux-gnu-gcc

out_dir=../out/tests
mkdir -p "$out_dir"

CC=riscv64-linux-gnu-gcc

cat <<EOF | $CC -o $out_dir/a.o -c -xc -
#include <stdio.h>
int main(void) {
    printf("Hello, World\n");
    return 0;
}
EOF