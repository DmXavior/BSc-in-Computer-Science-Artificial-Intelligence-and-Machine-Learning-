/**
* index.js
* This is your main app entry point
*/

// Set up express, bodyparser and EJS
const express = require('express');
const app = express();
const port = 3000;
var bodyParser = require("body-parser");

// Body parser middleware to parse incoming request bodies
app.use(bodyParser.urlencoded({ extended: true }));

// // Session middleware setup
// app.use(session({
//     secret: 'pauliepooh88!!', // a long random string for production
//     resave: false,
//     saveUninitialized: true,
//     cookie: { secure: false } // Set to true if using HTTPS
// }));

// Set view engine to use EJS for rendering
app.set('view engine', 'ejs'); // set the app to use ejs for rendering

// Set view engine to use EJS for rendering
app.use(express.static(__dirname + '/public')); // set location of static files

// Set up SQLite
// Items in the global namespace are accessible throught out the node application
const sqlite3 = require('sqlite3').verbose();
global.db = new sqlite3.Database('./database.db',function(err){
    if(err){
        console.error(err);
        process.exit(1); // bail out we can't connect to the DB
    } else {
        console.log("Database connected");
        global.db.run("PRAGMA foreign_keys=ON"); // tell SQLite to pay attention to foreign key constraints
    }
});

// Handle requests to the home page 
app.get('/', (req, res) => {
    res.render('index')
});

// Add Login Routes
app.get('/login', (req, res) => {
    res.render('login'); // Renders your login.ejs file
});

// Login route - POST request to handle form submission
app.post('/login', (req, res) => {
    const { user_name } = req.body;

    // Query the database to check if user_name exists
    global.db.get('SELECT * FROM users WHERE user_name = ?', [user_name], (err, row) => {
        if (err) {
            console.error(err);
            res.status(500).send('Internal Server Error');
        } else {
            if (row) {
                // User found, redirect to author home page with user_name as query parameter
                res.redirect(`/author/author-home?username=${encodeURIComponent(user_name)}`);
            } else {
                // User not found, handle invalid login
                res.status(401).send('Invalid username');
            }
        }
    });
});

// Add all the route handlers in usersRoutes to the app under the path /users
const usersRoutes = require('./routes/users');
app.use('/users', usersRoutes);

// Add all the route handlers in authorRoutes to the app under the path /author
const authorRoutes = require('./routes/author');
app.use('/author', authorRoutes);

// Add all the route handlers in readerRoutes to the app under the path /reader
const readerRoutes = require('./routes/reader');
app.use('/reader', readerRoutes);

// Make the web application listen for HTTP requests
app.listen(port, () => {
    console.log(`Example app listening on port ${port}`)
})

