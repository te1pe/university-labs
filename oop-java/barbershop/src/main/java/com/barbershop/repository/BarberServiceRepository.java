package com.barbershop.repository;

import com.barbershop.entity.BarberService;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.List;

public interface BarberServiceRepository extends JpaRepository<BarberService, Long> {

    List<BarberService> findAllByOrderByIdAsc();
}
