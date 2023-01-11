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
    ticket_title   varchar(255) default 'Ticket system' null,
    enabled        tinyint(1)   default 1               null,
    max_ticket     int          default 0               null
);

create table if not exists ticket_access_roles
(
    id        int auto_increment
        primary key,
    server_id bigint(50) null,
    role_id   bigint(50) null
);

create table if not exists user
(
    id      int auto_increment
        primary key,
    user_id bigint null,
    coins   bigint null
);

create table if not exists user_inventory
(
    id         bigint auto_increment
        primary key,
    user_id    bigint null,
    item_id    int    null,
    item_count int    null
);

create table if not exists verify
(
    id        bigint auto_increment
        primary key,
    server_id bigint           null,
    role_id   bigint default 0 null
);