package com.barbershop.service;

import com.barbershop.dto.AppointmentView;
import com.barbershop.entity.*;
import com.barbershop.repository.AppointmentRepository;
import com.barbershop.repository.BarberServiceRepository;
import com.barbershop.repository.MasterRepository;
import com.barbershop.util.RuDate;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.time.LocalDate;
import java.time.LocalDateTime;
import java.time.LocalTime;
import java.util.*;

@Service
public class AppointmentService {

    private static final String ANY_MASTER = "Любой мастер";

    private final AppointmentRepository appts;
    private final BarberServiceRepository services;
    private final MasterRepository masters;

    public AppointmentService(AppointmentRepository appts,
                              BarberServiceRepository services,
                              MasterRepository masters) {
        this.appts = appts;
        this.services = services;
        this.masters = masters;
    }

    @Transactional
    public Appointment book(User u, Long serviceId, Long masterId,
                            LocalDate date, LocalTime time, String comment) {

        BarberService s = services.findById(serviceId)
                .orElseThrow(() -> new IllegalArgumentException("Услуга не найдена"));
        Master m = masters.findById(masterId)
                .orElseThrow(() -> new IllegalArgumentException("Мастер не найден"));

        LocalDateTime startsAt = LocalDateTime.of(date, time);
        if (startsAt.isBefore(LocalDateTime.now())) {
            throw new IllegalArgumentException("Нельзя записаться на прошедшее время");
        }
        if (!ANY_MASTER.equals(m.getName()) && appts.existsByMasterAndStartsAt(m, startsAt)) {
            throw new IllegalArgumentException("Это время у мастера уже занято — выберите другое");
        }

        Appointment a = new Appointment();
        a.setUser(u);
        a.setService(s);
        a.setMaster(m);
        a.setStartsAt(startsAt);
        a.setComment(comment == null ? null : comment.trim());
        a.setStatus(AppointmentStatus.CONFIRMED);
        a.setCreatedAt(LocalDateTime.now());
        return appts.save(a);
    }

    public List<AppointmentView> listForUser(User u) {
        List<AppointmentView> out = new ArrayList<>();
        for (Appointment a : appts.findByUserOrderByStartsAtDesc(u)) {
            LocalDateTime t = a.getStartsAt();
            out.add(new AppointmentView(
                    RuDate.day2(t),
                    RuDate.month(t.getMonthValue()),
                    RuDate.time(t),
                    a.getService().getName(),
                    a.getMaster().getName(),
                    a.getStatus().getLabel(),
                    a.getStatus().getCssClass()
            ));
        }
        return out;
    }

    public Appointment findOwned(Long id, User u) {
        Appointment a = appts.findById(id)
                .orElseThrow(() -> new IllegalArgumentException("Запись не найдена"));
        if (!a.getUser().getId().equals(u.getId())) {
            throw new IllegalArgumentException("Нет доступа к этой записи");
        }
        return a;
    }
}
