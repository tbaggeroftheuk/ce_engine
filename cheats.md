| Specifier | Type             | Example                                 | Notes                              |
| --------- | ---------------- | --------------------------------------- | ---------------------------------- |
| `%d`      | `int`            | `printf("%d", 42);`                     | Signed decimal integer             |
| `%i`      | `int`            | `printf("%i", 42);`                     | Same as `%d`                       |
| `%u`      | `unsigned int`   | `printf("%u", 42);`                     | Unsigned decimal integer           |
| `%x`      | `unsigned int`   | `printf("%x", 255);` → `ff`             | Hex, lowercase                     |
| `%X`      | `unsigned int`   | `printf("%X", 255);` → `FF`             | Hex, uppercase                     |
| `%o`      | `unsigned int`   | `printf("%o", 8);` → `10`               | Octal                              |
| `%f`      | `float`/`double` | `printf("%f", 3.14);`                   | Fixed-point decimal                |
| `%e`      | `float`/`double` | `printf("%e", 314.0);` → `3.140000e+02` | Scientific notation                |
| `%g`      | `float`/`double` | `printf("%g", 3.14);`                   | Chooses `%f` or `%e` automatically |
| `%c`      | `char`           | `printf("%c", 'A');`                    | Single character                   |
| `%s`      | `char *`         | `printf("%s", "hello");`                | Null-terminated string             |
| `%%`      | N/A              | `printf("%%");` → `%`                   | Prints a literal `%`               |
