package com.barbershop.dto;

public class AppointmentView {

    private final String day;
    private final String month;
    private final String time;
    private final String serviceName;
    private final String masterName;
    private final String statusLabel;
    private final String statusClass;

    public AppointmentView(String day, String month, String time,
                           String serviceName, String masterName,
                           String statusLabel, String statusClass) {
        this.day = day;
        this.month = month;
        this.time = time;
        this.serviceName = serviceName;
        this.masterName = masterName;
        this.statusLabel = statusLabel;
        this.statusClass = statusClass;
    }

    public String getDay() { return day; }
    public String getMonth() { return month; }
    public String getTime() { return time; }
    public String getServiceName() { return serviceName; }
    public String getMasterName() { return masterName; }
    public String getStatusLabel() { return statusLabel; }
    public String getStatusClass() { return statusClass; }
}
