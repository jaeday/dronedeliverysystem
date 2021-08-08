const express = require("express");
const Coffee = require('./models/coffeeModel')

const db = require("./db");

const app = express();

app.use(express.json());

app.get("/", (req,res) => {
    res.send("Server working " + port);
});

app.get("/getcoffees", (req,res) => {
    Coffee.find({}, (err, docs)=>{
        if(err){
            console.log(err);
        }
        else{
            res.send(docs);
        }
    })
});

const port = process.env.PORT || 8000;

app.listen(port, () => 'server running on port port');