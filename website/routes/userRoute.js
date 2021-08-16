const express = require("express");
const router = express.Router();
const User = require('../models/userModel')

router.post("/register", (req,res)=>{
    const {name, email, password} = req.body
    const newUser = new User({name, email, password})
    try{
        newUser.save()
        res.send('User Registered Successfully')
    } catch (error) {
        return res.status(400).json({message: error});
    }
});

router.post("/login", async(req,res)=>{
    const {email, password} = req.body
    try{
        const user = await User.find({email, password})
        if(user.length > 0){
            res.send('User logged in successfully')
        }
        else {
            return res.status(400).json({message: 'User Login Failed'});
        }
    } catch (error) {
        return res.status(400).json({message: 'Something went wrong '});
    }
});

module.exports = router;