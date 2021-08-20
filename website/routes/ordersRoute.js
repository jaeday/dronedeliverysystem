const express = require("express");
const router = express.Router();
const stripe = require("stripe")("sk_test_51JPoR1GdJegYvYHwkG18rZQAWlMISZIl8M6UldYtgAdcqNA07szyG0UJGr7189JmOsTN0h65rGckglC1iR1WwxYw00pBwpjMWo")
const {v4: uuidv4} = require('uuid')
const Order = require('../models/orderModel')
//const dest = localStorage.getItem('location')
router.post("/placeorder", async(req, res) => {
    const{token, subtotal, currentUser, cartItems} = req.body
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
                userid: currentUser.userid,
                orderItems: cartItems,
                orderAmount: subtotal,
                shippingAddress :{
                    street: token.card.address_line1,
                    city: token.card.address_city,
                    country: token.card.address_country,
                    zipcode: token.card.address_zip
                },
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
        return res.status(400).json({message: 'Something went wrong '});
    }
})

module.exports = router;