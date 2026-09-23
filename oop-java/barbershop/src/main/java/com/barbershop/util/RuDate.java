package com.barbershop.util;

import java.time.LocalDateTime;

public class RuDate {

    private static final String[] M = {
            "янв", "фев", "мар", "апр", "мая", "июн",
            "июл", "авг", "сен", "окт", "ноя", "дек"
    };

    public static String month(int m1to12) {
        return M[m1to12 - 1];
    }

    public static String day2(LocalDateTime t) {
        return String.format("%02d", t.getDayOfMonth());
    }

    public static String time(LocalDateTime t) {
        return String.format("%02d:%02d", t.getHour(), t.getMinute());
    }

    public static String label(LocalDateTime t) {
        return t.getDayOfMonth() + " " + month(t.getMonthValue()) + " " + t.getYear();
    }

    public static String memberSince(LocalDateTime t) {
        return "с " + month(t.getMonthValue()) + " " + t.getYear();
    }
}
