package com.barbershop.controller;

import com.barbershop.entity.BarberService;
import com.barbershop.repository.BarberServiceRepository;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;

import java.util.List;

@Controller
public class PageController {

    private final BarberServiceRepository services;

    public PageController(BarberServiceRepository services) {
        this.services = services;
    }

    @GetMapping("/")
    public String index(Model model) {
        List<BarberService> all = services.findAllByOrderByIdAsc();
        List<BarberService> top = all.size() > 4 ? all.subList(0, 4) : all;
        model.addAttribute("topServices", top);
        return "index";
    }

    @GetMapping("/services")
    public String services(Model model) {
        model.addAttribute("services", services.findAllByOrderByIdAsc());
        return "services";
    }
}
