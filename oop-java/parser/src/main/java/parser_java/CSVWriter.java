package parser_java;

import java.util.*;
import java.io.*;

public class CSVWriter{
    public static void write(String path, List<Map.Entry<String, Integer>> list) {
        try (FileWriter fw = new FileWriter(path)) {
            fw.write("word,count,freq\n");
            int total = 0;
            for (Map.Entry<String, Integer> e : list) {
                total += e.getValue();
            }
            for (Map.Entry<String, Integer> e : list) {
                String word = e.getKey();
                int count = e.getValue();
                double percent = 0;
                if (total != 0) {
                    percent = (count * 100.0) / total;
                }
                fw.write(word + "," + count + "," + String.format(java.util.Locale.US, "%.2f", percent) + "\n");
            }
        } catch (IOException eror) {
            eror.printStackTrace();
        }
    }

}
