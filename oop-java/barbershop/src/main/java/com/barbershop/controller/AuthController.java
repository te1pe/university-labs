package com.barbershop.controller;

import com.barbershop.dto.RegisterForm;
import com.barbershop.service.UserService;
import jakarta.validation.Valid;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.validation.BindingResult;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.ModelAttribute;
import org.springframework.web.bind.annotation.PostMapping;

@Controller
public class AuthController {

    private final UserService userService;

    public AuthController(UserService userService) {
        this.userService = userService;
    }

    @GetMapping("/login")
    public String login() {
        return "login";
    }

    @GetMapping("/register")
    public String registerForm(Model model) {
        if (!model.containsAttribute("form")) {
            model.addAttribute("form", new RegisterForm());
        }
        return "register";
    }

    @PostMapping("/register")
    public String register(@Valid @ModelAttribute("form") RegisterForm form,
                           BindingResult br) {
        if (userService.emailExists(form.getEmail())) {
            br.rejectValue("email", "email.exists", "Этот e-mail уже зарегистрирован");
        }
        if (br.hasErrors()) {
            return "register";
        }
        userService.register(form);
        return "redirect:/login?registered";
    }
}
