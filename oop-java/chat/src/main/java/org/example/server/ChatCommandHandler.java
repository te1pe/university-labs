package org.example.server;

import com.google.gson.Gson;
import org.example.MessageDTO;
import org.java_websocket.WebSocket;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Base64;
import java.util.Collection;
import java.util.Collections;
import java.util.List;
import java.util.Map;

public class ChatCommandHandler {
    private final Gson gson = new Gson();
    private final UserManager user_manager;
    private final RoomManager room_manager;
    private final HistoryManager history_manager;
    private final Map<String, List<MessageDTO>> room_history; // комната -> список сообщений
    private final String uploads_dir;

    public ChatCommandHandler(UserManager um, RoomManager rm, HistoryManager hm, String uploads_dir) {
        this.user_manager = um;
        this.room_manager = rm;
        this.history_manager = hm;
        this.uploads_dir = uploads_dir;
        this.room_history = hm.load_history();

        // восстанавливаем комнаты из сохранённой истории
        for (String room : room_history.keySet()) {
            room_manager.create_room(room);
        }
        // комната по умолчанию, чтобы было куда зайти сразу
        if (!room_history.containsKey("general")) {
            room_manager.create_room("general");
            room_history.put("general", Collections.synchronizedList(new ArrayList<>()));
        }

        new File(uploads_dir).mkdirs(); // папка для загруженных файлов
    }

    public void handle_message(WebSocket conn, String json) {
        MessageDTO msg = gson.fromJson(json, MessageDTO.class);
        if (msg == null || msg.type == null) return;

        switch (msg.type) {
            case "register"    -> handle_register(conn, msg);
            case "login"       -> handle_login(conn, msg);
            case "get_rooms"   -> send_rooms(conn);
            case "create_room" -> handle_create_room(msg);
            case "join"        -> handle_join(conn, msg);
            case "message"     -> handle_text(msg);
            case "file"        -> handle_file(msg);
            default -> System.err.println("Неизвестный тип сообщения: " + msg.type);
        }
    }

    private void handle_register(WebSocket conn, MessageDTO msg) {
        boolean ok = user_manager.register_user(msg.user, msg.password);
        MessageDTO res = new MessageDTO();
        res.type = "register";
        res.success = ok;
        res.info = ok ? "Регистрация успешна, теперь войдите" : "Логин занят или данные пустые";
        conn.send(gson.toJson(res));
    }

    private void handle_login(WebSocket conn, MessageDTO msg) {
        boolean ok = user_manager.check_user(msg.user, msg.password);
        MessageDTO res = new MessageDTO();
        res.type = "login";
        res.success = ok;
        res.info = ok ? "Вход выполнен" : "Неверный логин или пароль";
        conn.send(gson.toJson(res));
    }

    private void handle_create_room(MessageDTO msg) {
        if (msg.room == null || msg.room.isBlank()) return;
        room_manager.create_room(msg.room);
        room_history.putIfAbsent(msg.room, Collections.synchronizedList(new ArrayList<>()));
        history_manager.save_history(room_history);
        broadcast_rooms();
    }

    private void handle_join(WebSocket conn, MessageDTO msg) {
        // если пользователь был в другой комнате — убираем его оттуда
        String old = room_manager.get_user_room(conn);
        if (old != null) {
            room_manager.remove_from_room(old, conn);
            broadcast_users(old);
        }

        room_manager.add_user(conn, msg.user, msg.room);
        room_history.putIfAbsent(msg.room, Collections.synchronizedList(new ArrayList<>()));

        // отправляем вошедшему историю комнаты
        MessageDTO hist = new MessageDTO();
        hist.type = "history";
        hist.messages = new ArrayList<>(room_history.get(msg.room));
        conn.send(gson.toJson(hist));

        // системное уведомление о входе
        MessageDTO sys = new MessageDTO();
        sys.type = "system";
        sys.room = msg.room;
        sys.text = msg.user + " вошёл в чат";
        room_history.get(msg.room).add(sys);
        history_manager.save_history(room_history);

        broadcast_to_room(msg.room, sys);
        broadcast_users(msg.room);
    }

    private void handle_text(MessageDTO msg) {
        if (msg.room == null) return;
        room_history.putIfAbsent(msg.room, Collections.synchronizedList(new ArrayList<>()));
        room_history.get(msg.room).add(msg);
        history_manager.save_history(room_history);
        broadcast_to_room(msg.room, msg);
    }

    private void handle_file(MessageDTO msg) {
        if (msg.room == null || msg.fileName == null || msg.data == null) return;

        // сохраняем копию файла на сервере (декодируем base64 в байты)
        try {
            byte[] bytes = Base64.getDecoder().decode(msg.data);
            String safe = System.currentTimeMillis() + "_" + msg.fileName;
            try (FileOutputStream out = new FileOutputStream(uploads_dir + File.separator + safe)) {
                out.write(bytes);
            }
        } catch (IOException | IllegalArgumentException e) {
            System.err.println("Не удалось сохранить файл на сервере: " + e.getMessage());
        }

        // в историю и в рассылку кладём сообщение целиком (с base64),
        // чтобы получатели могли сохранить файл у себя
        room_history.putIfAbsent(msg.room, Collections.synchronizedList(new ArrayList<>()));
        room_history.get(msg.room).add(msg);
        history_manager.save_history(room_history);
        broadcast_to_room(msg.room, msg);
    }

    public void handle_disconnect(WebSocket conn) {
        String room = room_manager.get_user_room(conn);
        room_manager.remove_user(conn);
        if (room != null) {
            room_manager.remove_from_room(room, conn);
            broadcast_users(room);
        }
    }

    public void send_rooms(WebSocket conn) {
        MessageDTO msg = new MessageDTO();
        msg.type = "rooms";
        msg.rooms = new ArrayList<>(room_manager.get_all_rooms());
        conn.send(gson.toJson(msg));
    }

    private void broadcast_rooms() {
        MessageDTO msg = new MessageDTO();
        msg.type = "rooms";
        msg.rooms = new ArrayList<>(room_manager.get_all_rooms());
        String json = gson.toJson(msg);
        for (WebSocket conn : get_all_connections()) {
            conn.send(json);
        }
    }

    private void broadcast_to_room(String room, MessageDTO msg) {
        String json = gson.toJson(msg);
        for (WebSocket conn : room_manager.get_room_connections(room)) {
            conn.send(json);
        }
    }

    private void broadcast_users(String room) {
        MessageDTO msg = new MessageDTO();
        msg.type = "users";
        msg.users = room_manager.get_users_in_room(room);
        broadcast_to_room(room, msg);
    }

    private Collection<WebSocket> get_all_connections() {
        return room_manager.get_all_connections();
    }
}