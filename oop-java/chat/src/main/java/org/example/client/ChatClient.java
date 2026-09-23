package org.example.client;

import com.google.gson.Gson;
import javafx.application.Application;
import javafx.application.Platform;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.Alert;
import javafx.scene.control.Button;
import javafx.scene.control.ButtonType;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import javafx.scene.control.ListView;
import javafx.scene.control.PasswordField;
import javafx.scene.control.ScrollPane;
import javafx.scene.control.Separator;
import javafx.scene.control.TextField;
import javafx.scene.control.TextInputDialog;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
import javafx.scene.layout.VBox;
import javafx.stage.FileChooser;
import javafx.stage.Stage;
import org.example.MessageDTO;
import org.java_websocket.client.WebSocketClient;
import org.java_websocket.handshake.ServerHandshake;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.URI;
import java.nio.file.Files;
import java.util.Base64;
import java.util.List;

public class ChatClient extends Application {
    private final Gson gson = new Gson();
    private WebSocketClient client;

    private Stage stage;
    private String username;
    private String current_room;

    // элементы окна входа
    private TextField login_field;
    private PasswordField pass_field;
    private Label status_label;

    // элементы окна чата
    private VBox message_area;
    private TextField input_field;
    private ListView<String> user_list;
    private ComboBox<String> room_box;
    private boolean updating_rooms = false; // защита от лишнего события при обновлении списка комнат

    @Override
    public void start(Stage stage) {
        this.stage = stage;
        stage.setTitle("Java Chat");
        stage.setScene(build_login_scene());
        stage.show();
        connect(); // подключаемся к серверу сразу, чтобы можно было войти/зарегистрироваться
    }

    // ---------- окно входа ----------
    private Scene build_login_scene() {
        Label title = new Label("Вход в чат");
        title.setStyle("-fx-font-size: 20; -fx-font-weight: bold;");

        login_field = new TextField();
        login_field.setPromptText("Логин");

        pass_field = new PasswordField();
        pass_field.setPromptText("Пароль");

        Button login_btn = new Button("Войти");
        login_btn.setMaxWidth(Double.MAX_VALUE);
        login_btn.setOnAction(e -> do_login());

        Button reg_btn = new Button("Регистрация");
        reg_btn.setMaxWidth(Double.MAX_VALUE);
        reg_btn.setOnAction(e -> do_register());

        status_label = new Label();
        status_label.setStyle("-fx-text-fill: #C0392B;");

        VBox box = new VBox(10, title, login_field, pass_field, login_btn, reg_btn, status_label);
        box.setPadding(new Insets(30));
        box.setAlignment(Pos.CENTER);
        box.setPrefWidth(320);

        return new Scene(box, 320, 340);
    }

    private void do_login() {
        if (!check_input()) return;
        MessageDTO msg = new MessageDTO();
        msg.type = "login";
        msg.user = login_field.getText().trim();
        msg.password = pass_field.getText();
        send(msg);
    }

    private void do_register() {
        if (!check_input()) return;
        MessageDTO msg = new MessageDTO();
        msg.type = "register";
        msg.user = login_field.getText().trim();
        msg.password = pass_field.getText();
        send(msg);
    }

    private boolean check_input() {
        if (login_field.getText().trim().isEmpty() || pass_field.getText().isEmpty()) {
            status_label.setText("Заполни логин и пароль");
            return false;
        }
        return true;
    }

