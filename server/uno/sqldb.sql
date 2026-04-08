create table deck(
    deckid int primary key,
    deckname text
);


create table game(
    gameid int primary key,
    code varchar(6),
    deckid int,
    creatorid int,
    currentPlayerIndex int,
    playerOrder text,
    isReversed int,
    foreign key(deckid) references deck(deckid)
);

create table player(
    playerid int primary key,
    username varchar(10),
    userpwd varchar(10),
    imagepath varchar(100),
    joinedgameid int,
    createdgameid int
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
    cardcolor varchar(6)
);

create table usercard(
    playerid int,
    cardid int,
    primary key(playerid,cardid),
    foreign key(playerid) references player(playerid),
    foreign key(cardid) references gamecard(cardid)
);

create table drawpile(
    drawpileid int primary key,
    gameid int,
    foreign key(gameid) references game(gameid)
);

create table drawpilecard(
    drawpileid int,
    cardid int,
    primary key(drawpileid,cardid),
    foreign key(drawpileid) references drawpile(drawpileid),
    foreign key(cardid) references gamecard(cardid)
);

create table playedpile(
    playedpileid int primary key,
    gameid int,
    foreign key(gameid) references game(gameid)
);

create table playedpilecard(
    playedpileid int,
    cardid int,
    primary key(playedpileid,cardid),
    foreign key(playedpileid) references playedpile(playedpileid),
    foreign key(cardid) references gamecard(cardid)
);

