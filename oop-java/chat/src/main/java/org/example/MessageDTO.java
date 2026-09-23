package org.example;

import java.util.List;

// единый объект сообщения: и клиент, и сервер шлют друг другу именно его в виде JSON
public class MessageDTO {
    public String type;               // тип: register/login/join/message/file/system/rooms/users/history
    public String user;               // имя пользователя
    public String room;               // название комнаты
    public String text;               // текст сообщения
    public String fileName;           // имя файла (для type="file")
    public String data;               // содержимое файла в base64
    public String password;           // пароль (только при register/login)
    public boolean success;           // результат авторизации
    public String info;               // пояснение (текст ошибки или успеха)
    public List<String> users;        // список пользователей в комнате
    public List<String> rooms;        // список всех комнат
    public List<MessageDTO> messages; // история комнаты
}