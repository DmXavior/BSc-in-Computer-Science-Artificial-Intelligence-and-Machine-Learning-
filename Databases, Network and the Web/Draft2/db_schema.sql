PRAGMA foreign_keys=ON;

BEGIN TRANSACTION;

-- Create users table
CREATE TABLE users (
    user_id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_name TEXT NOT NULL,
    blogtitle TEXT,
    blogsubtitle TEXT
);

-- Create email accounts table
CREATE TABLE email_accounts (
    email_account_id INTEGER PRIMARY KEY AUTOINCREMENT,
    email_address TEXT NOT NULL,
    user_id INTEGER NOT NULL,
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE
);

-- Create articles table
CREATE TABLE articles (
    articleid INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    datecreated DATE DEFAULT CURRENT_TIMESTAMP,
    datepublished DATE,
    datelastedited DATE DEFAULT CURRENT_TIMESTAMP,
    articletext TEXT,
    noofviews INTEGER DEFAULT 0,
    nooflikes INTEGER DEFAULT 0,
    articletitle TEXT,
    articlestatus TEXT DEFAULT 'Draft',
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE
);

-- Create comments table
CREATE TABLE comments (
    commentid INTEGER PRIMARY KEY AUTOINCREMENT,
    articleid INTEGER NOT NULL,
    datecommented DATE DEFAULT CURRENT_TIMESTAMP,
    commenttext TEXT,
    FOREIGN KEY (articleid) REFERENCES articles(articleid) ON DELETE CASCADE
);

-- Insert default data with blogtitle and blogsubtitle
INSERT INTO users (user_name, blogtitle, blogsubtitle) VALUES ('Simon Star', 'Simonion Blog', 'Exploring the Universe');
INSERT INTO users (user_name, blogtitle, blogsubtitle) VALUES ('Dianne Dean', 'Diannean Diary', 'A Journey Through Literature');
INSERT INTO users (user_name, blogtitle, blogsubtitle) VALUES ('Harry Hilbert', 'Harryfied Tech Corner', 'The Latest in Technology Trends');


INSERT INTO email_accounts (email_address, user_id) VALUES ('simon@gmail.com', 1); 
INSERT INTO email_accounts (email_address, user_id) VALUES ('simon@hotmail.com', 1); 
INSERT INTO email_accounts (email_address, user_id) VALUES ('dianne@yahoo.co.uk', 2); 

INSERT INTO articles (articleid, user_id, datecreated, datepublished, datelastedited, articletext, noofviews, nooflikes, articletitle, articlestatus) 
VALUES (1, 1, DATETIME('now'), NULL, DATETIME('now'), 'Lorem ipsum dolor sit amet, consectetur adipiscing elit.', 0, 0, 'My Journey into Astronomy', 'Draft');

INSERT INTO comments (articleid, commenttext) VALUES
(1, 'Great article, very informative!'),
(1, 'I enjoyed reading this.'),
(1, 'Interesting read, keep it up!');

COMMIT;