package parser_java;

import java.io.IOException;
import java.util.*;

public class Main {
    public static void main(String[] args) {
        if (args.length < 2) {
            System.err.println("Usage: <inputPath> <outputPath>");
            System.exit(1);
        }

        String inputPath = args[0];
        String outputPath = args[1];

        try {
            Validator.validateInputFile(inputPath);
            Validator.validateOutputFile(outputPath);

            String text = FileReaderUtilite.fileread(inputPath);
            Map<String, Integer> map = CSVLogic.word_counter(text);
            List<Map.Entry<String, Integer>> sorted =
                    CSVLogic.sortByFreq(map);
            CSVWriter.write(outputPath, sorted);

        } catch (IOException e) {
            System.err.println("Ошибка: " + e.getMessage());
            System.exit(1);
        }
    }
}
