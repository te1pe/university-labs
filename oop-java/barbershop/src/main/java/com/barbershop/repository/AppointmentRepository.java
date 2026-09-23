package com.barbershop.repository;

import com.barbershop.entity.Appointment;
import com.barbershop.entity.Master;
import com.barbershop.entity.User;
import org.springframework.data.jpa.repository.JpaRepository;

import java.time.LocalDateTime;
import java.util.List;

public interface AppointmentRepository extends JpaRepository<Appointment, Long> {

    List<Appointment> findByUserOrderByStartsAtDesc(User user);

    boolean existsByMasterAndStartsAt(Master master, LocalDateTime startsAt);
}
