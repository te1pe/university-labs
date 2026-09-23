package com.barbershop.entity;

import jakarta.persistence.*;

@Entity
@Table(name = "barber_service")
public class BarberService {

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    @Column(nullable = false)
    private String name;

    @Column(length = 500)
    private String description;

    @Column(name = "price_rub", nullable = false)
    private int priceRub;

    public Long getId() { return id; }
    public void setId(Long id) { this.id = id; }

    public String getName() { return name; }
    public void setName(String name) { this.name = name; }

    public String getDescription() { return description; }
    public void setDescription(String description) { this.description = description; }

    public int getPriceRub() { return priceRub; }
    public void setPriceRub(int priceRub) { this.priceRub = priceRub; }
}
