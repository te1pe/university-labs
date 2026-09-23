package com.barbershop.controller;

import com.barbershop.entity.User;
import com.barbershop.service.AppointmentService;
import com.barbershop.service.UserService;
import com.barbershop.util.RuDate;
import org.springframework.security.core.Authentication;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;

@Controller
public class CabinetController {

    private final UserService userService;
    private final AppointmentService appointmentService;

    public CabinetController(UserService userService,
                            AppointmentService appointmentService) {
        this.userService = userService;
        this.appointmentService = appointmentService;
    }

    @GetMapping("/cabinet")
    public String cabinet(Authentication auth, Model model) {
        User u = userService.byEmail(auth.getName());

        model.addAttribute("appts", appointmentService.listForUser(u));
        model.addAttribute("visits", appointmentService.listForUser(u).size());
        model.addAttribute("memberSince", RuDate.memberSince(u.getCreatedAt()));
        model.addAttribute("firstName", firstName(u.getFullName()));
        model.addAttribute("initials", initials(u.getFullName()));
        return "cabinet";
    }

    private String firstName(String full) {
        if (full == null || full.isBlank()) return "гость";
        return full.trim().split("\\s+")[0];
    }

    private String initials(String full) {
        if (full == null || full.isBlank()) return "?";
        String[] parts = full.trim().split("\\s+");
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < parts.length && i < 2; i++) {
            if (!parts[i].isEmpty()) sb.append(Character.toUpperCase(parts[i].charAt(0)));
        }
        return sb.toString();
    }
}
