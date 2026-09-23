package com.barbershop.dto;

public class BookingForm {

    private Long serviceId;
    private Long masterId;
    private String date;
    private String time;
    private String comment;

    public Long getServiceId() { return serviceId; }
    public void setServiceId(Long serviceId) { this.serviceId = serviceId; }

    public Long getMasterId() { return masterId; }
    public void setMasterId(Long masterId) { this.masterId = masterId; }

    public String getDate() { return date; }
    public void setDate(String date) { this.date = date; }

    public String getTime() { return time; }
    public void setTime(String time) { this.time = time; }

    public String getComment() { return comment; }
    public void setComment(String comment) { this.comment = comment; }
}
