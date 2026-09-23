package com.barbershop.service;

import com.barbershop.dto.RegisterForm;
import com.barbershop.entity.User;
import com.barbershop.repository.UserRepository;
import org.springframework.security.core.authority.SimpleGrantedAuthority;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.security.core.userdetails.UserDetailsService;
import org.springframework.security.core.userdetails.UsernameNotFoundException;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.stereotype.Service;

import java.time.LocalDateTime;

@Service
public class UserService implements UserDetailsService {

    private final UserRepository users;
    private final PasswordEncoder encoder;

    public UserService(UserRepository users, PasswordEncoder encoder) {
        this.users = users;
        this.encoder = encoder;
    }

    public boolean emailExists(String email) {
        return users.existsByEmail(email.trim().toLowerCase());
    }

    public User register(RegisterForm form) {
        User u = new User();
        u.setFullName(form.getFullName().trim());
        u.setEmail(form.getEmail().trim().toLowerCase());
        u.setPhone(form.getPhone().trim());
        u.setPassword(encoder.encode(form.getPassword()));
        u.setRole("ROLE_USER");
        u.setCreatedAt(LocalDateTime.now());
        return users.save(u);
    }

    public User byEmail(String email) {
        return users.findByEmail(email.toLowerCase())
                .orElseThrow(() -> new UsernameNotFoundException("Пользователь не найден: " + email));
    }

    @Override
    public UserDetails loadUserByUsername(String email) throws UsernameNotFoundException {
        User u = byEmail(email);
        return org.springframework.security.core.userdetails.User.builder()
                .username(u.getEmail())
                .password(u.getPassword())
                .authorities(new SimpleGrantedAuthority(u.getRole()))
                .build();
    }
}
