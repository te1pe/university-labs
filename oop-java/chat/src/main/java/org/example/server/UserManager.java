package org.example.server;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.reflect.TypeToken;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.Reader;
import java.io.Writer;
import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.util.HashMap;
import java.util.Map;

public class UserManager {
    private final String file;               // путь к users.json
    private final Gson gson = new GsonBuilder().setPrettyPrinting().create();
    private final Map<String, String> users; // логин -> хеш пароля

    public UserManager(String file) {
        this.file = file;
        this.users = load_users();
    }

    // регистрация: false, если логин занят или данные пустые
    public synchronized boolean register_user(String user, String pass) {
        if (user == null || user.isBlank() || pass == null || pass.isEmpty()) return false;
        if (users.containsKey(user)) return false;
        users.put(user, hash_password(pass));
        save_users();
        return true;
    }

    // проверка логина и пароля при входе
    public synchronized boolean check_user(String user, String pass) {
        if (user == null || pass == null) return false;
        String hash = users.get(user);
        return hash != null && hash.equals(hash_password(pass));
    }

    // SHA-256 -> hex-строка
    private String hash_password(String pass) {
        try {
            MessageDigest md = MessageDigest.getInstance("SHA-256");
            byte[] bytes = md.digest(pass.getBytes(StandardCharsets.UTF_8));
            StringBuilder sb = new StringBuilder();
            for (byte b : bytes) {
                sb.append(String.format("%02x", b)); // каждый байт -> две hex-цифры
            }
            return sb.toString();
        } catch (Exception e) {
            // без SHA-256 авторизация невозможна — это критическая ошибка среды
            throw new RuntimeException("Нет алгоритма SHA-256: " + e.getMessage());
        }
    }

    private Map<String, String> load_users() {
        File f = new File(file);
        if (!f.exists()) return new HashMap<>();
        try (Reader r = new InputStreamReader(new FileInputStream(f), StandardCharsets.UTF_8)) {
            java.lang.reflect.Type type = new TypeToken<HashMap<String, String>>() {}.getType();
            Map<String, String> data = gson.fromJson(r, type);
            return data != null ? data : new HashMap<>();
        } catch (IOException e) {
            System.err.println("Не удалось загрузить users.json: " + e.getMessage());
            return new HashMap<>();
        }
    }

    private void save_users() {
        try (Writer w = new OutputStreamWriter(new FileOutputStream(file), StandardCharsets.UTF_8)) {
            gson.toJson(users, w);
        } catch (IOException e) {
            System.err.println("Не удалось сохранить users.json: " + e.getMessage());
        }
    }
}