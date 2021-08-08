const mongoose = require("mongoose");

var mongoURL = 'mongodb+srv://shimmy:Sjh31217@cluster0.fpdlz.mongodb.net/coffeedelivery'

mongoose.connect(mongoURL, {useUnifiedTopology: true, useNewUrlParser: true})

var db = mongoose.connection

db.on('connected', () =>{
    console.log('Mongo DB Connection Successful')
})

db.on('error', () =>{
    console.log('Mongo DB Connection failed');
})

module.exports = mongoose