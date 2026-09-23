package parser_java;

import java.io.*;

public class Validator {

    public static void validateInputFile(String path) throws IOException {
        File file = new File(path);
        if (!file.exists()) {
            throw new IOException("Входной файл не существует: " + path);
        }
        if (!file.isFile()) {
            throw new IOException("Указанный inputPath не является файлом: " + path);
        }
        if (!file.canRead()) {
            throw new IOException("Нет прав на чтение файла: " + path);
        }
        if (file.length() == 0) {
            throw new IOException("Входной файл пуст: " + path);
        }
    }

    public static void validateOutputFile(String path) throws IOException {
        File file = new File(path);
        File parent = file.getAbsoluteFile().getParentFile();
        if (parent != null && !parent.exists()) {
            throw new IOException("Директория для выходного файла не существует: " + parent);
        }
        if (parent != null && !parent.canWrite()) {
            throw new IOException("Нет прав на запись в директорию: " + parent);
        }
        if (file.exists() && !file.canWrite()) {
            throw new IOException("Нет прав на перезапись файла: " + path);
        }
    }
}
