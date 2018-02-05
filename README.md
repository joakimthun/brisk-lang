## Brisk
A tiny toy “language” developed as an excuse to learn more about x86-x64 and code generation. The compiler currently generates a pe/coff object file and then uses Microsoft's linker to generate an executable.

### Hello Brisk!
```
pkg main

ext fn puts(u8 *str) -> i32

fn main() -> i32 {
  let str = "Hello World!"
  puts(str)
  ret 0
}
```