    // ---------- окно чата ----------
    private Scene build_chat_scene() {
        Label rooms_label = new Label("Комнаты");
        rooms_label.setStyle("-fx-font-weight: bold;");

        room_box = new ComboBox<>();
        room_box.setMaxWidth(Double.MAX_VALUE);
        room_box.setPromptText("Выберите комнату");
        room_box.setOnAction(e -> switch_room());

        Button create_btn = new Button("+ Создать комнату");
        create_btn.setMaxWidth(Double.MAX_VALUE);
        create_btn.setOnAction(e -> create_room());

        Label users_label = new Label("В сети");
        users_label.setStyle("-fx-font-weight: bold;");
        user_list = new ListView<>();

        VBox left = new VBox(10, rooms_label, room_box, create_btn, new Separator(), users_label, user_list);
        left.setPadding(new Insets(10));
        left.setPrefWidth(220);

        message_area = new VBox(8);
        message_area.setPadding(new Insets(15));
        ScrollPane scroll = new ScrollPane(message_area);
        scroll.setFitToWidth(true);

        Button file_btn = new Button("📎");
        file_btn.setOnAction(e -> send_file());

        input_field = new TextField();
        input_field.setPromptText("Сообщение...");
        HBox.setHgrow(input_field, Priority.ALWAYS);
        input_field.setOnAction(e -> send_message());

        Button send_btn = new Button("Отправить");
        send_btn.setOnAction(e -> send_message());

        HBox bottom = new HBox(8, file_btn, input_field, send_btn);
        bottom.setPadding(new Insets(10));
        bottom.setAlignment(Pos.CENTER);

        BorderPane root = new BorderPane();
        root.setLeft(left);
        root.setCenter(scroll);
        root.setBottom(bottom);

        // просим у сервера свежий список комнат
        MessageDTO ask = new MessageDTO();
        ask.type = "get_rooms";
        send(ask);

        return new Scene(root, 820, 560);
    }

    private void create_room() {
        TextInputDialog dlg = new TextInputDialog();
        dlg.setHeaderText("Название новой комнаты");
        dlg.showAndWait().ifPresent(name -> {
            if (!name.isBlank()) {
                MessageDTO msg = new MessageDTO();
                msg.type = "create_room";
                msg.room = name.trim();
                send(msg);
            }
        });
    }

    private void switch_room() {
        if (updating_rooms) return; // событие вызвано обновлением списка, а не пользователем
        String room = room_box.getValue();
        if (room == null || room.equals(current_room)) return;
        current_room = room;
        message_area.getChildren().clear();

        MessageDTO msg = new MessageDTO();
        msg.type = "join";
        msg.user = username;
        msg.room = room;
        send(msg);
    }

    private void send_message() {
        String text = input_field.getText().trim();
        if (text.isEmpty() || current_room == null) return;
        MessageDTO msg = new MessageDTO();
        msg.type = "message";
        msg.user = username;
        msg.room = current_room;
        msg.text = text;
        send(msg);
        input_field.clear();
    }

    private void send_file() {
        if (current_room == null) return;
        FileChooser chooser = new FileChooser();
        File f = chooser.showOpenDialog(stage);
        if (f == null) return;
        try {
            byte[] bytes = Files.readAllBytes(f.toPath());
            MessageDTO msg = new MessageDTO();
            msg.type = "file";
            msg.user = username;
            msg.room = current_room;
            msg.fileName = f.getName();
            msg.data = Base64.getEncoder().encodeToString(bytes); // файл -> base64
            send(msg);
        } catch (IOException e) {
            show_error("Не удалось прочитать файл: " + e.getMessage());
        }
    }

    // сохранить присланный файл на диск
    private void save_file(String fileName, String data) {
        FileChooser chooser = new FileChooser();
        chooser.setInitialFileName(fileName);
        File f = chooser.showSaveDialog(stage);
        if (f == null) return;
        try (FileOutputStream out = new FileOutputStream(f)) {
            out.write(Base64.getDecoder().decode(data));
        } catch (IOException | IllegalArgumentException e) {
            show_error("Не удалось сохранить файл: " + e.getMessage());
        }
    }

    // ---------- сеть ----------
    private void connect() {
        try {
            client = new WebSocketClient(new URI("ws://localhost:8080")) {
                // переопределения библиотеки WebSocketClient — имена менять нельзя
                @Override
                public void onOpen(ServerHandshake handshake) {
                    System.out.println("Подключено к серверу");
                }

                @Override
                public void onMessage(String message) {
                    // обработку выносим в поток JavaFX, т.к. меняем интерфейс
                    Platform.runLater(() -> handle_server_message(message));
                }

                @Override
                public void onClose(int code, String reason, boolean remote) {
                    System.out.println("Соединение закрыто: " + reason);
                }

                @Override
                public void onError(Exception ex) {
                    System.err.println("Ошибка клиента: " + ex.getMessage());
                }
            };
            client.connect();
        } catch (Exception e) {
            show_error("Не удалось подключиться: " + e.getMessage());
        }
    }

