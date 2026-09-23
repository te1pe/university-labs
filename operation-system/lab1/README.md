# Лабораторная 1 — Компиляция, сборка, запуск

**Курс:** Операционные системы, 01.09.2025–31.12.2025

## Задание

1. Написать программу `hello.c`, выводящую «Hello world»:
   - получить исполняемый файл;
   - посмотреть unresolved symbols (`puts`, `printf`) с помощью `nm`;
   - посмотреть зависимости (`ldd`);
   - запустить и убедиться, что фраза напечатана;
   - объяснить, каким образом были разрешены unresolved symbols.
2. Написать библиотеки:
   - **статическую** с функцией `hello_from_static_lib()` и использовать её в `hello.c`:
     будет ли функция unresolved в исполняемом файле и почему; где находится её код;
   - **динамическую** с функцией `hello_from_dynamic_lib()`: посмотреть состояние
     функции в получившемся исполняемом файле, объяснить увиденное.
3. Написать динамическую библиотеку с функцией `hello_from_dyn_runtime_lib()`
   и загрузить её с помощью `dlopen(3)`. Объяснить, что происходит.

## Файлы

| Файл | Что это |
| --- | --- |
| `hello.c` | базовая программа, вывод через `printf` |
| `hellov2.c` | версия со статической библиотекой |
| `static_hello.c` / `.h` | `hello_from_static_lib()` |
| `hellov3.c` | версия с динамической библиотекой |
| `dynamic_hello.c` / `.h` | `hello_from_dynamic_lib()` |
| `hellov4.c` | загрузка библиотеки в рантайме через `dlopen`/`dlsym` |
| `runtime_hello.c` | `hello_from_dyn_runtime_lib()` |

## Сборка

```bash
# 1. базовая программа
gcc hello.c -o hello
nm hello | grep ' U '          # неразрешённые символы
ldd hello                      # зависимости

# 2. статическая библиотека
gcc -c static_hello.c -o static_hello.o
ar rcs libstatichello.a static_hello.o
gcc hellov2.c -L. -lstatichello -o hello_static

# 3. динамическая библиотека
gcc -shared -fPIC dynamic_hello.c -o libdynamichello.so
gcc hellov3.c -L. -ldynamichello -o hello_dynamic
LD_LIBRARY_PATH=. ./hello_dynamic

# 4. загрузка в рантайме
gcc -shared -fPIC runtime_hello.c -o libruntimehello.so
gcc hellov4.c -ldl -o hello_dlopen
./hello_dlopen
```

## Что показывает работа

`nm` помечает `printf`/`puts` как `U` — их код лежит в libc, а не в объектном файле.
После линковки со статической библиотекой код `hello_from_static_lib` физически
попадает внутрь исполняемого файла, и символ перестаёт быть unresolved. При
динамической линковке символ остаётся неразрешённым в самом файле и разрешается
загрузчиком `ld.so` при запуске — это видно в `ldd`. `dlopen` сдвигает момент ещё
дальше: библиотека подключается по явному вызову в произвольной точке выполнения,
её отсутствие не мешает программе запуститься, а адрес функции достаётся через
`dlsym` — в `ldd` такой библиотеки не будет.
