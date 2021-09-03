const express = require("express");
const Coffee = require('./models/coffeeModel')
const coffeesRoute = require('./routes/coffeesRoute')
const userRoute = require('./routes/userRoute')
const ordersRoute = require('./routes/ordersRoute')




const db = require("./db");

const app = express();

const path = require('path')

app.use(express.json());



app.use('/api/coffees/', coffeesRoute)
app.use('/api/users/', userRoute)
app.use('/api/orders/', ordersRoute)

if(process.env.NODE_ENV === 'production'){
    app.use('/', express.static('client/build'))
    app.get('*', (req, res)=>{
        res.sendFile(path.resolve(__dirname, '/client/build/index.html'))
    })
}


const port = process.env.PORT || 8000;

app.listen(port, () => 'server running on port port');