package com.barbershop.dto;

import jakarta.validation.constraints.Email;
import jakarta.validation.constraints.NotBlank;
import jakarta.validation.constraints.Size;

public class RegisterForm {

    @NotBlank(message = "Укажите имя")
    private String fullName;

    @NotBlank(message = "Укажите e-mail")
    @Email(message = "Некорректный e-mail")
    private String email;

    @NotBlank(message = "Укажите телефон")
    private String phone;

    @NotBlank(message = "Укажите пароль")
    @Size(min = 6, message = "Пароль минимум 6 символов")
    private String password;

    public String getFullName() { return fullName; }
    public void setFullName(String fullName) { this.fullName = fullName; }

    public String getEmail() { return email; }
    public void setEmail(String email) { this.email = email; }

    public String getPhone() { return phone; }
    public void setPhone(String phone) { this.phone = phone; }

    public String getPassword() { return password; }
    public void setPassword(String password) { this.password = password; }
}
