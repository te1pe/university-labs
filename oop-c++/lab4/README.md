# Лабораторная 4 — Потоковый обработчик WAV

**Период:** 01.09.2025 — 31.12.2025

**Язык:** C++ **Сборка:** CMake

Консольная утилита, которая применяет цепочку преобразований к WAV-файлу.
Набор и порядок преобразований задаются текстовым конфигом, а не кодом:
добавление нового конвертера не требует правок в обработке файлов.

## Использование

```bash
./wavproc -h
./wavproc -c config.txt output.wav input1.wav [input2.wav ...]
```

## Формат конфига

Одна команда на строку, `#` начинает комментарий.

| Команда | Синтаксис | Действие |
| --- | --- | --- |
| `mute` | `mute <start_sec> <end_sec>` | заглушает интервал |
| `volume` | `volume <factor>` | меняет громкость с ограничением диапазона |
| `mix` | `mix <$stream> [start_sec]` | подмешивает другой поток |

## Архитектура

```
Converter (абстрактный интерфейс)
├── set_parameters(params, input_files)
├── process(main_stream, all_streams)
└── get_description()
     │
     ├── MuteConverter     заглушение интервала
     ├── VolumeConverter   изменение громкости
     └── MixConverter      микширование потоков

ConverterFactory   создание конвертера по имени из конфига
ConfigReader       разбор конфига → цепочка конвертеров
wav_reader/writer  чтение и запись WAV
```

**Фабрика** хранит `std::unordered_map<std::string, std::function<ConverterPtr()>>`
— регистрация нового конвертера сводится к одной строке в
`register_converters()`. `ConfigReader` про конкретные конвертеры не знает
вообще, он работает только с интерфейсом.

**Справка генерируется из кода:** `get_help()` обходит все зарегистрированные
конвертеры и собирает текст из их `get_description()` — справка не может
разойтись с реальным набором команд.

Владение конвертерами — через `std::unique_ptr`, ручного `delete` в коде нет.

## Файлы

```
main.cpp              разбор аргументов, конвейер обработки
conv.h                интерфейс Converter
convmt.h/.cpp         MuteConverter
convl.h/.cpp          VolumeConverter
convm.h/.cpp          MixConverter
convf.h/.cpp          ConverterFactory
configreader.h/.cpp   разбор конфига
wavr.h/.cpp           чтение WAV
wavw.h/.cpp           запись WAV
```

## Сборка

```bash
cmake -B build && cmake --build build
```
