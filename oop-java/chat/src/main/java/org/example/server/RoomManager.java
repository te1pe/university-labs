package org.example.server;

import org.java_websocket.WebSocket;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

public class RoomManager {
    private final Map<String, Set<WebSocket>> rooms = new ConcurrentHashMap<>(); // комната -> подключения
    private final Map<WebSocket, String> users = new ConcurrentHashMap<>();       // подключение -> имя
    private final Map<WebSocket, String> conn_rooms = new ConcurrentHashMap<>();  // подключение -> комната

    public void add_user(WebSocket conn, String user, String room) {
        users.put(conn, user);
        conn_rooms.put(conn, room);
        rooms.computeIfAbsent(room, k -> Collections.newSetFromMap(new ConcurrentHashMap<>())).add(conn);
    }

    public String get_user_room(WebSocket conn) {
        return conn_rooms.get(conn);
    }

    public void remove_from_room(String room, WebSocket conn) {
        Set<WebSocket> set = rooms.get(room);
        if (set != null) set.remove(conn);
    }

    public String remove_user(WebSocket conn) {
        conn_rooms.remove(conn);
        return users.remove(conn);
    }

    public void create_room(String room) {
        rooms.putIfAbsent(room, Collections.newSetFromMap(new ConcurrentHashMap<>()));
    }

    public Set<String> get_all_rooms() {
        return rooms.keySet();
    }

    public Set<WebSocket> get_room_connections(String room) {
        return rooms.getOrDefault(room, Collections.emptySet());
    }

    public Collection<WebSocket> get_all_connections() {
        return users.keySet();
    }

    public List<String> get_users_in_room(String room) {
        Set<WebSocket> set = rooms.get(room);
        if (set == null) return Collections.emptyList();
        List<String> result = new ArrayList<>();
        for (WebSocket conn : set) {
            String name = users.get(conn);
            if (name != null) result.add(name);
        }
        return result;
    }
}