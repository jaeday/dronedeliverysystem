const mongoose = require("mongoose")

const orderSchema = mongoose.Schema({
    name : {type: String, require},
    email : {type: String, require},
    userid : {type: String, require},
    orderItems : [],
    shippingAddress : {type: String, require},
    orderAmount: { type: Number, require},
    isDelivered : {type:Boolean, require, default: false},
    transactionid :{type: String, require}
}, {
    timestamps : true
})

module.exports = mongoose.model('orders' , orderSchema)