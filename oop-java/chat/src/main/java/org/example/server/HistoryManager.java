package org.example.server;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.reflect.TypeToken;
import org.example.MessageDTO;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.Reader;
import java.io.Writer;
import java.nio.charset.StandardCharsets;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class HistoryManager {
    private final String file;
    private final Gson gson = new GsonBuilder().setPrettyPrinting().create();

    public HistoryManager(String file) {
        this.file = file;
    }

    public synchronized void save_history(Map<String, List<MessageDTO>> history) {
        try (Writer w = new OutputStreamWriter(new FileOutputStream(file), StandardCharsets.UTF_8)) {
            gson.toJson(history, w);
        } catch (IOException e) {
            System.err.println("Не удалось сохранить историю: " + e.getMessage());
        }
    }

    public Map<String, List<MessageDTO>> load_history() {
        File f = new File(file);
        if (!f.exists()) return new ConcurrentHashMap<>();
        try (Reader r = new InputStreamReader(new FileInputStream(f), StandardCharsets.UTF_8)) {
            java.lang.reflect.Type type =
                    new TypeToken<ConcurrentHashMap<String, List<MessageDTO>>>() {}.getType();
            Map<String, List<MessageDTO>> data = gson.fromJson(r, type);
            return data != null ? data : new ConcurrentHashMap<>();
        } catch (IOException e) {
            System.err.println("Не удалось загрузить историю: " + e.getMessage());
            return new ConcurrentHashMap<>();
        }
    }
}