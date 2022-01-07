drop table if exists playerStats;
drop table if exists weaponStats;
drop table if exists player;
drop table if exists game;
drop table if exists weapon;

create table player
(
    id int not null auto_increment primary key,
    
    auth varchar(255) unique,
    name varchar(255),
    
    now datetime default current_timestamp,
    active int default 1
);

create table game
(
    id int not null auto_increment primary key,
    
    server varchar(255),
    map varchar(255),
    address varchar(255),
    
    rounds int,
    score1 int,
    score2 int,
    winner int,
    
    now datetime default current_timestamp,
    active int default 1
);

create table playerStats
(
    id int not null auto_increment primary key,
    
    gameFK int,
    playerFK int,
    
    team int,
    kills int,
    assist int,
    death int,
    headshot int,
    shot int,
    hit int,
    damage int,
    money int,
    
    roundPlay int,
    roundLose int,
    roundWin int,
    roundWinShare float,
    
    bombPlant int,
    bombDefuse int,
    bombExplode int,
    
    kills1 int,
    kills2 int,
    kills3 int,
    kills4 int,
    kills5 int,
    
    versus1 int,
    versus2 int,
    versus3 int,
    versus4 int,
    versus5 int,
    
    now datetime default current_timestamp,
    active int default 1,
    
    foreign key(gameFK) references game(id),
    foreign key(playerFK) references player(id)
);

create table weapon
(
    id int not null auto_increment primary key,
    name varchar(255),
    
    now datetime default current_timestamp,
    active int default 1
);

insert into weapon values
(1, 'p228', current_timestamp, 1),
(2, 'glock', current_timestamp, 1),
(3, 'scout', current_timestamp, 1),
(4, 'hegrenade', current_timestamp, 1),
(5, 'xm1014', current_timestamp, 1),
(6, 'c4', current_timestamp, 1),
(7, 'mac10', current_timestamp, 1),
(8, 'aug', current_timestamp, 1),
(9, 'smokegrenade', current_timestamp, 1),
(10, 'elite', current_timestamp, 1),
(11, 'fiveseven', current_timestamp, 1),
(12, 'ump45', current_timestamp, 1),
(13, 'sg550', current_timestamp, 1),
(14, 'galil', current_timestamp, 1),
(15, 'famas', current_timestamp, 1),
(16, 'usp', current_timestamp, 1),
(17, 'glock18', current_timestamp, 1),
(18, 'awp', current_timestamp, 1),
(19, 'mp5navy', current_timestamp, 1),
(20, 'm249', current_timestamp, 1),
(21, 'm3', current_timestamp, 1),
(22, 'm4a1', current_timestamp, 1),
(23, 'tmp', current_timestamp, 1),
(24, 'g3sg1', current_timestamp, 1),
(25, 'flashbang', current_timestamp, 1),
(26, 'deagle', current_timestamp, 1),
(27, 'sg552', current_timestamp, 1),
(28, 'ak47', current_timestamp, 1),
(29, 'knife', current_timestamp, 1),
(30, 'p90', current_timestamp, 1);

create table weaponStats
(
    id int not null auto_increment primary key,
    
    gameFK int,
    playerFK int,
    weaponFK int,
    
    kills int,
    death int,
    headshot int,
    shot int,
    hit int,
    damage int,
    
    now datetime default current_timestamp,
    active int default 1,
    
    foreign key(gameFK) references game(id),
    foreign key(playerFK) references player(id),
    foreign key(weaponFK) references weapon(id)
);

