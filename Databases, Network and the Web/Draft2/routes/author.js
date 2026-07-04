const express = require("express");
const router = express.Router();


/**
 * @desc Displays a page with details for author homepage
 */
router.get("/author-home", (req, res) => {
    res.render("author-home.ejs");
});

// Route to render author home page
router.get('/author-home', (req, res) => {
    const userName = req.query.username; // Retrieve username from query parameter

    // Render author home page with user name
    res.render('author-home', { userName });
});


// Route to render the settings page
router.get('/author-settings', (req, res) => {
    const userName = req.query.username; // Retrieve username from query parameter

    // Query database to fetch user settings based on user_name
    const query = 'SELECT blogtitle FROM users WHERE user_name = ?';
    global.db.get(query, [userName], (err, row) => {
        if (err) {
            console.error(err);
            return res.status(500).send('Error fetching user settings');
        }

        if (!row) {
            return res.status(404).send('User not found');
        }

        const { blogtitle } = row;
        res.render('author-settings', { userName, blogtitle });
    });
});


// Route to handle form submission and update settings
router.post('/author/update-settings', (req, res) => {
    const { userName, blogTitle } = req.body;

    // Update blogtitle in the database
    const query = 'UPDATE users SET blogtitle = ? WHERE user_name = ?';
    global.db.run(query, [blogTitle, userName], function(err) {
        if (err) {
            console.error(err);
            return res.status(500).send('Failed to update blog title');
        }

        console.log(`Blog title updated successfully for ${userName}`);
        res.redirect('/author/author-home'); // Redirect to author home page after update
    });
});


/**
 * @desc Displays a page with details for author login
 */
router.get("/login", (req, res) => {
    res.render("login.ejs");
});

/**
 * @desc Displays a page with details for author edit
 */
router.get("/author-edit", (req, res) => {
    res.render("author-edit.ejs");
});

// Export the router object so index.js can access it
module.exports = router;
