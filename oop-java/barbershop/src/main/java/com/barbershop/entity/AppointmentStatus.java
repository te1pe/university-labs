package com.barbershop.entity;

public enum AppointmentStatus {

    CONFIRMED("Подтверждена", "ok"),
    DONE("Завершена", "done"),
    CANCELLED("Отменена", "done");

    private final String label;
    private final String cssClass;

    AppointmentStatus(String label, String cssClass) {
        this.label = label;
        this.cssClass = cssClass;
    }

    public String getLabel() { return label; }
    public String getCssClass() { return cssClass; }
}
