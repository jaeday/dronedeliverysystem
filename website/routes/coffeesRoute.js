const express = require("express");
const router = express.Router();
const Coffee = require('../models/coffeeModel')

router.get("/getallcoffees", async(req, res) => {
    try {
        const coffees = await Coffee.find({})
        res.send(coffees)
    } catch (error) {
        return res.status(400).json({message: error});
    }
})

module.exports = router;