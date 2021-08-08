const mongoose = require("mongoose");

const coffeeSchema = mongoose.Schema({
    name : {type: String, require},
    sizes: [],
    prices: [],
    category: {type: String, require},
    image: {type: String, require},
    description: {type: String, require}
},{
    timestamps: true,
})

const coffeeModel = mongoose.model('coffees', coffeeSchema)

module.exports = coffeeModel