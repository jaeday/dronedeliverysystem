const express = require("express");
const router = express.Router();
const stripe = require("stripe")("sk_test_51JPoR1GdJegYvYHwkG18rZQAWlMISZIl8M6UldYtgAdcqNA07szyG0UJGr7189JmOsTN0h65rGckglC1iR1WwxYw00pBwpjMWo")
const {v4: uuidv4} = require('uuid')
const Order = require('../models/orderModel')
//const dest = localStorage.getItem('location')
router.post("/placeorder", async(req, res) => {
    const{token, subtotal, currentUser, cartItems, dest} = req.body
    try{
        const customer = await stripe.customers.create({
            email: token.email,
            source: token.id
        })

        const payment = await stripe.charges.create({
            amount : subtotal*100,
            currency: 'usd',
            customer: customer.id,
            receipt_email: token.email
        }, {
            idempotencyKey: uuidv4()
        })

        if(payment){
            //var dest = localStorage.getItem('location')
            const neworder = new Order({
                name: currentUser.name,
                email: currentUser.email,
                userid: currentUser._id,
                orderItems: cartItems,
                orderAmount: subtotal,
                shippingAddress : dest,
                transactionid: payment.source.id
            })
            neworder.save()
            //console.log(dest)
            res.send('Order placed successfully')
        }
        else{
            res.send('Payment Failed')
        }
    } catch (error) {
        console.log(error);
        return res.status(400).json({message: 'Something went wrong '});
    }
})

router.post("/getuserorders", async(req,res) => {
    const {userid} = req.body
    try{
        const orders = await Order.find({userid: userid}).sort({_id:-1})
        res.send(orders)
    }catch (error){
        return res.status(400).json({message: 'Something went wrong '});
    }
});

module.exports = router;