package com.barbershop.controller;

import com.barbershop.dto.BookingForm;
import com.barbershop.entity.Appointment;
import com.barbershop.entity.User;
import com.barbershop.repository.BarberServiceRepository;
import com.barbershop.repository.MasterRepository;
import com.barbershop.service.AppointmentService;
import com.barbershop.service.UserService;
import com.barbershop.util.RuDate;
import org.springframework.security.core.Authentication;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.ModelAttribute;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;

import java.time.LocalDate;
import java.time.LocalTime;
import java.util.List;

@Controller
public class BookingController {

    private static final List<String> SLOTS = List.of(
            "10:00", "10:30", "11:00", "12:00", "13:00", "14:00",
            "15:00", "16:00", "17:00", "18:00", "19:00", "20:00");

    private final BarberServiceRepository services;
    private final MasterRepository masters;
    private final AppointmentService appointmentService;
    private final UserService userService;

    public BookingController(BarberServiceRepository services,
                            MasterRepository masters,
                            AppointmentService appointmentService,
                            UserService userService) {
        this.services = services;
        this.masters = masters;
        this.appointmentService = appointmentService;
        this.userService = userService;
    }

    private void addRefData(Model model) {
        model.addAttribute("services", services.findAllByOrderByIdAsc());
        model.addAttribute("masters", masters.findAllByOrderByIdAsc());
        model.addAttribute("slots", SLOTS);
        model.addAttribute("today", LocalDate.now().toString());
    }

    @GetMapping("/booking")
    public String bookingForm(@RequestParam(value = "service", required = false) Long service,
                              Model model) {
        addRefData(model);
        BookingForm form = new BookingForm();
        if (service != null) {
            form.setServiceId(service);
        } else {
            services.findAllByOrderByIdAsc().stream().findFirst()
                    .ifPresent(s -> form.setServiceId(s.getId()));
        }
        model.addAttribute("form", form);
        return "booking";
    }

    @PostMapping("/booking")
    public String submit(@ModelAttribute("form") BookingForm form,
                         Authentication auth,
                         Model model) {
        String error = null;
        if (form.getServiceId() == null) error = "Выберите услугу";
        else if (form.getMasterId() == null) error = "Выберите мастера";
        else if (form.getDate() == null || form.getDate().isBlank()) error = "Выберите дату";
        else if (form.getTime() == null || form.getTime().isBlank()) error = "Выберите время";

        if (error == null) {
            try {
                User u = userService.byEmail(auth.getName());
                LocalDate date = LocalDate.parse(form.getDate());
                LocalTime time = LocalTime.parse(form.getTime());
                Appointment a = appointmentService.book(
                        u, form.getServiceId(), form.getMasterId(),
                        date, time, form.getComment());
                return "redirect:/booking/success/" + a.getId();
            } catch (IllegalArgumentException ex) {
                error = ex.getMessage();
            } catch (Exception ex) {
                error = "Не удалось создать запись. Проверьте дату и время.";
            }
        }

        addRefData(model);
        model.addAttribute("error", error);
        return "booking";
    }

    @GetMapping("/booking/success/{id}")
    public String success(@PathVariable Long id, Authentication auth, Model model) {
        User u = userService.byEmail(auth.getName());
        Appointment a = appointmentService.findOwned(id, u);
        model.addAttribute("appt", a);
        model.addAttribute("dateLabel", RuDate.label(a.getStartsAt()));
        model.addAttribute("time", RuDate.time(a.getStartsAt()));
        return "booking-success";
    }
}
