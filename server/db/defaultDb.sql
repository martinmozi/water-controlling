CREATE TABLE `interval` (
	`id`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
	`name`	TEXT,
	`_from`	INTEGER NOT NULL,
	`_to`	INTEGER NOT NULL,
	`days`	INTEGER NOT NULL,
	`active`	INTEGER NOT NULL,
	`gpioindex`	INTEGER
);

CREATE TABLE `gpiostatus` (
	`gpioindex`, INTEGER NOT NULL,
	`status`	INTEGER NOT NULL
);
