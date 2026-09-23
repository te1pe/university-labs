# Барбершоп

Регистрация/вход, главная с контактами, список услуг,
онлайн-запись и личный кабинет с записями.

## Стек
- Java 21, Spring Boot 3.3.5
- Spring MVC + Thymeleaf 
- Spring Security
- Spring Data JPA + Hibernate
- PostgreSQL

## 1. База данных


```bash
brew install postgresql@16
brew services start postgresql@16
```

```bash
psql postgres
```
```sql
CREATE USER barber WITH PASSWORD 'barber';
CREATE DATABASE barbershop OWNER barber;
GRANT ALL PRIVILEGES ON DATABASE barbershop TO barber;
\q
```

## 2. Запуск
1. File → Open → выбрать папку `barbershop`
2. Дождаться окончания индексации и загрузки зависимостей.
3. Запустить
4. Открыть в браузере: http://localhost:8080

## 3. Демо-доступ
- E-mail: `demo@ostrie.ru`
- Пароль: `demo1234`

## Структура
```
src/main/java/com/barbershop/
  entity/      — User, BarberService, Master, Appointment, AppointmentStatus
  repository/  — Spring Data JPA
  service/     — UserService, AppointmentService
  controller/  — Auth, Page, Cabinet, Booking
  config/      — SecurityConfig, GlobalControllerAdvice, DataInitializer
  dto/         — RegisterForm, BookingForm, AppointmentView
  util/        — RuDate
src/main/resources/
  templates/   — Thymeleaf/
  static/css/  — style.css
  static/images/works/ — work-1..6.jpg
  application.properties
```
