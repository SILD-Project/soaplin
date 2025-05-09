nasm -f elf64 test.asm -o test.o
ld -T link.ld -nostdlib --no-dynamic-linker --strip-all test.o -o sk-hello.elf
echo "Test executable has been built successfully."