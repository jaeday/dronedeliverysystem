const express = require("express");
const Coffee = require('./models/coffeeModel')
const coffeesRoute = require('./routes/coffeesRoute')

const db = require("./db");

const app = express();

app.use(express.json());

app.get("/", (req,res) => {
    res.send("Server working " + port);
});

app.use('/api/coffees', coffeesRoute)

const port = process.env.PORT || 8000;

app.listen(port, () => 'server running on port port');