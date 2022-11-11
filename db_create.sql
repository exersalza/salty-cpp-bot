create database if not exists salty_cpp_bot;
use salty_cpp_bot;

create table if not exists cur_tickets
(
    id        bigint auto_increment
        primary key,
    server_id bigint null,
    user_id   bigint null,
    ticket_id bigint null
);

create table if not exists ticket
(
    id             int auto_increment
        primary key,
    server_id      bigint                               not null,
    category_id    bigint       default 0               null,
    count          int          default 0               null,
    notify_channel bigint       default 0               null,
    ticket_title   varchar(255) default 'Ticket system' null
);

create table if not exists ticket_access_roles
(
    id        int auto_increment
        primary key,
    server_id bigint(50) null,
    role_id   bigint(50) null
);

