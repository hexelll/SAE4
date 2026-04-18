create table deck(
    deckid int primary key,
    deckname text
);


create table game(
    gameid int primary key,
    code varchar(6),
    deckid int,
    creatorid int,
    currentplayerindex int,
    isreversed int,
    foreign key(deckid) references deck(deckid)
);

create table player(
    playerid int primary key,
    username varchar(10),
    userpwd varchar(10),
    imagepath varchar(100),
    joinedgameid int,
    createdgameid int,
    gameindex int
);

create table rule(
    ruleid int primary key,
    rulename varchar(15),
    ruledescription varchar(100),
    rulevalue int
);

create table ruleset(
    rulesetid int primary key,
    rulesetname varchar(10),
    creatorid int,
    foreign key(creatorid) references player(playerid)
);

create table cardtype(
    cardtypeid int primary key,
    carddesc varchar(100)
);

create table gamecard(
    cardid int primary key,
    cardvalue int,
    cardcolorid int,
    cardtypeid int,
    foreign key(cardcolorid) references cardcolor(cardcolorid),
    foreign key(cardtypeid) references cardtype(cardtypeid)
);

create table usercard(
    playerid int,
    cardid int,
    primary key(playerid,cardid),
    foreign key(playerid) references player(playerid),
    foreign key(cardid) references gamecard(cardid)
);

create table cardcolor(
    cardcolorid int primary key,
    color varchar(6)
);

create table drawpilecard(
    gameid int,
    cardid int,
    primary key(gameid,cardid),
    foreign key(gameid) references game(gameid),
    foreign key(cardid) references gamecard(cardid)
);

create table playedpilecard(
    gameid int,
    cardid int,
    cardindex int,
    primary key(gameid,cardid),
    foreign key(gameid) references game(gameid),
    foreign key(cardid) references gamecard(cardid)
);

create table deckcard(
    deckid int,
    cardid int,
    primary key(deckid,cardid),
    foreign key(deckid) references deck(deckid),
    foreign key(cardid) references gamecard(cardid)
);