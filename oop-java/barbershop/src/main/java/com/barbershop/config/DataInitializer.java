package com.barbershop.config;

import com.barbershop.entity.BarberService;
import com.barbershop.entity.Master;
import com.barbershop.entity.User;
import com.barbershop.repository.BarberServiceRepository;
import com.barbershop.repository.MasterRepository;
import com.barbershop.repository.UserRepository;
import org.springframework.boot.CommandLineRunner;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.stereotype.Component;

import java.time.LocalDateTime;

@Component
public class DataInitializer implements CommandLineRunner {

    private final BarberServiceRepository services;
    private final MasterRepository masters;
    private final UserRepository users;
    private final PasswordEncoder encoder;

    public DataInitializer(BarberServiceRepository services, MasterRepository masters,
                           UserRepository users, PasswordEncoder encoder) {
        this.services = services;
        this.masters = masters;
        this.users = users;
        this.encoder = encoder;
    }

    @Override
    public void run(String... args) {
        if (services.count() == 0) {
            addService("Мужская стрижка", "Стрижка любой сложности, мытьё головы и укладка.", 1500);
            addService("Стрижка машинкой", "Короткая стрижка под одну или несколько насадок.", 900);
            addService("Моделирование бороды", "Стрижка и оформление бороды опасной бритвой.", 1000);
            addService("Королевское бритьё", "Бритьё опасной бритвой с горячим полотенцем и уходом.", 1200);
            addService("Стрижка + борода", "Полный образ за один визит — комплекс со скидкой.", 2300);
            addService("Детская стрижка", "Для юных джентльменов до 10 лет, спокойно и без слёз.", 1000);
            addService("Камуфляж седины", "Деликатное тонирование волос и бороды.", 800);
            addService("Укладка и стайлинг", "Финишная укладка премиальными средствами.", 500);
        }

        if (masters.count() == 0) {
            addMaster("Артём", "Топ-барбер · 8 лет", "А");
            addMaster("Денис", "Мастер-барбер· 5 лет", "Д");
            addMaster("Михаил", "Мега-барбер · 6 лет", "М");
            addMaster("Любой мастер", "Подберём свободного", "★");
        }

        if (!users.existsByEmail("demo@ostrie.ru")) {
            User u = new User();
            u.setFullName("Иван Соколов");
            u.setEmail("demo@ostrie.ru");
            u.setPhone("+7 999 123-45-67");
            u.setPassword(encoder.encode("demo1234"));
            u.setRole("ROLE_USER");
            u.setCreatedAt(LocalDateTime.now());
            users.save(u);
        }
    }

    private void addService(String name, String desc, int price) {
        BarberService s = new BarberService();
        s.setName(name);
        s.setDescription(desc);
        s.setPriceRub(price);
        services.save(s);
    }

    private void addMaster(String name, String role, String initials) {
        Master m = new Master();
        m.setName(name);
        m.setRole(role);
        m.setInitials(initials);
        masters.save(m);
    }
}