    private void send(MessageDTO msg) {
        if (client != null && client.isOpen()) {
            client.send(gson.toJson(msg));
        } else {
            show_error("Нет соединения с сервером");
        }
    }

    private void handle_server_message(String json) {
        MessageDTO msg = gson.fromJson(json, MessageDTO.class);
        if (msg == null || msg.type == null) return;

        switch (msg.type) {
            case "register" -> status_label.setText(msg.info);
            case "login" -> {
                if (msg.success) {
                    username = login_field.getText().trim();
                    stage.setScene(build_chat_scene());
                    stage.setTitle("Java Chat — " + username);
                } else {
                    status_label.setText(msg.info);
                }
            }
            case "rooms"   -> update_rooms(msg.rooms);
            case "users"   -> update_users(msg.users);
            case "history" -> {
                message_area.getChildren().clear();
                if (msg.messages != null) {
                    for (MessageDTO m : msg.messages) {
                        render_message(m);
                    }
                }
            }
            case "system"  -> add_system_message(msg.text);
            case "message" -> render_message(msg);
            case "file"    -> render_message(msg);
            default -> System.err.println("Неизвестный тип от сервера: " + msg.type);
        }
    }

    private void update_rooms(List<String> rooms) {
        if (room_box == null || rooms == null) return;
        updating_rooms = true;
        String selected = current_room;
        room_box.getItems().setAll(rooms);
        if (selected != null && rooms.contains(selected)) {
            room_box.setValue(selected);
        }
        updating_rooms = false;
    }

    private void update_users(List<String> users) {
        if (user_list == null || users == null) return;
        user_list.getItems().setAll(users);
    }

    // ---------- отрисовка сообщений ----------
    private void render_message(MessageDTO m) {
        boolean own = username != null && username.equals(m.user);
        if ("system".equals(m.type)) {
            add_system_message(m.text);
        } else if ("file".equals(m.type)) {
            add_file_message(m.user, m.fileName, m.data, own);
        } else {
            add_text_message(m.user, m.text, own);
        }
    }

    private void add_text_message(String user, String text, boolean own) {
        Label label = new Label(user + ": " + text);
        label.setWrapText(true);
        label.setMaxWidth(480);
        label.setStyle("-fx-padding: 8; -fx-background-radius: 10; -fx-background-color: "
                + (own ? "#DCF8C6" : "#FFFFFF") + "; -fx-border-color: #E0E0E0; -fx-border-radius: 10;");
        HBox row = new HBox(label);
        row.setAlignment(own ? Pos.CENTER_RIGHT : Pos.CENTER_LEFT);
        message_area.getChildren().add(row);
    }

    private void add_system_message(String text) {
        Label label = new Label(text);
        label.setStyle("-fx-text-fill: #888888; -fx-font-style: italic;");
        HBox row = new HBox(label);
        row.setAlignment(Pos.CENTER);
        message_area.getChildren().add(row);
    }

    private void add_file_message(String user, String fileName, String data, boolean own) {
        Label label = new Label("📎 " + user + ": " + fileName);
        Button save = new Button("Сохранить");
        save.setOnAction(e -> save_file(fileName, data));

        HBox bubble = new HBox(8, label, save);
        bubble.setAlignment(Pos.CENTER_LEFT);
        bubble.setStyle("-fx-padding: 8; -fx-background-radius: 10; -fx-background-color: "
                + (own ? "#DCF8C6" : "#FFFFFF") + "; -fx-border-color: #E0E0E0; -fx-border-radius: 10;");

        HBox row = new HBox(bubble);
        row.setAlignment(own ? Pos.CENTER_RIGHT : Pos.CENTER_LEFT);
        message_area.getChildren().add(row);
    }

    private void show_error(String text) {
        Alert alert = new Alert(Alert.AlertType.ERROR, text, ButtonType.OK);
        alert.showAndWait();
    }
}