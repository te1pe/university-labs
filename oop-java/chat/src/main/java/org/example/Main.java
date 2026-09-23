package org.example;

import javafx.application.Application;
import org.example.client.ChatClient;
import org.example.server.ChatCommandHandler;
import org.example.server.ChatServer;
import org.example.server.HistoryManager;
import org.example.server.RoomManager;
import org.example.server.UserManager;

public class Main {
    public static void main(String[] args) {
        // режим запуска: "server", "client" или без аргумента — оба сразу локально
        if (args.length > 0 && args[0].equalsIgnoreCase("server")) {
            start_server();
        } else if (args.length > 0 && args[0].equalsIgnoreCase("client")) {
            start_client();
        } else {
            new Thread(Main::start_server).start();
            try {
                Thread.sleep(1000); // даём серверу подняться перед запуском клиента
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
            start_client();
        }
    }

    private static void start_server() {
        UserManager um = new UserManager("users.json");
        RoomManager rm = new RoomManager();
        HistoryManager hm = new HistoryManager("history.json");
        ChatCommandHandler handler = new ChatCommandHandler(um, rm, hm, "uploads");

        ChatServer server = new ChatServer(8080, handler);
        server.start();
    }

    private static void start_client() {
        Application.launch(ChatClient.class);
    }
}