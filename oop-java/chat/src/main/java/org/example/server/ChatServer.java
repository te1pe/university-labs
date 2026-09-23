package org.example.server;

import org.java_websocket.WebSocket;
import org.java_websocket.handshake.ClientHandshake;
import org.java_websocket.server.WebSocketServer;

import java.net.InetSocketAddress;

public class ChatServer extends WebSocketServer {
    private final ChatCommandHandler handler;

    public ChatServer(int port, ChatCommandHandler handler) {
        super(new InetSocketAddress(port));
        this.handler = handler;
    }

    // методы ниже переопределяют WebSocketServer — их имена менять нельзя,
    // иначе библиотека не сможет их вызвать
    @Override
    public void onOpen(WebSocket conn, ClientHandshake handshake) {
        handler.send_rooms(conn); // сразу шлём новому клиенту список комнат
    }

    @Override
    public void onClose(WebSocket conn, int code, String reason, boolean remote) {
        handler.handle_disconnect(conn);
    }

    @Override
    public void onMessage(WebSocket conn, String message) {
        handler.handle_message(conn, message);
    }

    @Override
    public void onError(WebSocket conn, Exception ex) {
        System.err.println("Ошибка сокета: " + ex.getMessage());
    }

    @Override
    public void onStart() {
        System.out.println("Сервер запущен на порту " + getPort());
    }
}