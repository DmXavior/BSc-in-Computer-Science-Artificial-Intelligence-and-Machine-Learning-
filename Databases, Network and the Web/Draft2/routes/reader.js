const express = require("express");
const router = express.Router();

/**
 * @desc Displays a page with details for reader homepage
 */
router.get("/reader-home", (req, res) => {
    res.render("reader-home.ejs");
});

// /**
//  * @desc Displays a page with details for reader articles
//  */
// router.get("/reader-article", (req, res) => {
//     res.render("reader-article.ejs");
// });

// Route to render reader articles page
router.get('/reader-article', (req, res) => {
    // Fetch articles from the database
    const query = `
        SELECT articles.articleid, articles.articletitle, articles.articletext, articles.datepublished, users.user_name
        FROM articles
        INNER JOIN users ON articles.user_id = users.user_id
        WHERE articles.articlestatus = 'Published'
        ORDER BY articles.datepublished DESC
    `;

    global.db.all(query, [], (err, articles) => {
        if (err) {
            console.error(err);
            return res.status(500).send('Error fetching articles');
        }

        // Fetch comments for each article
        const fetchCommentsPromises = articles.map(article => {
            return new Promise((resolve, reject) => {
                const query = `
                    SELECT commenttext
                    FROM comments
                    WHERE articleid = ?
                `;

                global.db.all(query, [article.articleid], (err, comments) => {
                    if (err) {
                        reject(err);
                    } else {
                        article.comments = comments;
                        resolve();
                    }
                });
            });
        });

        // Resolve all promises and render the page with fetched data
        Promise.all(fetchCommentsPromises)
            .then(() => {
                res.render('reader-article.ejs', { articles });
            })
            .catch(err => {
                console.error(err);
                res.status(500).send('Error fetching comments');
            });
    });
});


// Export the router object so index.js can access it
module.exports = router;
