package parser_java;

import java.util.*;
import java.util.regex.Pattern;

public class CSVLogic {
    private static final Pattern SPLIT = Pattern.compile("[^\\p{L}\\p{Nd}]+");

    public static Map<String, Integer> word_counter(String text){
        String[] words = SPLIT.split(text);
        Map<String, Integer> map = new HashMap<>();

        for (String word : words){
            if (word.isEmpty()) continue;
            word=word.toLowerCase();
            if (map.containsKey(word)){
                map.put(word, map.get(word)+1);
            }
            else{
                map.put(word, 1);
            }

        }
    return map;
    }
    public static List<Map.Entry<String, Integer>> sortByFreq(Map<String, Integer>map){
    List<Map.Entry<String, Integer>> freq= new ArrayList<>(map.entrySet());
    freq.sort((a, b) -> {
        int c = Integer.compare(b.getValue(), a.getValue());
        if (c!=0) return c;
        return a.getKey().compareTo(b.getKey());
    } );
    return freq;
    }
}
