package com.barbershop.repository;

import com.barbershop.entity.Master;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.List;

public interface MasterRepository extends JpaRepository<Master, Long> {

    List<Master> findAllByOrderByIdAsc();
}
